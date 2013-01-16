#include <flv/AV.h>

AV::AV() 
  :vid_w(640)
  ,vid_h(480)
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
  ,audio_samplerate(0)
  ,audio_total_samples(0)
  ,audio_num_channels(0)
  ,audio_max_samples_per_frame(0)
  ,audio_bytes_per_frame(0)
  ,audio_is_buffer_ready(false)
  ,audio_time_started(0)
  ,flv(NULL)
  ,audio_ring_buffer(1024 * 1024 * 5) 
  ,video_ring_buffer(1024 * 1024 * 5) 
  ,audio_work_buffer(1024 * 1024 * 5)
  ,video_work_buffer(1024 * 1024 * 5)
  ,is_initialized(false)
  ,must_stop(false)
{
}

AV::~AV() {
  stop();
}

bool AV::setupVideo(int w, int h, double fps, AVVideoFormat fmt) {
  vid_fps = fps;
  vid_millis_per_frame = (1.0/vid_fps) * 1000.0;
  vid_w = w;
  vid_h = h;
  vid_fmt = fmt;
  vid_tmp_buffer = new char[vid_w * vid_h * 3];
  return true;
}

bool AV::setupAudio(int numChannels, int sampleRate, int maxSamplesPerFrame, AVAudioFormat audioFmt) {
  if(numChannels != 1 && numChannels != 2) {
    printf("ERROR: only 1 or 2 audio channels are supported.\n");
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
    printf("ERROR: unsupported audio format.. we haven't defined a size per sample yet.\n");
    return false;
  }

  audio_bytes_per_frame = audio_num_channels * audio_bytes_per_sample * audio_max_samples_per_frame;
  return true;
}

bool AV::initialize() {
  FLVHeader flv_header;
  flv_header.has_audio = (audio_samplerate != 0);
  flv_header.has_video = (vid_w != 0 && vid_h != 0);

  if(flv_header.has_audio) {
    flv_header.sound_bitrate = 128000.0;
    flv_header.sound_type = (audio_num_channels > 1) ? FLV_SOUNDTYPE_STEREO : FLV_SOUNDTYPE_MONO;
    flv_header.sound_bits = FLV_SOUNDSIZE_16BIT; // we're compressing with mp3 so it's internally always 16bit, as per spec
    flv_header.sound_samplerate = audioSampleRateToFLVSampleRate(audio_samplerate);
  }

  if(flv == NULL) {
    printf("ERROR: FLV is not set.\n");
    return false;
  }

  flv->open(flv_header);

  if(!initializeVideo()) {
    printf("ERROR: cannot initialize video.\n");
    return false;
  }

  if(!initializeAudio()) {
    printf("ERROR: cannot initialize audio.\n");
    return false;
  }
  is_initialized = true;
  must_stop = false;
  return true;
}

bool AV::initializeVideo() {
  if(!setupX264()) {
    return false;
  }

  vid_bytes_per_frame = vid_w * vid_h * 3;

  printX264Params(&vid_params);

  int r = x264_picture_alloc(&vid_pic_in, X264_CSP_I420, vid_w, vid_h);
  if(r != 0) {
    printf("ERROR: cannot allocate picture that holds the encoded data.\n");
    return false;
  }

  vid_sws = sws_getContext(vid_w, vid_h, PIX_FMT_RGB24, 
                           vid_w, vid_h, PIX_FMT_YUV420P, 
                           SWS_FAST_BILINEAR, NULL, NULL, NULL);

  if(!vid_sws) {
    printf("ERROR: cannot setup sws\n");
    return false;
  }

  if(flv == NULL) {
    printf("ERROR: FLV is not set.\n");
    return false;
  }
  
  FLVParams flv_params;
  flv_params.x264_params = &vid_params;
  flv->writeParams(flv_params);

  int num_nals = 0;
  x264_nal_t* nals = NULL;
  x264_encoder_headers(vid_encoder, &nals, &num_nals);
  if(!nals) {
    printf("ERROR: x264_encoder_headers() returned NULL!\n");
    return false;
  }
  printf("num_nals in header: %d\n", num_nals);
  printX264Headers(nals);

  FLVAvcSequenceHeader avc_header;
  avc_header.nals = nals;
  flv->writeAvcSequenceHeader(avc_header);

  return true;
}

