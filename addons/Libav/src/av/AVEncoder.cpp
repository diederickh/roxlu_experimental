#include <roxlu/core/Utils.h>
#include <roxlu/core/Log.h>
#include <av/AVEncoder.h>
#include <assert.h>

AVEncoder::AVEncoder() 
  :output_format(NULL)
  ,format_context(NULL)
  ,time_started(0)
  ,millis_per_frame(0)
  ,new_frame_timeout(0)
  ,audio_codec_context(NULL)
  ,audio_codec(NULL)
  ,audio_stream(NULL)
  ,audio_frame(NULL)
  ,video_codec(NULL)
  ,video_codec_context(NULL)
  ,video_stream(NULL)
  ,video_frame_out(NULL)
  ,sws(NULL)
{
  rx_init_libav();
}

AVEncoder::~AVEncoder() {
  RX_ERROR("MAKE SURE THAT ALL ALLOCATED OBJECTS ARE FREED HERE!!");
}

bool AVEncoder::setup(AVEncoderSettings cfg) {

  if(!cfg.validate()) {
    return false;
  }

  millis_per_frame = (cfg.time_base_num / cfg.time_base_den) * 1000;
  if(!millis_per_frame) {
    RX_ERROR(ERR_AV_WRONG_MILLIS_FPS);
    return false;
  }

  settings = cfg;

  return true;
}

bool AVEncoder::start(std::string filename, bool datapath) {
  if(output_format) {
    RX_ERROR(ERR_AV_ALREADY_STARTED);
    return false;
  }

  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  output_format = av_guess_format(NULL, filename.c_str(), NULL);
  if(!output_format) {
    RX_ERROR(ERR_AV_OUTPUT_FMT);
    return false;
  }

  format_context = avformat_alloc_context();
  if(!format_context) {
    RX_ERROR(ERR_AV_FORMAT_CONTEXT);
    return false;
  }
  format_context->oformat = output_format;

  if(output_format->video_codec == AV_CODEC_ID_NONE) {
    RX_ERROR(ERR_AV_NO_VIDEO_CODEC);
    return false;
  }

  if(!addVideoStream(output_format->video_codec)) {
    return false;
  }

  if(!openVideo()) {
    return false;
  }

  if(!initializeSWS()) {
    return false;
  }

  av_dump_format(format_context, 0, filename.c_str(), 1);

  if(!(output_format->flags & AVFMT_NOFILE)) {
    if(avio_open(&format_context->pb, filename.c_str(), AVIO_FLAG_WRITE) < 0) {
      RX_ERROR(ERR_AV_OPEN_FILE);
      return false;
    }
  }

  avformat_write_header(format_context, NULL);
  time_started = rx_millis();
  return true;
}

bool AVEncoder::initializeSWS() {
  assert(video_codec_context);
#if 0
  if(settings.in_pixel_format != video_codec_context->pix_fmt
     || settings.in_w != settings.out_w
     || settings.in_h != settings.out_h) 
    {
#endif

      sws = sws_getContext(settings.in_w, settings.in_h, settings.in_pixel_format,
                           settings.out_w, settings.out_h, video_codec_context->pix_fmt,
                           SWS_FAST_BILINEAR, NULL, NULL, NULL);
      if(!sws) {
        RX_ERROR(ERR_AV_SWS);
        return false;
      }

#if 0
  }
#endif

  return true;
}

bool AVEncoder::addVideoStream(enum AVCodecID codecID) {
  assert(output_format);
  assert(format_context);

  video_codec = avcodec_find_encoder(codecID);
  if(!video_codec) {
    RX_ERROR(ERR_AV_VIDEO_ENC_NOT_FOUND);
    return false;
  }

  video_stream = avformat_new_stream(format_context, video_codec);
  if(!video_stream) {
    RX_ERROR(ERR_AV_VIDEO_STREAM);
    return false;
  }

  video_codec_context = video_stream->codec;

  listSupportedVideoCodecPixelFormats();

  video_codec_context->bit_rate = 4000000;
  video_codec_context->width = settings.out_w;
  video_codec_context->height = settings.out_h;
  video_codec_context->time_base.den = settings.time_base_den;
  video_codec_context->time_base.num = settings.time_base_num;
  video_codec_context->gop_size = 12; /* emit one intra frame very gop_size frames at most */
  video_codec_context->pix_fmt = AV_PIX_FMT_YUV420P;
  //video_codec_context->pix_fmt = AV_PIX_FMT_UYVY422;  // when using the Logitech Webcam c920 on mac, this is the standard format, so setting this is value for the encode would mean we don't need to convert but this makes sws_scale crash

  if(format_context->oformat->flags & AVFMT_GLOBALHEADER) {
    video_codec_context->flags |= CODEC_FLAG_GLOBAL_HEADER;
  }

  return true;
}

