#include <flv/AV.h>

AV::AV() 
  :vid_in_w(640)
  ,vid_in_h(480)
  ,vid_out_w(384)
  ,vid_out_h(684)
  ,vid_num_channels(3)
  ,vid_fps(0.0)
  ,vid_millis_per_frame(0.0)
  ,vid_fmt(AV_FMT_RGB24)
  ,vid_timeout(0)
  ,vid_is_buffer_ready(false)
  ,vid_encoder(NULL)
  ,vid_sws(NULL)
  ,vid_tmp_buffer(NULL)
  ,vid_total_frames(0)
  ,vid_time_started(0)
  ,vid_last_timestamp(0)
  ,audio_samplerate(0)
  ,audio_total_samples(0)
  ,audio_num_channels(0)
  ,audio_max_samples_per_frame(0)
  ,audio_bytes_per_frame(0)
  ,audio_is_buffer_ready(false)
  ,audio_time_started(0)
  ,audio_lame_flags(NULL)
  ,flv(NULL)
  ,audio_ring_buffer(1024 * 1024 * 5) 
  ,audio_work_buffer(1024 * 1024 * 5)
  ,video_ring_buffer(0)
  ,video_work_buffer(0)
  ,is_initialized(false)
  ,must_stop(false)
{
  uv_mutex_init(&mutex);
  uv_mutex_init(&stop_mutex);
}

AV::~AV() {
  stop();
  shutdown();
}

bool AV::setupVideo(int inw, 
                    int inh, 
                    int outw, 
                    int outh, 
                    double fps,
                    AVVideoFormat fmt,
                    FLV* f
)
{
  vid_fps = fps;
  vid_millis_per_frame = (1.0/vid_fps) * 1000.0;
  vid_in_w = inw;
  vid_in_h = inh;
  vid_out_w = outw;
  vid_out_h = outh;
  vid_fmt = fmt;
  vid_num_channels = (fmt == AV_FMT_RGB24) ? 3 : 4; 
  vid_bytes_per_frame = vid_in_w * vid_in_h * vid_num_channels;
  flv = f;

  vid_tmp_buffer = new char[vid_bytes_per_frame];
  memset(vid_tmp_buffer, 0x00, vid_bytes_per_frame);

  video_ring_buffer.resize(vid_bytes_per_frame * 5);
  video_work_buffer.resize(vid_bytes_per_frame * 5);
  return true;
}

bool AV::setupAudio(int numChannels, 
                    int sampleRate, 
                    int maxSamplesPerFrame, 
                    AVAudioFormat audioFmt
)
{
  if(numChannels != 1 && numChannels != 2) {
    RX_ERROR(("only 1 or 2 audio channels are supported."));
    return false;
  }

  audio_samplerate = sampleRate;
  audio_num_channels = numChannels;
  audio_fmt = audioFmt;
  audio_max_samples_per_frame = maxSamplesPerFrame;

  if(audio_fmt == AV_FMT_INT16) {
    audio_bytes_per_sample = 2;
  }
  else if(audio_fmt == AV_FMT_FLOAT32) {
    audio_bytes_per_sample = 4;
  }
  else {
    RX_ERROR(("unsupported audio format.. we haven't defined a size per sample yet."));
    return false;
  }
  audio_bytes_per_frame = audio_num_channels * audio_bytes_per_sample * audio_max_samples_per_frame;
  return true;
}

bool AV::initialize() {
  if(!initializeVideo()) {
    RX_ERROR(("cannot initialize video."));
    return false;
  }

  if(!initializeAudio()) {
    RX_ERROR(("cannot initialize audio."));
    return false;
  }
  is_initialized = true;
  must_stop = false;
  return true;
}