bool AV::initializeAudio() {
  if(audio_num_channels) {
    audio_lame_flags = lame_init();
    if(audio_lame_flags == NULL) {
      printf("ERROR: cannot initialize lame.\n");
      return false;
    }
    lame_set_num_channels(audio_lame_flags, audio_num_channels);
    lame_set_in_samplerate(audio_lame_flags, audio_samplerate);
    lame_set_brate(audio_lame_flags, 128);
    //lame_set_mode(audio_lame_flags, MONO); // lets lame pick the mode
    lame_set_quality(audio_lame_flags, 5); // 2 = high, 5 = medium, 7 = low

    int r = lame_init_params(audio_lame_flags);
    if(r < 0) {
      printf("ERROR: error with lame_init_params()\n");
      return false;
    }
  }
  return true;  
}

bool AV::setupX264() {
  int r = 0;
  x264_param_t* p = &vid_params;

  r = x264_param_default_preset(p, "ultrafast", "zerolatency"); // ultrafast, zerolatency
  if(r != 0) {
    printf("ERROR: cannot set x264 preset.\n");
    return false;
  }

  r = x264_param_apply_profile(p, "baseline");
  if(r != 0) {
    printf("ERROR: cannot apply profile.\n");
    return false;
  }

  //p->i_log_level = X264_LOG_DEBUG;
  p->i_threads = 1;
  p->i_width = vid_w;
  p->i_height = vid_h;
  p->i_fps_num = vid_fps;
  p->i_fps_den = 1;
  p->b_annexb = 0; // for flv files this must be 0
  p->b_repeat_headers = 0;

  /*
    p->b_annexb = 0;
    p->b_repeat_headers = 0;
    p->i_keyint_max = 1;
    p->rc.i_rc_method = X264_RC_CRF;
    p->rc.i_qp_min = 1;
    p->rc.i_qp_max = 10;
    p->rc.f_rf_constant = 25;
    p->rc.f_rf_constant_max = 35;
  */

  vid_encoder = x264_encoder_open(&vid_params);
  if(!vid_encoder) {
    printf("ERROR: cannot open x264 encoder.\n");
    return false;
  }

  x264_encoder_parameters(vid_encoder, &vid_params);
  return true;
}

void AV::run() {
  // audio + video 
  if(vid_w != 0 && audio_num_channels != 0) {
    std::vector<AVPacket*> work_packets;
    while(true) {
      if(must_stop) {
        break;
      }

      if(!is_initialized) {
        rx_sleep_millis(vid_millis_per_frame);
        continue;
      }

      if(!vid_is_buffer_ready || !audio_is_buffer_ready) {
        rx_sleep_millis(vid_millis_per_frame);
        continue;
      }
      size_t num_packets = packets.size();
      if(num_packets == 0) {
        continue;
      }

      mutex.lock();
      {
        std::copy(packets.begin(), packets.end(), std::back_inserter(work_packets));
        audio_work_buffer.write(audio_ring_buffer.getReadPtr(), audio_ring_buffer.size());
        video_work_buffer.write(video_ring_buffer.getReadPtr(), video_ring_buffer.size());
        audio_ring_buffer.reset();
        video_ring_buffer.reset();
        packets.clear();
      }
      mutex.unlock();

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
      }

      audio_work_buffer.reset();
      video_work_buffer.reset();
      work_packets.clear();
    }
  }

  // video only loop
  else {
    std::vector<AVPacket*> work_packets;
    while(true) {
      if(must_stop) {
        break;
      }

      if(!is_initialized) {
        rx_sleep_millis(vid_millis_per_frame);
        continue;
      }

      if(!vid_is_buffer_ready) { 
        continue;
      }
      
      mutex.lock();
      {
        std::copy(packets.begin(), packets.end(), std::back_inserter(work_packets));
        video_work_buffer.write(video_ring_buffer.getReadPtr(), video_ring_buffer.size());
        packets.clear();
        video_ring_buffer.reset();
      }
      mutex.unlock();

      for(std::vector<AVPacket*>::iterator it = work_packets.begin(); it != work_packets.end(); ++it) {
        AVPacket* p = *it;
        if(p->type == AV_VIDEO) {
          encodeVideoPacket(p);
        }
      }

      for(std::vector<AVPacket*>::iterator it = work_packets.begin(); it != work_packets.end(); ++it) {
        delete *it;
      }

      video_work_buffer.reset();
      work_packets.clear();
    } // video only
  }

  reset();
  thread.exit();
}

