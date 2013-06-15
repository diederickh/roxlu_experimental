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
  pts = 0;
  type = AV_TYPE_NONE;

  if(frame) {
    av_frame_unref(frame);
    avcodec_free_frame(&frame);          
    frame = NULL;
  }

}

// ---------------------------------------------

AVDecoder::AVDecoder() 
  :format_context(NULL)
  ,video_stream(NULL)
  ,video_codec(NULL)
  ,video_codec_context(NULL)
  ,video_time_millis(0)
{
  rx_init_libav();
}

AVDecoder::~AVDecoder() {
  close();
}

bool AVDecoder::open(AVFormatContext* ctx) {
  RX_VERBOSE("Open using custom context");
  int r = 0;
  char err_msg[512] = {0};

  if(format_context) {
    RX_ERROR("It looks like you're trying to open a decoder which is already opened");
    return false;
  }

  format_context = ctx;

  r = avformat_open_input(&format_context, "foo.h264", NULL, NULL);
  if(r < 0) {
    RX_ERROR("Error while opening the formatcontext: %s", av_strerror(r, err_msg, sizeof(err_msg)));
    return false;
  }

  return true;
}

bool AVDecoder::open(std::string filename, bool datapath) { 
  int r = 0;
  char err_msg[512];

  if(format_context) {
    RX_ERROR(ERR_AVD_ALREADY_OPENED);
    return false;
  }

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
    RX_ERROR("%d", r);
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

  video_codec_context = avcodec_alloc_context3(video_codec);
  if(!video_codec_context) {
    RX_ERROR(ERR_AVD_ALLOC_VIDEO_CONTEXT);
    return false;
  }


  video_codec_context->refcounted_frames = 1;   // see reference of avcodec_decode_video2

#if 0
  // There seems to be a bug in libav regarding copy_context and avcodec_close. 
  // avcodec_close is not freeing the copied extradata in case, because it checks
  // if the avformatcontext.codec is an encoder. Because we have a decoder,
  // the extra data isn't freed().
  // see: https://gist.github.com/roxlu/5f0bbed4da1a52c971c4#file-utils-c-L1464-L1465
  r = avcodec_copy_context(video_codec_context, video_stream->codec);
  if(r != 0) {
    RX_ERROR(ERR_AVD_COPY_VIDEO_CONTEXT, av_strerror(r, err_msg, sizeof(err_msg)));
    return false;
  }
#else 
  if(video_stream->codec->extradata_size) {
    video_codec_context->extradata = (uint8_t*)av_malloc(video_stream->codec->extradata_size);
    video_codec_context->extradata_size = video_stream->codec->extradata_size;

    memcpy(video_codec_context->extradata, 
           video_stream->codec->extradata, 
           video_stream->codec->extradata_size);
  }

#endif
  r = avcodec_open2(video_codec_context, video_codec, NULL);
  if(r < 0) {
    RX_ERROR(ERR_AVD_OPEN_VIDEO_CONTEXT, av_strerror(r, err_msg, sizeof(err_msg)));
    return false;
  }

  return true;
}

bool AVDecoder::close() {

  if(format_context) {
    avformat_close_input(&format_context);
    format_context = NULL;
  }

  if(video_codec_context) {
    av_freep(&video_codec_context->extradata);
    avcodec_close(video_codec_context);
    av_free(video_codec_context);
    video_codec_context = NULL;
  }

  video_stream = NULL;
  video_codec = NULL; 
  video_time_millis = 0;

  return true; 
}

AVDecoderFrame* AVDecoder::decodeFrame(bool& isEOF) {
  isEOF = false;

  AVDecoderFrame* decoder_frame = new AVDecoderFrame();
  decoder_frame->frame = avcodec_alloc_frame();
  if(!decoder_frame->frame) {
    RX_ERROR(ERR_AVD_ALLOC_FRAME);
    delete decoder_frame;
    return NULL;
  }
  
  AVPacket packet;    
  av_init_packet(&packet);

  if(av_read_frame(format_context, &packet) < 0) {
    RX_VERBOSE(V_AVD_EOF);         
    isEOF = true;         
    delete decoder_frame;
    av_free_packet(&packet); 
    return NULL;
  }

  if(packet.stream_index == video_stream->index) {

    int got_picture = 0;
    int bytes_used = avcodec_decode_video2(video_codec_context, decoder_frame->frame, &got_picture, &packet);
    if(got_picture) {
      decoder_frame->pts = video_time_millis * decoder_frame->frame->pkt_pts;
      decoder_frame->type = AV_TYPE_VIDEO;
      
    }
  }
  else {
    // this is where we could handle other streams (like audio / subtitle)
    // RX_VERBOSE(V_AVD_STREAM_NOT_USED, packet.stream_index);
    delete decoder_frame;
    decoder_frame = NULL;
  }

  av_free_packet(&packet);   

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