bool AV::initializeVideo() {
  unsigned int csp = (vid_vflip) ? X264_CSP_I420 | X264_CSP_VFLIP : X264_CSP_I420;
  int r = x264_picture_alloc(&vid_pic_in, csp, vid_out_w, vid_out_h);
  if(r != 0) {
    RX_ERROR(("cannot allocate picture that holds the encoded data."));
    return false;
  }

  AVPixelFormat av_fmt = videoFormatToAVPixelFormat(vid_fmt);
  vid_sws = sws_getContext(vid_in_w, vid_in_h, av_fmt,
                           vid_out_w, vid_out_h, PIX_FMT_YUV420P, 
                           SWS_FAST_BILINEAR, NULL, NULL, NULL);

  printAVPixelFormat(av_fmt);
  
  if(!vid_sws) {
    RX_ERROR(("cannot setup sws."));
    return false;
  }
  return true;
}

bool AV::initializeAudio() {
  return true;  
}

bool AV::openLame() {
  if(audio_num_channels) {
    audio_lame_flags = lame_init();
    if(audio_lame_flags == NULL) {
      RX_ERROR(("ERROR: cannot initialize lame."));
      return false;
    }
    lame_set_num_channels(audio_lame_flags, audio_num_channels);
    lame_set_in_samplerate(audio_lame_flags, audio_samplerate);
    lame_set_brate(audio_lame_flags, 128);
    //lame_set_mode(audio_lame_flags, MONO); // lets lame pick the mode
    lame_set_quality(audio_lame_flags, 5); // 2 = high, 5 = medium, 7 = low

    int r = lame_init_params(audio_lame_flags);
    if(r < 0) {
      RX_ERROR(("ERROR: error with lame_init_params()"));
      return false;
    }
  }
	return true;
}

bool AV::openFLV() {
  FLVHeader flv_header;
  flv_header.has_audio = (audio_samplerate != 0);
  flv_header.has_video = (vid_in_w != 0 && vid_in_h != 0);

  if(flv_header.has_audio) {
    flv_header.sound_bitrate = 128000.0;
    flv_header.sound_type = (audio_num_channels > 1) ? FLV_SOUNDTYPE_STEREO : FLV_SOUNDTYPE_MONO;
    flv_header.sound_bits = FLV_SOUNDSIZE_16BIT; // we're compressing with mp3 so it's internally always 16bit, as per spec
    flv_header.sound_samplerate = audioSampleRateToFLVSampleRate(audio_samplerate);
  }

  if(flv == NULL) {
    RX_ERROR(("ERROR: AV::openFLV() - FLV is not set."));
    return false;
  }

  flv->open(flv_header);

  FLVParams flv_params;
  flv_params.x264_params = &vid_params;
  flv->writeParams(flv_params);

  int num_nals = 0;
  x264_nal_t* nals = NULL;
  x264_encoder_headers(vid_encoder, &nals, &num_nals);
  if(!nals) {
    RX_ERROR(("x264_encoder_headers() returned NULL!"));
    return false;
  }
  printX264Headers(nals);

  FLVAvcSequenceHeader avc_header;
  avc_header.nals = nals;
  flv->writeAvcSequenceHeader(avc_header);
  return true;
}

bool AV::openX264() {
  int r = 0;
  x264_param_t* p = &vid_params;

  r = x264_param_default_preset(p, "ultrafast", "zerolatency"); // ultrafast, zerolatency
  if(r != 0) {
    RX_ERROR(("cannot set x264 preset."));
    return false;
  }

  r = x264_param_apply_profile(p, "baseline");
  if(r != 0) {
    RX_ERROR(("cannot apply profile."));
    return false;
  }
#if defined(NDEBUG)
  p->i_log_level = X264_LOG_DEBUG;
#endif
  p->i_threads = 1;
  p->i_width = vid_out_w;
  p->i_height = vid_out_h;
  p->i_fps_num = vid_fps;
  p->i_fps_den = 1;
  p->b_annexb = 0; // for flv files this must be 0
  p->b_repeat_headers = 0;

  vid_encoder = x264_encoder_open(&vid_params);
  if(!vid_encoder) {
    RX_ERROR(("cannot open x264 encoder."));
    return false;
  }

  x264_encoder_parameters(vid_encoder, &vid_params);

  printX264Params(&vid_params);
  return true;
}

#define AV_CHECK_MUST_STOP \
  {                                             \
    uv_mutex_lock(&stop_mutex);                 \
    if(must_stop) {                             \
       uv_mutex_unlock(&stop_mutex);            \
      break;                                    \
    }                                           \
    uv_mutex_unlock(&stop_mutex);               \
  }                                            