void AV::encodeAudioPacket(AVPacket* p) {
  if(!is_initialized) {
    printf("WARNING: cannot add audio frame when we're not initialized.\n");
    return;
  }

  size_t bytes_read = audio_work_buffer.read((char*)audio_tmp_in_buffer, p->num_bytes);
  if(bytes_read != p->num_bytes) {
    printf("ERROR: cannot read audio from buffer. Skipping encoding.\n");
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
    printf("WARNING: cannot add audio frame when we're not initialized.\n");
    return;
  }

  int vid_in_stride = vid_w * 3; // @todo make member
  video_work_buffer.read(vid_tmp_buffer, vid_bytes_per_frame); // @todo test/use: p->num_bytes
  int h = sws_scale(vid_sws,
                    (uint8_t**)&vid_tmp_buffer,
                    &vid_in_stride,
                    0, 
                    vid_h, 
                    vid_pic_in.img.plane, 
                    vid_pic_in.img.i_stride);

  if(h != vid_h) {
    printf("ERROR: cannot sws_scale().\n");
    return;
  }

  vid_pic_in.i_pts = vid_total_frames;
  vid_total_frames++;

  x264_nal_t* nal = NULL;
  int nals_count = 0;
  int frame_size = x264_encoder_encode(vid_encoder, &nal, &nals_count, &vid_pic_in, &vid_pic_out);
  if(frame_size < 0) {
    printf("ERROR: x264_encoder_encode fails\n");
    return;
  }
  if(nal == NULL) {
    printf("WARNING: x264_encoder_encode() return 0 nals.\n");
    return;
  }
      
  FLVVideoPacket flv_vid_packet;
  flv_vid_packet.timestamp = p->timestamp; //vid_total_frames * vid_millis_per_frame;
  flv_vid_packet.is_keyframe = vid_pic_out.b_keyframe;
  flv_vid_packet.nals_size = frame_size;
  flv_vid_packet.nals_data = nal[0].p_payload;
  flv->writeVideoPacket(flv_vid_packet);
}

void AV::reset() {
  if(!is_initialized) {
    return;
  }

  is_initialized = false;

  FLVCloseParams p;
  flv->close(p);
  
  lame_close(audio_lame_flags);
  audio_lame_flags = NULL;
  memset(audio_tmp_in_buffer, 0, (MP3_BUFFER_SIZE * 2));
  memset(audio_tmp_in_buffer_left, 0, MP3_BUFFER_SIZE);
  memset(audio_tmp_in_buffer_right, 0, MP3_BUFFER_SIZE);
  memset(audio_tmp_out_buffer, 0, MP3_BUFFER_SIZE);

  audio_samplerate = 0;
  audio_num_channels = 0;
  audio_bytes_per_frame = 0;
  audio_max_samples_per_frame = 0;
  audio_is_buffer_ready = 0;
  audio_total_samples = 0;
  audio_time_started = 0;

  delete[] vid_tmp_buffer;
  vid_tmp_buffer = NULL;
  vid_w = 0;
  vid_h = 0;
  vid_fps = 0;
  vid_millis_per_frame = 0.0;
  vid_total_frames = 0;
  vid_bytes_per_frame = 0;
  vid_timeout = 0;
  vid_time_started = 0;

  memset((char*)&vid_params, 0, sizeof(vid_params));
  audio_ring_buffer.reset();
  video_ring_buffer.reset();
  audio_work_buffer.reset();
  video_work_buffer.reset();
  
  x264_encoder_close(vid_encoder);
  
  x264_picture_clean(&vid_pic_in);

  sws_freeContext(vid_sws);
  
  vid_sws = NULL;
  vid_encoder = NULL;
}


