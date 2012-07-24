#include "AV.h"

AVContext::AVContext()
	:c(NULL)
	,of(NULL)
	,vs(NULL)
	,vbuf(NULL)
	,vframe(NULL)
	,tmp_vframe(NULL)
	,sws(NULL)
	,vsize(0)
{
}

AV::AV()
	:src_w(0)
	,src_h(0)
	,can_add_frames(false)
	
{
	av_register_all();
	avformat_network_init();
}

AV::~AV() {
	closeVideo(ct);
}

/**
 * @param 	int srcW		Source width (size of input image)
 * @param	int srcH		Source height
 */
bool AV::setup(const int srcW, const int srcH) {
	src_w = srcW;
	src_h = srcH;
	
	// setup libav internals.
	if(!setupAV()) {
		printf("Cannot setup libav internals.\n");
		return false;
	}

	// setup SWS we use to convert RGB to YUV
	if(!setupSWS()) {
		printf("Cannot setup sws\n");
		return false;
	}	
	
	can_add_frames = true;
	return true;
}


// add a new video frame
bool AV::addFrame(unsigned char* pixels) {
	if(!can_add_frames) {
		return false;
	}
	
	// Fill and resize our input image to the destination size
	avpicture_fill((AVPicture*)ct.tmp_vframe, pixels, PIX_FMT_RGB24, src_w, src_h);
	int out_h = sws_scale(
		ct.sws
		,(const uint8_t* const*)((AVPicture*)ct.tmp_vframe)->data
		,ct.tmp_vframe->linesize
		,0
		,src_h
		,ct.vframe->data
		,ct.vframe->linesize
	);


	// create packet to encode
	AVPacket packet;
	av_init_packet(&packet);
	
	if(ct.vs->codec->coded_frame->pts != AV_NOPTS_VALUE) {
	    packet.pts = av_rescale_q(
						 ct.vs->codec->coded_frame->pts
						,ct.vs->codec->time_base
						,ct.vs->time_base
		);
	}

	
	if(ct.vs->codec->coded_frame->key_frame) {
		packet.flags |= AV_PKT_FLAG_KEY;
	}
	
	packet.stream_index = ct.vs->index;
	packet.data = ct.vbuf;
	packet.size = ct.vsize;
	
	int result = 0;
	int encode_result = avcodec_encode_video2(ct.vs->codec, &packet, ct.vframe, &result);
	if(encode_result != 0) {
		printf("Can't encode the video. result: %d\n", encode_result);
		return false;
	}
	
	if(av_interleaved_write_frame(ct.c, &packet) != 0) {
		printf("Error while writing out a frame.\n");
		return false;
	}
	
	printf("out_h: %d\n", out_h);
	return true;
}

bool AV::save() {
	av_write_trailer(ct.c);
	can_add_frames = false; // after saving we can't add any more frames.
}


// Private
// -----------------------------------------------------------------------------

// setup SWS we use for image conversion
bool AV::setupSWS() {
	if(ct.vs == NULL) {
		printf("Call setupSWS after calling setupAV!\n");
		return false;
	}
	
	ct.sws = sws_getContext(
		 src_w						// input w
		,src_h						// input h
		,PIX_FMT_RGB24				// input format
		,ct.vs->codec->width		// output w
		,ct.vs->codec->height		// output h
		,ct.vs->codec->pix_fmt		// output format	
		,SWS_BICUBIC				// scaling method
		,NULL
		,NULL
		,NULL
	);
	
	if(ct.sws == NULL) {
		return false;
	}
	
	return true;
}


// setup libAV related structs.
bool AV::setupAV() {
	// Create output format 
	ct.of = av_guess_format(NULL, "roxlu.flv", NULL);
	if(!ct.of) {
		printf("Cannot create flv AVOutputFormat\n");
		return false;
	}
	
	// Create the main libav context.
	ct.c = avformat_alloc_context();
	if(!ct.c) {
		printf("Cannot allocate the AVFormatContext\n");
		return false;
	}
	
	// Tell our main context what format to use.
	ct.c->oformat = ct.of;
	
	// Set the output type for our main context.
	const char* output_filename = "tcp://127.0.0.1:6666";
//	const char* output_filename = "test.flv";
	snprintf(ct.c->filename, sizeof(ct.c->filename), "%s", output_filename);
	
	// add the video stream.
	ct.vs = addVideoStream(ct, ct.of->video_codec);
	if(!ct.vs) {
		printf("Cannot create video stream: %d.\n", ct.of->video_codec);
		return false;
	}
	
	// open the video stream and init encoder
	if(!openVideo(ct)) {
		printf("Cannot open video stream.\n");
		return false;
	}
	
	// show some info
	av_dump_format(ct.c, 0, output_filename, 1);
	
	// open the output file if needed
	if(!(ct.of->flags & AVFMT_NOFILE)) {
		if(avio_open(&ct.c->pb, output_filename, AVIO_FLAG_WRITE) < 0) {
			printf("Cannot open: %s\n", output_filename);
			return false;
		}
	}
	
	// write header.
	avformat_write_header(ct.c, NULL);
	
	return true;
}