void av_thread_function(void* user) {
  AV* av_ptr = static_cast<AV*>(user);
  av_ptr->run();
}

void AV::run() {

  // audio + video 
  if(vid_in_w != 0 && audio_num_channels != 0) {
    std::vector<AVPacket*> work_packets;
    while(true) {

      if(!is_initialized) {
        rx_sleep_millis(vid_millis_per_frame);
        AV_CHECK_MUST_STOP;
        continue;
      }

      if(!vid_is_buffer_ready || !audio_is_buffer_ready) {
        rx_sleep_millis(vid_millis_per_frame);
        AV_CHECK_MUST_STOP;
        continue;
      }

      size_t num_packets = packets.size();
      if(num_packets == 0) {
        AV_CHECK_MUST_STOP;
        continue;
      }

      uv_mutex_lock(&mutex);
      {
        std::copy(packets.begin(), packets.end(), std::back_inserter(work_packets));
        audio_work_buffer.write(audio_ring_buffer.getReadPtr(), audio_ring_buffer.size());
        video_work_buffer.write(video_ring_buffer.getReadPtr(), video_ring_buffer.size());
        audio_ring_buffer.reset();
        video_ring_buffer.reset();
        packets.clear();
      }
      uv_mutex_unlock(&mutex);

      for(std::vector<AVPacket*>::iterator it = work_packets.begin(); it != work_packets.end(); ++it) {
        AVPacket* p = *it;
        if(p->type == AV_VIDEO) {
          encodeVideoPacket(p);
        }
        else if(p->type == AV_AUDIO) {
          encodeAudioPacket(p);
        }
      }

      for(std::vector<AVPacket*>::iterator it = work_packets.begin(); it != work_packets.end(); ++it) {
        delete *it;
        *it = NULL;
      }
      work_packets.clear();

      audio_work_buffer.reset();
      video_work_buffer.reset();

      AV_CHECK_MUST_STOP;
    }
  }

  // video only loop
  else {
    std::vector<AVPacket*> work_packets;
    while(true) {

      if(!is_initialized) {
        rx_sleep_millis(vid_millis_per_frame);
        AV_CHECK_MUST_STOP;
        continue;
      }

      if(!vid_is_buffer_ready) { 
        AV_CHECK_MUST_STOP;
        continue;
      }

      uv_mutex_lock(&mutex);
      {
        std::copy(packets.begin(), packets.end(), std::back_inserter(work_packets));
        video_work_buffer.write(video_ring_buffer.getReadPtr(), video_ring_buffer.size());
        packets.clear();
        video_ring_buffer.reset();
      }
      uv_mutex_unlock(&mutex);

      for(std::vector<AVPacket*>::iterator it = work_packets.begin(); it != work_packets.end(); ++it) {
        AVPacket* p = *it;
        if(p->type == AV_VIDEO) {
          encodeVideoPacket(p);
        }
      }
  
      for(std::vector<AVPacket*>::iterator it = work_packets.begin(); it != work_packets.end(); ++it) {
        delete *it;
        *it = NULL;
      }
      work_packets.clear();

      video_work_buffer.reset();

      AV_CHECK_MUST_STOP;

    } // video only
  }
  reset();
}

