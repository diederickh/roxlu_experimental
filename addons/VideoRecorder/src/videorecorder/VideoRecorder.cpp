#include <videorecorder/VideoRecorder.h>

#if RGB_CONVERTER == CONVERTER_SWSCALE
VideoRecorder::VideoRecorder(int inW, int inH, int outW, int outH, int fps)
  :in_width(inW)
  ,in_height(inH)
  ,out_width(outW)
  ,out_height(outH)

#else
   VideoRecorder::VideoRecorder(int inW, int inH, int fps)
   :in_width(inW)
  ,in_height(inH)
  ,out_width(inW)
  ,out_height(inH)
#endif
  ,in_stride(inW * 3)
  ,fps(fps) 
  ,encoder(NULL)
#if RGB_CONVERTER == CONVERTER_SWSCALE
  ,sws(NULL)
#endif
  ,fp(NULL)
  ,io(NULL)
  ,num_frames(0)
  ,vfr_input(false)
  ,spx_enc(NULL)
  ,start_time(0)
   // timeing
   /*
     ,last_dts(0)
     ,prev_dts(0)
     ,first_dts(0)
     ,largest_pts(-1)
     ,second_largest_pts(-1)
     ,duration(0)
   */
{
  rec_params.video_width = out_width;
  rec_params.video_height = out_height;
  rec_params.video_codec_id = FLV_VIDEOCODEC_AVC;
  rec_params.audio_codec_id = FLV_SOUNDFORMAT_SPEEX;
  rec_params.x264_param = &params;
  rec_params.x264_nal = nals;
  rec_params.x264_pic = &pic_out;
}

VideoRecorder::~VideoRecorder() {
  closeFile();
}

void VideoRecorder::initEncoders() {
  initVideoEncoder();
  initAudioEncoder();
  if(io) {
    io->writeParams(&rec_params);
    io->writeHeaders(&rec_params);
  }
}

void VideoRecorder::writeIOHeaders() {
  //  io->writeHeaders(&rec_params);
}

void VideoRecorder::initAudioEncoder() {
  int quality = 4;
  speex_bits_init(&spx_bits);
  spx_enc = speex_encoder_init(&speex_wb_mode); // wideband, 16khz
  speex_encoder_ctl(spx_enc, SPEEX_SET_QUALITY, &quality);
  speex_encoder_ctl(spx_enc, SPEEX_GET_FRAME_SIZE, &rec_params.audio_frame_size);

  rec_params.audio_sample_rate = 16000;
  rec_params.audio_timebase = 1.0 / rec_params.audio_sample_rate;
  rec_params.audio_codec_id = FLV_SOUNDFORMAT_SPEEX;

  printf("> Audio sample rate: %d\n", rec_params.audio_sample_rate);
  printf("> Audio timebase: %f\n", rec_params.audio_timebase);
  printf("> Audio frame size: %d\n", rec_params.audio_frame_size);

}