AVFrame* AVEncoder::allocVideoFrame(enum AVPixelFormat pixelFormat, int width, int height) {
  uint8_t* buf;
  AVFrame* pic;
  int size;

  pic = avcodec_alloc_frame();
  if(!pic) {
    RX_ERROR(ERR_AV_ALLOC_AVFRAME);
    return NULL;
  }
  
  size = avpicture_get_size(pixelFormat, width, height);
  buf = (uint8_t*)av_malloc(size);
  if(!buf) {
    RX_ERROR(ERR_AV_ALLOC_BUF_AVFRAME);
    av_free(pic);
    return NULL;
  }

  avpicture_fill((AVPicture*)pic, buf, pixelFormat, width, height);
  return pic;
}

bool AVEncoder::openVideo() {

  if(avcodec_open2(video_codec_context, NULL, NULL) < 0) {
    RX_ERROR(ERR_AV_OPEN_VIDEO_CODEC);
    return false;
  }

  // allocate the encodec raw frame 
  video_frame_out = allocVideoFrame(video_codec_context->pix_fmt, 
                                video_codec_context->width, 
                                video_codec_context->height);
  if(!video_frame_out) {
    RX_ERROR(ERR_AV_ALLOC_VIDEO_FRAME);
    return false;
  }

  // when the output format is not YUV420P, we need a temporary 
  // frame to convert into the correct format 
  //if(settings.in_pixel_format != settings.out_pixel_format) {
  //  tmp_video_frame = allocVideoFrame(settings.in_pixel_format, settings.in_w, settings.in_h);
  //}
  return true;
}


bool AVEncoder::rescale(unsigned char* data) {
  assert(video_codec_context);

  int img_nbytes = avpicture_fill(&video_frame_in, (uint8_t*)data, 
                                  settings.in_pixel_format, settings.in_w, 
                                  settings.in_h);

  int h = sws_scale(sws,
                    video_frame_in.data, video_frame_in.linesize, 0, settings.in_h,
                    video_frame_out->data, video_frame_out->linesize);

  if(h != settings.in_h) {
    RX_ERROR(ERR_AV_SWS_SCALE);
    return false;
  }

  return true;
}

bool AVEncoder::addVideoFrame(unsigned char* data, size_t nbytes) {
  int64_t pts = (rx_millis() - time_started) / millis_per_frame;
  /// millis_per_frame;
  RX_VERBOSE("millis_per_Frame: %ld, time_started: %ld, pts: %ld", millis_per_frame, time_started, pts);
  return addVideoFrame(data, pts, nbytes);
  
}

bool AVEncoder::addVideoFrame(unsigned char* data, int64_t pts, size_t nbytes) {
  assert(video_codec_context);
  assert(video_stream);

  uint64_t now = rx_millis();
  if(now < new_frame_timeout) {
    RX_VERBOSE("...");
    return false;
  }
  new_frame_timeout = now + millis_per_frame;
  
  if(!rescale(data)) {
    RX_ERROR(ERR_AV_ADD_VFRAME_SCALE);
    return false;
  }

  AVPacket pkt = { 0 } ;
  int got_packet;
  av_init_packet(&pkt);

  video_frame_out->pts = pts;
  int r = avcodec_encode_video2(video_codec_context, &pkt, video_frame_out, &got_packet);
  if(!r && got_packet && pkt.size) {
    if(pkt.pts != AV_NOPTS_VALUE) {
      pkt.pts = av_rescale_q(pkt.pts, 
                             video_codec_context->time_base, 
                             video_stream->time_base);
    }
    if(pkt.dts != AV_NOPTS_VALUE) {
      pkt.dts = av_rescale_q(pkt.dts, 
                             video_codec_context->time_base, 
                             video_stream->time_base);
    }
    pkt.stream_index = video_stream->index;
    r = av_interleaved_write_frame(format_context, &pkt);
  }
  else {
    r = 0;
  }

  if(r != 0) {
    RX_ERROR(ERR_AV_WRITE_VIDEO);
    return false;
  }

  return true;
}


bool AVEncoder::openAudio() {
  return true;
}

bool AVEncoder::stop() {
  // @todo add a check if we actually started encoding
  av_write_trailer(format_context);

  if(video_stream) {
    closeVideo();
  }

  for(int i = 0; i < format_context->nb_streams; ++i) {
    av_freep(&format_context->streams[i]->codec);
    av_freep(&format_context->streams[i]);
  }

  if(!(output_format->flags & AVFMT_NOFILE)) {
    avio_close(format_context->pb);
    RX_VERBOSE("CLOSED!");
  }
  
  av_free(format_context);
  // @todo close sws
  time_started = 0;

  return true;
}

void AVEncoder::closeVideo() {
  if(video_codec) {
    RX_VERBOSE("close video!");
    avcodec_close(video_stream->codec);
    av_free(video_frame_out->data[0]);
    av_free(video_frame_out);
  }
}


void AVEncoder::listSupportedVideoCodecPixelFormats() {
  if(!video_codec) {
    RX_ERROR(ERR_AV_LIST_PIX_FMT_CODEC_NOT_OPEN);
    return;
  }
  
  RX_VERBOSE("-----------------------------");
  const enum AVPixelFormat* fmt = video_codec->pix_fmts;
  while(*fmt != -1) {
    const AVPixFmtDescriptor* desc = av_pix_fmt_desc_get(*fmt);
    RX_VERBOSE("FMT: %s", desc->name);
    fmt++;
  }
  RX_VERBOSE("-----------------------------");
}