void AV::encodeAudioPacket(AVPacket* p) {
  if(!is_initialized) {
    RX_WARNING(("cannot add audio frame when we're not initialized."));
    return;
  }

  size_t bytes_read = audio_work_buffer.read((char*)audio_tmp_in_buffer, p->num_bytes);
  if(bytes_read != p->num_bytes) {
    RX_ERROR(("cannot read audio from buffer. Skipping encoding."));
    return;
  }
  short int* src_ptr = (short int*)audio_tmp_in_buffer;
  short int* in_left_ptr = src_ptr;
  short int* in_right_ptr = src_ptr;

  if(flv->flv_header.sound_type == FLV_SOUNDTYPE_MONO) {

    if(audio_fmt == AV_FMT_FLOAT32) {
      float* src_float_ptr = (float*)audio_tmp_in_buffer;      
      for(int j = 0; j < p->num_frames; ++j) {
        *(in_left_ptr + j) = *(src_float_ptr++) * 32767;
      }
      in_right_ptr = in_left_ptr;
    }

  }
  else if(flv->flv_header.sound_type == FLV_SOUNDTYPE_STEREO) {

    if(audio_fmt == AV_FMT_INT16) {
      short int* src_ptr = (short int*)audio_tmp_in_buffer;
      in_left_ptr = (short int*)audio_tmp_in_buffer_left;
      in_right_ptr = (short int*)audio_tmp_in_buffer_right;

      for(int j = 0; j < p->num_frames; ++j) {  // uninterleave
        *(in_left_ptr + j) = *(src_ptr++);
        *(in_right_ptr + j) = *(src_ptr++);
      }

    }
    else if(audio_fmt == AV_FMT_FLOAT32) {
      float* src_float_ptr = (float*)audio_tmp_in_buffer;
      in_left_ptr = (short int*)audio_tmp_in_buffer_left;
      in_right_ptr = (short int*)audio_tmp_in_buffer_right;

      for(int j = 0; j < p->num_frames; ++j) {
        *(in_left_ptr + j) = *(src_float_ptr++) * 32767;
        *(in_right_ptr + j) = *(src_float_ptr++) * 32767;
      }
    }
  }

  int written = lame_encode_buffer(audio_lame_flags, 
                                   in_left_ptr, in_right_ptr,
                                   p->num_frames, audio_tmp_out_buffer, MP3_BUFFER_SIZE);

  if(written > 0) {
    FLVAudioPacket pkt;
    pkt.timestamp = p->timestamp;
    pkt.data = audio_tmp_out_buffer;
    pkt.size = written;
    flv->writeAudioPacket(pkt);
  }
}

void AV::encodeVideoPacket(AVPacket* p) {
  if(!is_initialized) {
    RX_WARNING(("cannot add audio frame when we're not initialized."));
    return;
  }

  int vid_in_stride = vid_in_w * vid_num_channels; // @todo make member
  video_work_buffer.read(vid_tmp_buffer, vid_bytes_per_frame); // @todo test/use: p->num_bytes
  
  int h = sws_scale(vid_sws,
                    (uint8_t**)&vid_tmp_buffer,
                    &vid_in_stride,
                    0, 
                    vid_in_h, 
                    vid_pic_in.img.plane, 
                    vid_pic_in.img.i_stride);

  if(h != vid_out_h) {
    RX_ERROR(("cannot sws_scale().\n"));
    return;
  }

  vid_pic_in.i_pts = vid_total_frames;
  vid_total_frames++;

  x264_nal_t* nal = NULL;
  int nals_count = 0;
  int frame_size = x264_encoder_encode(vid_encoder, &nal, &nals_count, &vid_pic_in, &vid_pic_out);

  if(frame_size < 0) {
    RX_WARNING(("x264_encoder_encode fails"));
    return;
  }
  if(nal == NULL) {
    RX_ERROR(("x264_encoder_encode() return 0 nals."));
    return;
  }

  if(p->timestamp < vid_last_timestamp) {
    RX_ERROR(("Given timestamp is smaller the previous one! Current timestamp: %lld, previous: %lld", p->timestamp, vid_last_timestamp));
  }
  vid_last_timestamp = p->timestamp;
      
  FLVVideoPacket flv_vid_packet;
  flv_vid_packet.timestamp = p->timestamp; //vid_total_frames * vid_millis_per_frame;
  flv_vid_packet.is_keyframe = vid_pic_out.b_keyframe;
  flv_vid_packet.nals_size = frame_size;
  flv_vid_packet.nals_data = nal[0].p_payload;
  flv->writeVideoPacket(flv_vid_packet);
}