void VideoRecorder::initVideoEncoder() {
  setParams();

  // create the encoder
  encoder = x264_encoder_open(&params);
  if(!encoder) {
    printf("Error: cannot open encoder.\n");
    ::exit(1);
  }
  x264_encoder_parameters(encoder, &params);

  if(io) {
    //		io->writeParams(&rec_params);
  }

  // allocate picture / sws
  x264_picture_alloc(&pic_in, X264_CSP_I420, out_width, out_height);
#if RGB_CONVERTER == CONVERTER_SWSCALE
  sws = sws_getContext(in_width, in_height, PIX_FMT_RGB24, out_width, out_height, PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
#endif


  int nheader;
  x264_encoder_headers(encoder, &nals, &nheader);
  rec_params.x264_nal = nals;
  if(io) {
    //		io->writeHeaders(&rec_params);
  }

}

void VideoRecorder::setParams() {
  x264_param_t* p = &params;
  x264_param_default_preset(p, "ultrafast", "zerolatency");
  p->i_threads = 1;
  p->i_width = out_width;
  p->i_height = out_height;
  p->i_fps_num = fps;
  p->i_fps_den = 1;

  // intra refresh
  p->i_keyint_max = fps;
  p->i_keyint_max = 1; // for streaming we need to 
  p->b_intra_refresh = 1;


  // rate control
  p->rc.i_rc_method = X264_RC_CRF;
  p->rc.f_rf_constant = 25;
  p->rc.f_rf_constant_max = 35;
  p->rc.i_qp_min = 1; // @todo adjust for correct streaming quality
  p->rc.i_qp_max = 10;


  // streaming
  p->b_repeat_headers = 0; // 1 for streaming, for now this only works with '0'
  p->b_annexb = 0; // 1 for streaming, for now this only works with '0'

  x264_param_apply_profile(p, "baseline");
}

bool VideoRecorder::openFile(const char* filepath) {
  fp = fopen(filepath, "w+b");
  if(!fp) {
    printf("Error: Cannot open raw file.\n");
    return false;
  }

  if(io) {
    io->writeOpenFile(&rec_params);
  }

  initEncoders();

  int r = writeHeaders();
  if(r < 0) {
    printf("Error: cannot write headers.\n");
    return false;
  }

  return true;
}

int VideoRecorder::writeHeaders() {

  x264_nal_t* n = nals;
  int size = n[0].i_payload + n[1].i_payload + n[2].i_payload;
  if(fwrite(n[0].p_payload, size, 1, fp)) {
    return size;
  }

  return 1;
}

// encode raw pcm 16bit signed data
int VideoRecorder::addAudioFrame(short int* data, int size) {
  if(!start_time) {
    start_time = Timer::now();
  }
	
  speex_bits_reset(&spx_bits);
  speex_encode_int(spx_enc, data, &spx_bits);
  int written = speex_bits_write(&spx_bits, spx_buffer, sizeof(spx_buffer));

  rec_params.spx_num_bytes = written;
  rec_params.spx_buffer = spx_buffer;
  rec_params.audio_num_encoded_samples += rec_params.audio_frame_size;

  if(io) {
    //	io->writeAudioFrame(&rec_params);
  }
  printf("@ Audio frame size: %d, encoded samples: %d\n", written, rec_params.audio_num_encoded_samples);
  AudioPacket* pkt = new AudioPacket();
  pkt->data = new rx_uint8[written];
  pkt->data_size = written;
  pkt->time_dts = Timer::now() - start_time;
  memcpy(pkt->data, spx_buffer, written);
  pkt->dts = (rec_params.audio_num_encoded_samples * rec_params.audio_timebase) * 1000; 
  audio_packets.push_back(pkt);

  AVInfoPacket info_pkt;
  info_pkt.av_type = AV_AUDIO;
  info_pkt.dts = pkt->time_dts;
  info_pkt.dx = audio_packets.size() - 1;
  info_packets.push_back(info_pkt);

  writePackets();
  return written;
}

int VideoRecorder::addVideoFrame(unsigned char* pixels) {
  if(!start_time) {
    start_time = Timer::now();
  }
#if RGB_CONVERTER == CONVERTER_SWSCALE
  int h = sws_scale(sws
                    ,(const  uint8_t* const*)&pixels 
                    ,&in_stride
                    ,0
                    ,in_height
                    ,pic_in.img.plane
                    ,pic_in.img.i_stride
                    );
#else
  int r = libyuv::RGB24ToI420(pixels
                              ,in_stride
                              ,pic_in.img.plane[0], pic_in.img.i_stride[0]
                              ,pic_in.img.plane[1], pic_in.img.i_stride[1]
                              ,pic_in.img.plane[2], pic_in.img.i_stride[2]
                              ,in_width, in_height);
#endif

  pic_in.i_pts = num_frames;
  ++num_frames;

  int frame_size = x264_encoder_encode(encoder, &nals, &num_nals, &pic_in, &pic_out);
  if(frame_size == 0) {
    printf("Error: frame size is 0\n");
    return 0;
  }
  else {
    if(io) {
      // store a new video packet.
		
      VideoPacket* pkt = new VideoPacket();;
      //			info_pkt.dts = (pic_out.i_dts * (1.0f/fps)) * 1000 + 0.5;
      pkt->dts = pic_out.i_dts;
      pkt->pts = pic_out.i_pts;
      pkt->time_dts = (Timer::now() - start_time);

      //pkt->dts = (pic_out.i_dts * (1.0f/fps) * 1000 + 0.5f);
      //pkt->pts = (pic_out.i_pts * (1.0f/fps) * 1000 + 0.5f);


      pkt->data = new rx_uint8[frame_size];
      memcpy(pkt->data, nals[0].p_payload, frame_size);
      pkt->data_size = frame_size;
      pkt->is_keyframe = pic_out.b_keyframe;
      video_packets.push_back(pkt);

      AVInfoPacket info_pkt;
      info_pkt.dx = video_packets.size() - 1;
      info_pkt.av_type = AV_VIDEO;
      //info_pkt.dts = (pic_out.i_dts * (1.0f/fps)) * 1000 + 0.5; // working
      info_pkt.dts = pkt->time_dts;
			
      //printf("Info_pkt.dts: %d, VideoPacket.dts: %d, VideoPacket.pts: %d\n", info_pkt.dts, pkt->dts, pkt->pts);
      info_packets.push_back(info_pkt);
      //io->writeVideoPacket(pkt);
      //delete pkt;

      writePackets();
						
      rec_params.x264_nal = nals;
      rec_params.x264_frame_size = frame_size;
      rec_params.x264_pic = &pic_out;
      //io->writeVideoFrame(&rec_params);
    }

    if(fwrite(nals[0].p_payload, frame_size, 1, fp)) {
      //      return frame_size;
    }

    return frame_size;
  }

  return -1;
}

void VideoRecorder::closeFile() {

  if(fp) {
    fclose(fp);
    fp = NULL;
  }
  if(io) {
    io->writeCloseFile(&rec_params);
  }
}


/*
  This function handles all logic related to muxing. We interleave the 
  packets based on their dts (decompression timestamp)
*/
void VideoRecorder::writePackets() {
  if(info_packets.size() < 2) {
    return ;
  }
  std::sort(info_packets.begin(), info_packets.end(), AVInfoPacketSorter());
  for(int i = 0; i < info_packets.size(); ++i) {
    AVInfoPacket& info_pkt = info_packets[i];
    printf("InfoPacket: %d, Type: %c, DTS: %d\n", i, info_pkt.av_type == AV_VIDEO ? 'V' : 'A', info_pkt.dts);
    if(info_pkt.av_type == AV_AUDIO) {
      io->writeAudioPacket(audio_packets[info_pkt.dx]);
      delete audio_packets[info_pkt.dx];
    }
    else {
      io->writeVideoPacket(video_packets[info_pkt.dx]);
      delete video_packets[info_pkt.dx];
    }
    printf("==============\n\n");
    //io->writeVideoPacket(video_packets[i]);
  }
  audio_packets.clear();
  video_packets.clear();
  info_packets.clear();
	
}