AVStream* AV::addVideoStream(AVContext& context, enum CodecID codecID) {
	AVCodec* codec = NULL;
	AVStream* stream = NULL;
	
	codec = avcodec_find_encoder(codecID);
	if(!codec) {
		printf("Cannot find codec with ID: %d\n", codecID);
		return NULL;
	}
	
	stream = avformat_new_stream(context.c, codec);
	if(!stream) {
		printf("Cannot create stream.\n");
		return NULL;
	}
	
	// sample params
	stream->codec->bit_rate = 400000;
	stream->codec->width = src_w; //  @todo change this to output width
	stream->codec->height = src_h; //  @todo change this to output height
	stream->codec->time_base.den = 30; 
	stream->codec->time_base.num = 1;
	stream->codec->gop_size = 30; // still necessary (?)
	stream->codec->codec_id = context.of->video_codec;
	stream->codec->codec_type = AVMEDIA_TYPE_VIDEO;
	stream->codec->pix_fmt = PIX_FMT_YUV420P;
	
	//if(stream->codec->codec_id == CODEC_ID_MPEG2VIDEO
	if(codecID == CODEC_ID_MPEG2VIDEO) {
		stream->codec->max_b_frames = 2;
	}
	
	if(codecID == CODEC_ID_MPEG1VIDEO) {
		stream->codec->mb_decision = 2;
	}
	
	// separate stream headers? for output muxer
	if(context.c->oformat->flags & AVFMT_GLOBALHEADER) {
		stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}
	
	return stream;
}

bool AV::openVideo(AVContext& context) {
	// open the encoder.
	if(avcodec_open2(context.vs->codec, NULL, NULL) < 0) {
		printf("Cannot open the encoder.\n");
		return false;
	}
	
	context.vbuf = NULL;
	//if(!(context.c->oformat->flags & AVFMT_RAWPICTURE)) {
	//	printf("Allocating video buffer when it's not a raw picture (?)\n");
		context.vsize = 200000;
		context.vbuf = (uint8_t*) av_malloc(context.vsize);
	//}
	
	// allocate encoded raw image.
	context.vframe = allocFrame(context.vs->codec->pix_fmt, context.vs->codec->width, context.vs->codec->height); // we could use "w" and "h", this seems more versatile
	if(!context.vframe) {
		printf("Cannot allocate memory for vframe.\n");
		return false;
	}
	
	// allocate RGB image (used for conversion)
	//  context.vs->codec->width, context.vs->codec->height); // we could use "w" and "h", this seems more versatile
	context.tmp_vframe = allocFrame(PIX_FMT_RGB24, src_w, src_h);
	if(!context.tmp_vframe) {
		printf("Cannot allocate memory for tmp_vframe we need for conversion.\n");
		return false;
	}	
	return true;
}

void AV::closeVideo(AVContext& context) {
	avcodec_close(context.vs->codec);
	av_free(context.vframe->data[0]);
	av_free(context.tmp_vframe->data[0]);
	av_free(context.vbuf);
}

AVFrame* AV::allocFrame(enum PixelFormat pixFormat, int width, int height) {
	AVFrame* pic = NULL;
	uint8_t* buf = NULL;
	int size = 0;
	
	pic = avcodec_alloc_frame();
	if(!pic) {
		printf("Cannot allocate frame.\n");
		return NULL;
	}
	
	// @todo figure out who should maintain ownership of "buf". I think the AVContext
	// @BBB-work on #libav-devel says "yes libav frees it as you close the decoder"
	size = avpicture_get_size(pixFormat, width, height);
	buf = (uint8_t*) av_malloc(size); // memory leak here? who owns this?
	
	avpicture_fill((AVPicture*)pic, buf, pixFormat, width, height);
	
	return pic;
}

