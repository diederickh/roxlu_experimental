/*

  Note: I'm freeing the video_frame_out and video_frame_in
  int the destructor; or at least I'm following what I read and saw
  in the libav documentation. but it seems that these still leak.

  I would like to call avcodec_free_frame(video_frame_{in,out}) in 
  stop(), but that introduces a memory leak.

 */
#include <roxlu/core/Utils.h>
#include <roxlu/core/Log.h>
#include <av/AVEncoder.h>
#include <assert.h>

AVEncoder::AVEncoder() 
  :output_format(NULL)
  ,format_context(NULL)
  ,time_started(0)
  ,millis_per_video_frame(0)
  ,audio_codec_context(NULL)
  ,audio_codec(NULL)
  ,audio_stream(NULL)
  ,audio_frame(NULL)
  ,audio_input_frame_size(0) // experimental
  ,added_audio_frames(0)
  ,video_codec(NULL)
  ,video_codec_context(NULL)
  ,video_stream(NULL)
  ,video_frame_out(NULL)
  ,video_frame_in(NULL)
  ,sws(NULL)
  ,filter_graph(NULL)
  ,src_filter(NULL)
  ,sink_filter(NULL)
  ,fps_filter(NULL)
{
  rx_init_libav();
}

AVEncoder::~AVEncoder() {
  if(isStarted()) {
    stop();
  }

  // @TODO - free the frames in stop(), but it seems that these functions leak! - maybe use avframe_unref()
  if(video_frame_out) {
    avcodec_free_frame(&video_frame_out);
    video_frame_out = NULL;
  }

  if(video_frame_in) {
    avcodec_free_frame(&video_frame_in);
    video_frame_in = NULL;
  }

  millis_per_video_frame = 0;
}