void AV::shutdown() {
  if(!is_initialized) {
    RX_VERBOSE("we're not yet initialized so cannot shutdown.");
    return;
  }

  is_initialized = false;
  audio_samplerate = 0;
  audio_num_channels = 0;
  audio_bytes_per_frame = 0;
  audio_max_samples_per_frame = 0;
  audio_is_buffer_ready = 0;
  audio_total_samples = 0;
  audio_time_started = 0;

  delete[] vid_tmp_buffer;
  vid_tmp_buffer = NULL;
  vid_in_w = 0;
  vid_in_h = 0;
  vid_fps = 0;
  vid_millis_per_frame = 0.0;
  vid_total_frames = 0;
  vid_bytes_per_frame = 0;
  vid_timeout = 0;
  vid_time_started = 0;
  vid_is_buffer_ready = false;
  vid_last_timestamp = 0;

  delete[] vid_tmp_buffer;
  vid_tmp_buffer = NULL;

  memset((char*)&vid_params, 0, sizeof(vid_params));

  x264_picture_clean(&vid_pic_in);

  sws_freeContext(vid_sws);

  if(audio_lame_flags) { 
    lame_close(audio_lame_flags);
    audio_lame_flags = NULL;
  }
  
  uv_mutex_destroy(&mutex);
  uv_mutex_destroy(&stop_mutex);

  vid_sws = NULL;
  vid_encoder = NULL;
}

void AV::reset() {
  if(!is_initialized) {
    RX_VERBOSE("we're not yet initialized, so cannot reset().");
    return;
  }

  FLVCloseParams p;
  flv->close(p);

  if(audio_lame_flags) {
    //lame_encode_finish(audio_lame_flags, audio_tmp_in_buffer, MP3_BUFFER_SIZE);
    lame_close(audio_lame_flags);
    audio_lame_flags = NULL;
  }
  
  memset(audio_tmp_in_buffer, 0, (MP3_BUFFER_SIZE * 2));
  memset(audio_tmp_in_buffer_left, 0, MP3_BUFFER_SIZE);
  memset(audio_tmp_in_buffer_right, 0, MP3_BUFFER_SIZE);
  memset(audio_tmp_out_buffer, 0, MP3_BUFFER_SIZE);

  audio_is_buffer_ready = 0;
  audio_total_samples = 0;
  audio_time_started = 0;
  audio_is_buffer_ready = false;

  vid_total_frames = 0;
  vid_timeout = 0;
  vid_time_started = 0;
  vid_is_buffer_ready = false;
  vid_last_timestamp = 0;

  audio_ring_buffer.reset();
  video_ring_buffer.reset();
  audio_work_buffer.reset();
  video_work_buffer.reset();

  uv_mutex_lock(&stop_mutex);
  must_stop = false;
  uv_mutex_unlock(&stop_mutex);

  x264_encoder_close(vid_encoder);

  for(std::vector<AVPacket*>::iterator it = packets.begin(); it != packets.end(); ++it) {
    delete *it;
    *it = NULL;
  }
  packets.clear();
}

FLVAudioSampleRate AV::audioSampleRateToFLVSampleRate(int rate) {
  switch(rate) {
    case 5500: return FLV_SOUNDRATE_5_5KHZ; 
    case 11025: return FLV_SOUNDRATE_11KHZ;
    case 22050: return FLV_SOUNDRATE_22KHZ;
    case 44100: return FLV_SOUNDRATE_44KHZ;
    default: {
      RX_ERROR(("cannot convert the given audio samplerate to one that can be used by FLV. exiting."));
      ::exit(EXIT_FAILURE);
    };
  }
}