FLVAudioSampleRate AV::audioSampleRateToFLVSampleRate(int rate) {
  switch(rate) {
    case 5500: return FLV_SOUNDRATE_5_5KHZ; 
    case 11025: return FLV_SOUNDRATE_11KHZ;
    case 22050: return FLV_SOUNDRATE_22KHZ;
    case 44100: return FLV_SOUNDRATE_44KHZ;
    default: {
      printf("ERROR: cannot convert the given audio samplerate to one that can be used by FLV. exiting.\n");
      ::exit(EXIT_FAILURE);
    };
  }
}

void AV::printX264Params(x264_param_t* p) {
  printf("--------------------------------------------------\n");
  printf("x264_param_t.i_threads: %d\n", p->i_threads);
  printf("x264_param_t.i_lookahead_threads: %d\n", p->i_lookahead_threads);
  printf("x264_param_t.b_sliced_threads: %d\n", p->b_sliced_threads);
  printf("x264_param_t.b_deterministic: %d \n", p->b_deterministic);
  printf("x264_param_t.b_cpu_independent: %d \n", p->b_cpu_independent);
  printf("x264_param_t.i_sync_lookahead: %d\n", p->i_sync_lookahead);
  printf("x264_param_t.i_width: %d\n", p->i_width);
  printf("x264_param_t.i_height: %d\n", p->i_height);
  printf("x264_param_t.i_csp: %d \n", p->i_csp);
  printf("x264_param_t.i_level_idc: %d\n", p->i_level_idc);
  printf("x264_param_t.i_frame_total: %d \n", p->i_frame_total);
  printf("x264_param_t.i_nal_hrd: %d \n", p->i_nal_hrd);
  // skipping vui
  printf("--\n");
  printf("x264_param_t.i_frame_reference: %d\n", p->i_frame_reference);
  printf("x264_param_t.i_dpb_size: %d \n", p->i_dpb_size);
  printf("x264_param_t.i_keyint_max: %d \n", p->i_keyint_max);
  printf("x264_param_t.i_keyint_min: %d \n", p->i_keyint_min);
  printf("x264_param_t.i_scenecut_threshold: %d: \n", p->i_scenecut_threshold);
  printf("x264_param_t.b_intra_refresh: %d\n", p->b_intra_refresh);
  printf("x264_param_t.i_bframe: %d\n", p->i_bframe);
  printf("x264_param_t.i_bframe_adaptive: %d\n", p->i_bframe_adaptive);
  // skipping a lot..
  printf("--\n");
  printf("x264_param_t.i_rc_method: %d\n", p->rc.i_rc_method);
  printf("x264_param_t.i_qp_constant: %d\n", p->rc.i_qp_constant);
  printf("x264_param_t.i_qp_min: %d\n", p->rc.i_qp_min);
  printf("x264_param_t.i_qp_max: %d \n", p->rc.i_qp_max);
  printf("x264_param_t.i_qp_step: %d \n", p->rc.i_qp_step);
  printf("x264_param_t.i_bitrate: %d \n", p->rc.i_bitrate);
  // skipping .. 
  printf("--\n");
  printf("x264_param_t.b_aud: %d\n", p->b_aud);
  printf("x264_param_t.b_repeat_headers: %d \n", p->b_repeat_headers);
  printf("x264_param_t.b_annexb: %d (flv does not support annexb)\n", p->b_annexb);
  printf("x264_param_t.i_sps_id: %d \n", p->i_sps_id);
  printf("x264_param_t.b_vfr_input: %d\n", p->b_vfr_input);
  printf("x264_param_t.b_pulldown: %d\n", p->b_pulldown);
  printf("x264_param_t.i_fps_num: %d\n", p->i_fps_num);
  printf("x264_param_t.i_fps_den: %d\n", p->i_fps_den);
  printf("x264_param_t.i_timebase_num: %d \n", p->i_timebase_num);
  printf("x264_param_t.i_timebase_den: %d\n", p->i_timebase_den);
  printf("--------------------------------------------------\n");
}

void AV::printX264Headers(x264_nal_t* nal) {
  printf("--\n");
  printf("sps_size: %d\n", nal[0].i_payload);
  printf("pps_size: %d\n", nal[1].i_payload);
  printf("sei_size: %d\n", nal[2].i_payload);
  printf("--\n");
}
