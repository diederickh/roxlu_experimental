#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <av/AVDecoder.h>

// ---------------------------------------------

AVDecoderFrame::AVDecoderFrame()
  :frame(NULL)
  ,pts(0)
  ,type(AV_TYPE_NONE)
{
}

AVDecoderFrame::~AVDecoderFrame() {
  // @TODO free up!
  RX_VERBOSE("CLEAN/RESET AVDECODERFRAME");
}

// ---------------------------------------------

AVDecoder::AVDecoder() 
  :format_context(NULL)
  ,video_stream(NULL)
  ,video_codec(NULL)
  ,video_codec_context(NULL)
  ,video_time_millis(0)
{
  avcodec_register_all();
}

AVDecoder::~AVDecoder() {
  RX_VERBOSE("CORRECTLY SHUTDOWN + CLEAN ALL LIBAV CONTEXTS + REMOVE AVDECODERFRAMES");
}

bool AVDecoder::open(std::string filename, bool datapath) { 
  int r = 0;
  char err_msg[512];

  if(datapath) {
    filename = rx_to_data_path(filename);
  }
  
  format_context = avformat_alloc_context();
  if(!format_context) {
    RX_ERROR(ERR_AVD_ALLOC_FORMAT_CTX);
    return false;
  }

  r = avformat_open_input(&format_context, filename.c_str(), NULL, NULL);
  if(r < 0) {
    RX_ERROR(ERR_AVD_OPEN_INPUT, filename.c_str(), av_strerror(r, err_msg, sizeof(err_msg)));
    return false;
  }

  r = avformat_find_stream_info(format_context, NULL);
  if(r < 0) {
    RX_ERROR(ERR_AVD_STREAM_INFO);
    return false;
  }

  // get the video stream.
  for(unsigned int i = 0; i < format_context->nb_streams; ++i) {
    AVCodecContext* codec_context = format_context->streams[i]->codec;
    const AVCodecDescriptor* codec_desc = avcodec_descriptor_get(codec_context->codec_id);
    if(codec_desc) {
      RX_VERBOSE("Found stream: %d, codec: %s", i, codec_desc->name);
    }

    if(codec_context->codec_type == AVMEDIA_TYPE_VIDEO) {
      video_stream = format_context->streams[i];
      video_time_millis = (double(video_stream->time_base.num) / video_stream->time_base.den) * 1000;
      RX_VERBOSE("VIDEO STREAM TIME BASE: %f", video_time_millis);
      break;
    }
  }

  if(!video_stream) {
    RX_ERROR(ERR_AVD_NO_VIDEO_STREAM);
    return false;
  }


  video_codec = avcodec_find_decoder(video_stream->codec->codec_id);
  if(!video_codec) {
    RX_ERROR(ERR_AVD_FIND_VIDEO_DECODER);
    return false;
  }

  // @TODO @Paranoialmaniac in #libav-devel told me I don't need to allocate a codec context because it's already allocated by avformat_find_stream_info
  // @TODO call avcodec_close(), avfree(video_codec_context) when stopping ! 
  video_codec_context = avcodec_alloc_context3(video_codec);
  if(!video_codec_context) {
    RX_ERROR(ERR_AVD_ALLOC_VIDEO_CONTEXT);
    return false;
  }

  // copy the extra data. @TODO where (and) do I need to free this?
  if(video_stream->codec->extradata_size) {
    video_codec_context->extradata = (uint8_t*)av_malloc(video_stream->codec->extradata_size);
    video_codec_context->extradata_size = video_stream->codec->extradata_size;

    memcpy(video_codec_context->extradata, 
           video_stream->codec->extradata, 
           video_stream->codec->extradata_size);
  }

  r = avcodec_open2(video_codec_context, video_codec, NULL);
  if(r < 0) {
    RX_ERROR(ERR_AVD_OPEN_VIDEO_CONTEXT, av_strerror(r, err_msg, sizeof(err_msg)));
    return false;
  }

  return true;
}

bool AVDecoder::close() {
  if(!format_context) {
    RX_VERBOSE(ERR_AVD_CLOSE_NO_FORMAT_CTX);
    return false;
  }

  // @todo close the decoder .. set everything to the same state as when we were just created */
  RX_ERROR("CLEAR ALL THINGS WE'VE ALLOCATED!!");
  if(video_stream) {

  }
  return true; 
}

AVDecoderFrame* AVDecoder::decodeFrame(bool& isEOF) {
  isEOF = false;

  AVDecoderFrame* decoder_frame = new AVDecoderFrame();
  decoder_frame->frame =  avcodec_alloc_frame();
  if(!decoder_frame->frame) {
    RX_ERROR(ERR_AVD_ALLOC_FRAME);
    delete decoder_frame;
    return NULL;
  }
  
  AVPacket packet;                                                                  // @TODO where do I need to free it
  av_init_packet(&packet);                                                          // is this really necessary? (not done here: http://blog.tomaka17.com/2012/03/libavcodeclibavformat-tutorial/)
    
  if(av_read_frame(format_context, &packet) < 0) {
    RX_VERBOSE(V_AVD_EOF);                                                          // @TODO clean up packet (?)
    isEOF = true;
    delete decoder_frame;
    return NULL;
  }

  if(packet.stream_index == video_stream->index) {
    int got_picture = 0;
    int bytes_used = avcodec_decode_video2(video_codec_context, decoder_frame->frame, &got_picture, &packet);
    if(got_picture) {
      decoder_frame->pts = video_time_millis * decoder_frame->frame->pkt_pts;
      decoder_frame->type = AV_TYPE_VIDEO;
    }
    RX_VERBOSE("PTS: %ld, PACKET.PTS: %ld, WIDTH: %d, PICTURE: %d", decoder_frame->frame->pkt_pts, packet.pts, decoder_frame->frame->width, got_picture);
  }
  else {
    RX_VERBOSE(V_AVD_STREAM_NOT_USED, packet.stream_index);
  }

  return decoder_frame;
}

void AVDecoder::print() {
  if(video_stream) {
    RX_VERBOSE("AVDecoder.getWidth(): %d", getWidth());
    RX_VERBOSE("AVDecoder.getHeight(): %d", getHeight());
    RX_VERBOSE("AVDecoder.video_stream.codec.time_base.den: %d", video_stream->codec->time_base.den);
    RX_VERBOSE("AVDecoder.video_stream.codec.time_base.num: %d", video_stream->codec->time_base.num);
    RX_VERBOSE("AVDecoder.video_stream.time_base.den: %d", video_stream->time_base.den);
    RX_VERBOSE("AVDecoder.video_stream.time_base.num: %d", video_stream->time_base.num);

    // name of the video codec
    const AVCodecDescriptor* codec_desc = avcodec_descriptor_get(video_stream->codec->codec_id);
    if(codec_desc) {
      RX_VERBOSE("AVDecoder.video_stream codec: %s",codec_desc->name);
    }  

    // name of pixel format 
    const AVPixFmtDescriptor* pix_desc = av_pix_fmt_desc_get(video_stream->codec->pix_fmt);
    if(pix_desc) {
      RX_VERBOSE("AVDecoder.video_stream.codec.pix_fmt: %s", pix_desc->name);
    }
  }
}