void AV::printX264Params(x264_param_t* p) {
  RX_VERBOSE("--------------------------------------------------");
  RX_VERBOSE("x264_param_t.i_threads: %d", p->i_threads);
  RX_VERBOSE("x264_param_t.i_lookahead_threads: %d", p->i_lookahead_threads);
  RX_VERBOSE("x264_param_t.b_sliced_threads: %d", p->b_sliced_threads);
  RX_VERBOSE("x264_param_t.b_deterministic: %d ", p->b_deterministic);
  RX_VERBOSE("x264_param_t.b_cpu_independent: %d ", p->b_cpu_independent);
  RX_VERBOSE("x264_param_t.i_sync_lookahead: %d", p->i_sync_lookahead);
  RX_VERBOSE("x264_param_t.i_width: %d", p->i_width);
  RX_VERBOSE("x264_param_t.i_height: %d", p->i_height);
  RX_VERBOSE("x264_param_t.i_csp: %d ", p->i_csp);
  RX_VERBOSE("x264_param_t.i_level_idc: %d", p->i_level_idc);
  RX_VERBOSE("x264_param_t.i_frame_total: %d ", p->i_frame_total);
  RX_VERBOSE("x264_param_t.i_nal_hrd: %d ", p->i_nal_hrd);
  // skipping vui
  RX_VERBOSE("--");
  RX_VERBOSE("x264_param_t.i_frame_reference: %d", p->i_frame_reference);
  RX_VERBOSE("x264_param_t.i_dpb_size: %d ", p->i_dpb_size);
  RX_VERBOSE("x264_param_t.i_keyint_max: %d ", p->i_keyint_max);
  RX_VERBOSE("x264_param_t.i_keyint_min: %d ", p->i_keyint_min);
  RX_VERBOSE("x264_param_t.i_scenecut_threshold: %d: ", p->i_scenecut_threshold);
  RX_VERBOSE("x264_param_t.b_intra_refresh: %d", p->b_intra_refresh);
  RX_VERBOSE("x264_param_t.i_bframe: %d", p->i_bframe);
  RX_VERBOSE("x264_param_t.i_bframe_adaptive: %d", p->i_bframe_adaptive);
  // skipping a lot..
  RX_VERBOSE("--");
  RX_VERBOSE("x264_param_t.i_rc_method: %d", p->rc.i_rc_method);
  RX_VERBOSE("x264_param_t.i_qp_constant: %d", p->rc.i_qp_constant);
  RX_VERBOSE("x264_param_t.i_qp_min: %d", p->rc.i_qp_min);
  RX_VERBOSE("x264_param_t.i_qp_max: %d ", p->rc.i_qp_max);
  RX_VERBOSE("x264_param_t.i_qp_step: %d ", p->rc.i_qp_step);
  RX_VERBOSE("x264_param_t.i_bitrate: %d ", p->rc.i_bitrate);
  // skipping .. 
  RX_VERBOSE("--");
  RX_VERBOSE("x264_param_t.b_aud: %d", p->b_aud);
  RX_VERBOSE("x264_param_t.b_repeat_headers: %d ", p->b_repeat_headers);
  RX_VERBOSE("x264_param_t.b_annexb: %d (flv does not support annexb)", p->b_annexb);
  RX_VERBOSE("x264_param_t.i_sps_id: %d ", p->i_sps_id);
  RX_VERBOSE("x264_param_t.b_vfr_input: %d", p->b_vfr_input);
  RX_VERBOSE("x264_param_t.b_pulldown: %d", p->b_pulldown);
  RX_VERBOSE("x264_param_t.i_fps_num: %d", p->i_fps_num);
  RX_VERBOSE("x264_param_t.i_fps_den: %d", p->i_fps_den);
  RX_VERBOSE("x264_param_t.i_timebase_num: %d ", p->i_timebase_num);
  RX_VERBOSE("x264_param_t.i_timebase_den: %d", p->i_timebase_den);
  RX_VERBOSE("--------------------------------------------------");
}

void AV::printX264Headers(x264_nal_t* nal) {
  RX_VERBOSE("--");
  RX_VERBOSE("sps_size: %d", nal[0].i_payload);
  RX_VERBOSE("pps_size: %d", nal[1].i_payload);
  RX_VERBOSE("sei_size: %d", nal[2].i_payload);
  RX_VERBOSE("--");
}

void AV::printAVPixelFormat(AVPixelFormat p) {
  std::string str;
  switch(p) {
    case PIX_FMT_RGB24: str = "PIX_FMT_RGB24"; break;
    case PIX_FMT_BGRA: str = "PIX_FMT_BGRA"; break;
    default: str = "PIX_FMT_NONE"; break;
  }
  RX_VERBOSE("AVPixelFormat: %s\n", str.c_str());
}