bool AVEncoder::setup(AVEncoderSettings cfg) {

  if(!cfg.validate()) {
    return false;
  }

  millis_per_video_frame = (cfg.time_base_num / cfg.time_base_den) * 1000;
  if(!millis_per_video_frame) {
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

  if(needsSWS()) {
    if(!initializeSWS()) {
      return false;
    }
  }

  if(!addAudioStream(settings.audio_codec)) { 
    return false;
  }

  if(!openAudio()) {
    return false;
  }

  if(!openFilterGraph()) {
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

  print();

  return true;
}

bool AVEncoder::initializeSWS() {
  assert(video_codec_context);

  sws = sws_getContext(settings.in_w, settings.in_h, settings.in_pixel_format,
                       settings.out_w, settings.out_h, video_codec_context->pix_fmt,
                       SWS_FAST_BILINEAR, NULL, NULL, NULL);
  if(!sws) {
    RX_ERROR(ERR_AV_SWS);
    return false;
  }


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

  const AVCodecDescriptor* desc = avcodec_descriptor_get(codecID);
  if(desc) {
    RX_VERBOSE(V_AV_VIDEO_CODEC, desc->name); 
  }

  video_stream = avformat_new_stream(format_context, video_codec);
  if(!video_stream) {
    RX_ERROR(ERR_AV_VIDEO_STREAM);
    return false;
  }

  video_codec_context = video_stream->codec;

  listSupportedVideoCodecPixelFormats();

  video_codec_context->width = settings.out_w;
  video_codec_context->height = settings.out_h;
  video_codec_context->time_base.den = settings.time_base_den;
  video_codec_context->time_base.num = settings.time_base_num;
  video_codec_context->pix_fmt = AV_PIX_FMT_YUV420P; // @todo - test if the video codec supports this format

  //video_codec_context->gop_size = 12; /* emit one intra frame very gop_size frames at most */
  //video_codec_context->keyint_min = 25;
  //video_codec_context->gop_size = 40;
  //video_codec_context->bit_rate = 500000;
  //video_codec_context->qmin = 10;
  //video_codec_context->qmax = 18;
  //video_codec_context->rc_buffer_size = 0;
  //video_codec_context->rc_max_rate = 128000;
  //video_codec_context->rc_buffer_size = 0;
  
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
  // we assume we're using libx264 for now.
  AVDictionary* opts = NULL;
  av_dict_set(&opts, "preset", "ultrafast", 0);
  av_dict_set(&opts, "tune", "zerolatency", 0);
  av_dict_set(&opts, "crf", "18", 0); 

  if(avcodec_open2(video_codec_context, NULL, &opts) < 0) {
    RX_ERROR(ERR_AV_OPEN_VIDEO_CODEC);
    av_dict_free(&opts);
    return false;
  }
  av_dict_free(&opts);

  // allocate the encodec raw frame 
  if(!video_frame_out) {
    video_frame_out = allocVideoFrame(video_codec_context->pix_fmt, 
                                      video_codec_context->width, 
                                      video_codec_context->height);
    if(!video_frame_out) {
      RX_ERROR(ERR_AV_ALLOC_VIDEO_FRAME);
      return false;
    }
  }

  // allocate a container for the input data
  if(!video_frame_in) {
    video_frame_in = allocVideoFrame(settings.in_pixel_format, settings.in_w, settings.in_h);
    if(!video_frame_in) {
      RX_ERROR("Cannot allocate video frame in");
      return false;
    }
  }

  return true;
}


bool AVEncoder::rescale(unsigned char* data) {
  assert(video_codec_context);
  assert(video_frame_in);
  assert(video_frame_out);

  // get the input data into the video_frame_in member
  int img_nbytes = avpicture_fill((AVPicture*)video_frame_in, (uint8_t*)data, 
                                  settings.in_pixel_format, settings.in_w, 
                                  settings.in_h);

   int h = sws_scale(sws,
                    video_frame_in->data, video_frame_in->linesize, 0, settings.in_h,
                    video_frame_out->data, video_frame_out->linesize);

  if(h != settings.in_h) {
    RX_ERROR(ERR_AV_SWS_SCALE);
    return false;
  }

  return true;
}

bool AVEncoder::addVideoFrame(unsigned char* data, size_t nbytes) {
  int64_t pts = (rx_millis() - time_started) / millis_per_video_frame;
  return addVideoFrame(data, pts, nbytes);
}

bool AVEncoder::update() {
  if(!isStarted()) {
    return true;
  }

  AVFrame* f = avcodec_alloc_frame();
  int r = 0;

  int got_packet = 0;

  while(true) {
    r = av_buffersink_get_frame(sink_filter, f);
    if(r < 0) {
      break;
    }

    AVPacket pkt = { 0 } ;
    av_init_packet(&pkt);

    r = avcodec_encode_video2(video_codec_context, &pkt, f, &got_packet);
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

      av_frame_unref(f);

      if(r < 0) {
        break;
      }
    }

  }
  av_frame_free(&f);

  if(r != 0) {
    char err[512];
    av_strerror(r, err, sizeof(err));
    RX_ERROR(ERR_AV_WRITE_VIDEO, err);
    return false;
  }

  return true;
}

bool AVEncoder::addVideoFrame(unsigned char* data, int64_t pts, size_t nbytes) {
  assert(video_codec_context);
  assert(video_stream);
  assert(filter_graph);
  assert(src_filter);

  int r = 0;

  // scale or convert the pixel format if necessary
  if(sws) {
    if(!rescale(data)) {
      RX_ERROR(ERR_AV_ADD_VFRAME_SCALE);
      return false;
    }
  }
  else {
    r = avpicture_fill((AVPicture*)video_frame_out, (uint8_t*)data, 
                       settings.in_pixel_format, settings.in_w, 
                       settings.in_h);
    if(r == 0) {
      RX_ERROR(ERR_AV_FILL_VIDEO_FRAME);
      return false;
    }
  }

  // add the video into the filter graph
  video_frame_out->format = video_codec_context->pix_fmt;
  const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get((enum AVPixelFormat)video_frame_out->format);
  if(!desc) {
    RX_ERROR(ERR_AV_FILL_VIDEO_FRAME, video_frame_out->format);
    return false;
  }

  video_frame_out->width = settings.out_w;
  video_frame_out->height = settings.out_h;
  video_frame_out->pts = pts;

  r = av_buffersrc_write_frame(src_filter, video_frame_out);
  if(r < 0) {
    char err[512];
    av_strerror(r, err, sizeof(err));
    RX_ERROR(ERR_AV_ADD_TO_SRC_FILTER, err);
    return false;
  }

  return true;
}

bool AVEncoder::stop() {
  int r = 0;
  char err_msg[512];

  if(!output_format) {
    RX_ERROR(ERR_AV_NOT_STARTED);
    return false;
  }

  r = av_write_trailer(format_context);
  if(r != 0) {
    // @todo free/close all other av contexts when we fail here.
    // @todo use this way for av_strerror everywhere
    av_strerror(r, err_msg, sizeof(err_msg));
    RX_ERROR(ERR_AV_TRAILER, err_msg);
    return false; // @todo hmm do we want this?
  }

  if(video_stream) {
    closeVideo();
  }

  if(audio_stream) {
    closeAudio();
  }

  for(int i = 0; i < format_context->nb_streams; ++i) {
    av_freep(&format_context->streams[i]->codec);
    av_freep(&format_context->streams[i]);
  }

  if(!(output_format->flags & AVFMT_NOFILE)) {
    avio_close(format_context->pb);
  }
  
  av_free(format_context);
  format_context = NULL;

  if(sws) {
    sws_freeContext(sws);
    sws = NULL;
  }

  closeFilterGraph();
  
  added_audio_frames = 0;
  time_started = 0;
  output_format = NULL;

  return true;
}

void AVEncoder::closeVideo() {
  if(video_codec) {
    avcodec_close(video_stream->codec); // @todo - shouldn't I call av_free here as well?
  }

  // these are all just references to the video AVStream, see addVideoStream()
  video_stream = NULL;              
  video_codec_context = NULL;       
  video_codec = NULL;               
}



void AVEncoder::listSupportedAudioCodecSampleFormats() {
  if(!audio_codec) {
    RX_ERROR(ERR_AV_LIST_SMP_FMT_CODEC_NOT_OPEN);
    return;
  }

  RX_VERBOSE("");
  RX_VERBOSE("Supported sample formats that the audio encoder can handle:");
  RX_VERBOSE("-----------------------------");
  const enum AVSampleFormat* fmt = audio_codec->sample_fmts;
  if(fmt) {
    while(*fmt != -1) {
      const enum AVSampleFormat f = *fmt;
      RX_VERBOSE("FMT: %s", av_get_sample_fmt_name(f));
      fmt++;
    }
  }
  RX_VERBOSE("-----------------------------");
  RX_VERBOSE("");
}

bool AVEncoder::needsSWS() {
  assert(video_codec);

  if(!isInputPixelFormatSupportedByCodec()) {
    return true;
  }

  if(settings.in_w != settings.out_w || settings.in_h != settings.out_h) {
    return true;
  }

  return false;
}

bool AVEncoder::isInputPixelFormatSupportedByCodec() {
  assert(video_codec);

  bool supported = false;
  const enum AVPixelFormat* fmt = video_codec->pix_fmts;
  if(fmt) {
    while(*fmt != -1) {
      if(*fmt == settings.in_pixel_format) {
        supported = true;
        break;
      }
      fmt++;
    }
  }
  return supported;
  
}

bool AVEncoder::addAudioStream(enum AVCodecID codecID) {
  assert(output_format);
  assert(format_context);

  audio_codec = avcodec_find_encoder(codecID);
  if(!audio_codec) {
    RX_ERROR(ERR_AV_AUDIO_ENC_NOT_FOUND);
    return false;
  }

  const AVCodecDescriptor* desc = avcodec_descriptor_get(codecID);
  if(desc) {
    RX_VERBOSE(V_AV_AUDIO_CODEC, desc->name);
  }

  audio_stream = avformat_new_stream(format_context, audio_codec);
  if(!audio_stream) {
    RX_ERROR(ERR_AV_AUDIO_STREAM);
    return false;
  }

  audio_codec_context = audio_stream->codec;

  listSupportedAudioCodecSampleFormats();
  
  audio_codec_context->sample_fmt = settings.sample_fmt;
  audio_codec_context->bit_rate = settings.audio_bit_rate;
  audio_codec_context->sample_rate = settings.sample_rate;
  audio_codec_context->channels = settings.num_channels;

  if(format_context->oformat->flags & AVFMT_GLOBALHEADER) {
    audio_codec_context->flags |= CODEC_FLAG_GLOBAL_HEADER;
  }
  
  return true;
}

bool AVEncoder::openAudio() {

  if(!audio_codec_context) {
    RX_ERROR(ERR_AV_OPEN_AUDIO);
    return false;
  }

  if(avcodec_open2(audio_codec_context, NULL, NULL) < 0) {
    RX_ERROR(ERR_AV_OPEN_AUDIO_CODEC);
    return false;
  }

  if(audio_codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE) {
    audio_input_frame_size = 1000;
  }
  else {
    audio_input_frame_size = audio_codec_context->frame_size;
  }

  return true;
}

bool AVEncoder::addAudioFrame(uint8_t* data, int nsamples) {
  bool result = addAudioFrame(data, nsamples, added_audio_frames);
  added_audio_frames += nsamples;
  return result;
}

bool AVEncoder::addAudioFrame(uint8_t* data, int nsamples, int64_t pts) {
  assert(audio_stream);

  char err_msg[512];
  AVCodecContext* c = audio_codec_context;
  AVFrame* frame = avcodec_alloc_frame();
  AVPacket pkt = { 0 };
  int got_packet = 0;
  int r = 0;
  int buffer_size = audio_input_frame_size * av_get_bytes_per_sample(c->sample_fmt) * c->channels;

  frame->pts = pts;
  frame->nb_samples = nsamples;

  av_init_packet(&pkt);

  r = avcodec_fill_audio_frame(frame, c->channels, c->sample_fmt, 
                               data, buffer_size, 1);
  if(r != 0) {
    RX_ERROR(ERR_AV_FILL_AUDIO_FRAME, av_strerror(r, err_msg, sizeof(err_msg)));
    avcodec_free_frame(&frame);
    return false;
  }

  // encode the audio data
  r = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
  if(r != 0) {
    avcodec_free_frame(&frame);
    RX_ERROR(ERR_AV_ENCODE_AUDIO);
    return false;
  }
  
  if(got_packet) {
    if(pkt.pts != AV_NOPTS_VALUE) {
      pkt.pts = av_rescale_q(pkt.pts, audio_codec_context->time_base, audio_stream->time_base);
    }
    if(pkt.dts != AV_NOPTS_VALUE) {
      pkt.dts = av_rescale_q(pkt.dts, audio_codec_context->time_base, audio_stream->time_base);
    }
    if(pkt.duration != AV_NOPTS_VALUE) {
      pkt.duration = av_rescale_q(pkt.duration, audio_codec_context->time_base, audio_stream->time_base);
    }
  }

  if(!got_packet) {
    avcodec_free_frame(&frame); 
    return false;
  }

  // write the audio frame
  pkt.stream_index = audio_stream->index;
  r = av_interleaved_write_frame(format_context, &pkt);
  if(r != 0) {
    RX_ERROR(ERR_AV_WRITE_VIDEO);
    avcodec_free_frame(&frame);
    return false;
  }

  avcodec_free_frame(&frame);
  return true;
}

void AVEncoder::closeAudio() {
  if(audio_codec) {
    avcodec_close(audio_stream->codec);
  }

  audio_input_frame_size = 0; 

  // just references to audio_stream->* members; 
  audio_codec = NULL;
  audio_stream = NULL;
  audio_codec_context = NULL;
}


void AVEncoder::print() {
  if(!output_format) {
    return;
  }
  RX_VERBOSE("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  if(video_stream) {
    RX_VERBOSE("video_stream.time_base.num: %d", video_stream->time_base.num);
    RX_VERBOSE("video_stream.time_base.den: %d", video_stream->time_base.den);
  }

  if(audio_stream) {
    RX_VERBOSE("audio_stream.time_base.num: %d", audio_stream->time_base.num);
    RX_VERBOSE("audio_stream.time_base.den: %d", audio_stream->time_base.den);
  }
  RX_VERBOSE("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
}


// =====================================================================
bool AVEncoder::openFilterGraph() {
  assert(!filter_graph);

  char err[512];
  int r = 0;

  // create the graph
  filter_graph = avfilter_graph_alloc();
  if(!filter_graph) {
    RX_ERROR("Cannot allocate filter graph");
    closeFilterGraph();
    return false;
  }

  // add the source filter
  char args[1024];
  sprintf(args, "width=%d:height=%d:pix_fmt=%s:time_base=%d/%d:sar=1",
           settings.out_w, settings.out_h,
           "yuv420p", int(settings.time_base_num), int(settings.time_base_den));

  r = avfilter_graph_create_filter(&src_filter, avfilter_get_by_name("buffer"), "src", args, NULL, filter_graph);
  if(r < 0) {
    av_strerror(r, err, sizeof(err));
    RX_ERROR("Cannot create the buffer filter: %s / %s", err, args);
    closeFilterGraph();
    return false;
  }

  // add the sink filter
  r = avfilter_graph_create_filter(&sink_filter, avfilter_get_by_name("buffersink"), "sink", NULL, NULL, filter_graph);
  if(r < 0) {
    RX_ERROR("Cannot create the buffersink filter");
    closeFilterGraph();
    return false;
  }

  // add the fps filter
  sprintf(args, "fps=%d", int(settings.time_base_den));
  r = avfilter_graph_create_filter(&fps_filter, avfilter_get_by_name("fps"), "fps", args, NULL, filter_graph);
  if(r < 0) {
    RX_ERROR("Cannot create the fps filter");
    closeFilterGraph();
    return false;
  }

  r = avfilter_link(src_filter, 0, fps_filter, 0);
  if(r < 0) {
    RX_ERROR("Cannot link the src_filter to the fps_filter");
    closeFilterGraph();
    return false;
  }

  r = avfilter_link(fps_filter, 0, sink_filter, 0);
  if(r < 0) {
    RX_ERROR("Cannot link the fps_filter to the sink_filter");
    closeFilterGraph();
    return false;
  }

  // check validity
  r = avfilter_graph_config(filter_graph, NULL);
  if(r < 0) {
    RX_ERROR("avfilter_graph_config failed");
    closeFilterGraph();
    return false;
  }

  return true;
}

bool AVEncoder::closeFilterGraph() {

  if(filter_graph) {
    avfilter_graph_free(&filter_graph);
    filter_graph = NULL;
  }

  fps_filter = NULL;
  sink_filter = NULL;
  fps_filter = NULL;

  return true;
}


int AVEncoder::getAudioInputFrameSizePerChannel(enum AVCodecID codecID, 
                                                enum AVSampleFormat sampleFormat, 
                                                int bitrate, 
                                                int samplerate) 
{
  // check if we have all necessary settings to get
  AVCodec* c = avcodec_find_encoder(codecID);
  if(!c) {
    RX_ERROR(ERR_AV_AUDIO_ENC_NOT_FOUND);
    return 0;
  }

  if(c->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE) {
    return 1000;
  }
  else {
    AVCodecContext* ctx = avcodec_alloc_context3(c);
    if(!ctx) {
      RX_ERROR(ERR_AV_ALLOC_CODEC_CONTEXT);
      return 0;
    }

    ctx->sample_fmt = sampleFormat;
    ctx->bit_rate = bitrate;
    ctx->sample_rate = samplerate;

    int r = avcodec_open2(ctx, c, NULL);
    if(r < 0) {
      RX_ERROR(ERR_AV_OPEN_AUDIO_CODEC);
      return 0;
    }
    
    int fs = ctx->frame_size;
    avcodec_close(ctx);
    av_free(ctx);

    return fs;

  }
  return 0;

}


void AVEncoder::listSupportedVideoCodecPixelFormats() {
  if(!video_codec) {
    RX_ERROR(ERR_AV_LIST_PIX_FMT_CODEC_NOT_OPEN);
    return;
  }

  RX_VERBOSE("");
  RX_VERBOSE("Supported pixel formats that the video encoder can handle:");
  RX_VERBOSE("-----------------------------");
  const enum AVPixelFormat* fmt = video_codec->pix_fmts;
  if(fmt) {
    while(*fmt != -1) {
      const AVPixFmtDescriptor* desc = av_pix_fmt_desc_get(*fmt);
      RX_VERBOSE("FMT: %s", desc->name);
      fmt++;
    }
  }
  RX_VERBOSE("-----------------------------");
  RX_VERBOSE("");
}


