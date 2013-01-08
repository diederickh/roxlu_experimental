#include <ogg/OggMaker.h>

OggMaker::OggMaker()
  :vfmt(OVF_RGB24)
  ,afmt(OAF_FLOAT32)
  ,use_audio(true)
  ,use_video(true)
  ,is_setup(false)
  ,image_w(640)
  ,image_h(480)
  ,td(NULL)
  ,in_stride(image_w * 3)
  ,fps(60)
  ,fp(NULL)
  ,yuv_y(NULL)
  ,yuv_u(NULL)
  ,yuv_v(NULL)
  ,num_video_frames(0)
  ,num_audio_frames(0) // number of times addAudioFrame is called, not the real number of 'sample frames'
  ,last_time(0)
  ,millis_per_frame(0)
  ,time_accum(0)
  ,bytes_per_sample(0)
  ,num_channels(0)
  ,samplerate(0)
  ,sws(NULL)
{
  millis_per_frame = (1.0/fps) * 1000;
}

OggMaker::~OggMaker() {
  // cleanup video
  if(use_video) {
    if(td != NULL) {
      oggStreamFlush(video_oss);
    }

    th_info_clear(&ti);
    if(td != NULL) {
      th_encode_free(td);
    }
    if(yuv_y != NULL) {
      delete[] yuv_y;
      delete[] yuv_u;
      delete[] yuv_v;
    }
    //ogg_stream_destroy(&video_oss); // gives SIGABRT
  }
  
  // cleanup audio
  if(use_audio) {
    oggStreamFlush(audio_oss);
    if(fp != NULL) {
      fclose(fp);
    }
    //ogg_stream_destroy(&audio_oss); // gives SIGABRT
  }
}

bool OggMaker::setup(OggVideoFormat videoFormat,
                     OggAudioFormat audioFormat,
                     int numChannels,
                     int sampleRate,
                     size_t bytesPerSample
)
{
  fp = fopen("monkey.ogv", "wb");
  if(!fp) {
    printf("ERROR: cannot open ogg file.\n");
    return false;
  }
  
  num_channels = numChannels;
  vfmt = videoFormat;
  afmt = audioFormat;
  samplerate = sampleRate;
  bytes_per_sample = bytesPerSample;

  /* @todo move to print
  printf("OggMaker.num_channels: %d\n", num_channels);
  printf("OggMaker.samplerate: %d\n", samplerate);
  printf("OggMaker.bytes_per_sample: %d\n", int(bytes_per_sample));
  printf("OggMaker.afmt: %s\n", (afmt == OAF_INT16) ? "OAF_INT16" : "OAF_FLOAT32");
  */

  // setup + write headers
  if(use_audio) {
    if(!vorbisSetup()) {
      return false;
    }
  }
  if(use_video) {
    if(!theoraSetup()) {
      return false;
    }
  }

  // flush 
  if(use_audio) {
    ogg_stream_packetin(&audio_oss, &vheader_comm);
    ogg_stream_packetin(&audio_oss, &vheader_code);
    oggStreamPageOut(audio_oss);
    oggStreamFlush(audio_oss);
  }
  if(use_video) {
    oggStreamFlush(video_oss);
  }

  is_setup = true;
  return true;
}

bool OggMaker::theoraSetup() {
  // @todo use vfmt member; we might have YUV422 too 
  // initialize encoder info
  th_info_init(&ti);
  ti.frame_width = ((image_w + 15) >> 4) << 4;
  ti.frame_height = ((image_h + 15) >> 4) << 4;
  ti.pic_width = image_w;
  ti.pic_height = image_h;
  ti.pic_x = 0;
  ti.pic_y = 0;
  ti.fps_numerator = fps;
  ti.fps_denominator = 1;
  ti.aspect_numerator = 1;
  ti.aspect_denominator = 1;
  ti.colorspace = TH_CS_UNSPECIFIED;
  ti.pixel_fmt = TH_PF_420;
  // ti.target_bitrate = 3000; /* just some guessed value for now */
  ti.quality = 63; /* 0 to 63 */
  
  // create encoder context
  td = th_encode_alloc(&ti);
  if(td == NULL) {
    printf("ERROR: cannot allocate theora encoder context.\n");
    return false;
  }

  th_info_clear(&ti);

  // Create comment
  th_comment comment;
  th_comment_init(&comment);
  th_comment_add(&comment, (char*)"roxlu");
  comment.vendor = (char*)"roxlu";
    
  ogg_packet pkt;
  ogg_page page;
  if(ogg_stream_init(&video_oss, rand()) < 0) {
    printf("ERROR: cannot ogg_stream_init()\n");
    return false;
  }

  // flush comment/header
  while(th_encode_flushheader(td, &comment, &pkt)) {
    if(ogg_stream_packetin(&video_oss, &pkt) < 0) {
      printf("ERROR: cannot ogg_stream_packetin()\n");
      return false;
      continue;
    }
    oggStreamPageOut(video_oss);
  }
  //  oggStreamFlush(video_oss);
  
  // setup encoder vars
  yuv_w = (image_w + 15) & ~15;
  yuv_h = (image_h + 15) & ~15;
  ycbcr[0].width = yuv_w;
  ycbcr[0].height = yuv_h;
  ycbcr[0].stride = yuv_w;
  ycbcr[1].width = (yuv_w >> 1);
  ycbcr[1].stride = ycbcr[1].width;
  ycbcr[1].height = (yuv_h >> 1);
  ycbcr[2].width = ycbcr[1].width;
  ycbcr[2].stride = ycbcr[1].stride;
  ycbcr[2].height = ycbcr[1].height;

  ycbcr[0].data = yuv_y = new unsigned char[ycbcr[0].stride * ycbcr[0].height];
  ycbcr[1].data = yuv_u = new unsigned char[ycbcr[1].stride * ycbcr[1].height];
  ycbcr[2].data = yuv_v = new unsigned char[ycbcr[2].stride * ycbcr[2].height];

  in_image = vpx_img_alloc(NULL, VPX_IMG_FMT_RGB24, image_w, image_h, 0);
  out_planes[0] = ycbcr[0].data;
  out_planes[1] = ycbcr[1].data;
  out_planes[2] = ycbcr[2].data;

  out_strides[0] = ycbcr[0].stride;
  out_strides[1] = ycbcr[1].stride;
  out_strides[2] = ycbcr[2].stride;

  // @todo use the "vfmt" member to make a conversion context 
  sws = sws_getContext(image_w, image_h, PIX_FMT_RGB24,
                       image_w, image_h, PIX_FMT_YUV420P,
                       SWS_FAST_BILINEAR, NULL, NULL, NULL
                       );
  if(sws == NULL) {
    printf("ERROR: cannot create sws convert context.");
    return false;
  }

  return true;
}

void OggMaker::oggStreamPageOut(ogg_stream_state& os) {
  ogg_page page;
  while(ogg_stream_pageout(&os, &page)) {
    if(fwrite(page.header, page.header_len, 1, fp) != 1) {
    }
    if(fwrite(page.body, page.body_len, 1, fp) != 1) {
    }
  }
}
void OggMaker::oggStreamFlush(ogg_stream_state& os) {
  ogg_page page;
  while(ogg_stream_flush(&os, &page) > 0) {
      if(fwrite(page.header, page.header_len, 1, fp) != 1) {
        break;
      }
      if(fwrite(page.body, page.body_len, 1, fp) != 1) {
        break;
      }
  }
}

bool OggMaker::vorbisSetup() {
  int r = 0;
  vorbis_info_init(&vi);
  r = vorbis_encode_init_vbr(&vi, num_channels, samplerate, .4); 

  if(r != 0) {
    printf("ERROR: cannot initialize vorbis\n");
    ::exit(0);
  }

  // create comment
  vorbis_comment_init(&vc);
  vorbis_comment_add_tag(&vc, "ENCODER", "VorbisWriter");
  
  // setup analysis
  vorbis_analysis_init(&vd, &vi);
  vorbis_block_init(&vd, &vb);

  // create ogg stream
  srand(time(NULL));
  if(ogg_stream_init(&audio_oss, rand()) != 0) {
    printf("ERROR: initializing audio stream.\n");
    return false;
  }
  
  // write headers
  ogg_page og;
  ogg_packet op;
  ogg_packet header;
  
  vorbis_analysis_headerout(&vd, &vc, &header, &vheader_comm, &vheader_code);
  ogg_stream_packetin(&audio_oss, &header);
  oggStreamPageOut(audio_oss);
  oggStreamFlush(audio_oss);
  return true;
}

void OggMaker::addAudioFrame(void* input, size_t nframes) {
  if(!use_audio) {
    return;
  }
  num_audio_frames++; 

  float** buffer = vorbis_analysis_buffer(&vd, nframes);

  // check if we need to convert the input.
  if(afmt == OAF_INT16) {
    short int* input_ptr = (short int*)input;
    float* out_channel_ptr = NULL;
    short int* in_channel_ptr = NULL;
    for(int i = 0; i < num_channels; ++i) {
      in_channel_ptr = input_ptr + i;
      out_channel_ptr = buffer[i];
      for(int j = 0; j < nframes; ++j) {
        out_channel_ptr[j] = (float(*(in_channel_ptr))) / 32768.0f;
        in_channel_ptr += num_channels;
      }
    }
  }
  else if(afmt == OAF_FLOAT32) {
    int src_dx = 0;
    int dest_dx = 0;
    float* input_ptr = (float*)input;
    float* out_channel_ptr = NULL;
    float* in_channel_ptr = NULL;
    for(int i = 0; i < num_channels; ++i) {
      src_dx = 0;
      out_channel_ptr = buffer[i];
      in_channel_ptr = input_ptr + i;
      for(int j = 0; j < nframes; ++j) {
        out_channel_ptr[j] = *in_channel_ptr;
        in_channel_ptr += num_channels;
      }
    }
  }

  int r = vorbis_analysis_wrote(&vd, nframes);
  if (r != 0) {
    printf("ERROR: error with vorbis_analysis_wrote\n");
  }

  // write out 
  ogg_packet op;
  while(vorbis_analysis_blockout(&vd, &vb) == 1) {
    vorbis_analysis(&vb, NULL);
    vorbis_bitrate_addblock(&vb);
    while(vorbis_bitrate_flushpacket(&vd, &op)) {
      ogg_stream_packetin(&audio_oss, &op);
      oggStreamPageOut(audio_oss);
      oggStreamFlush(audio_oss);
    }
  }
}

void OggMaker::addVideoFrame(void* pixels, size_t nbytes, int last) {
  if(!use_video) {
    return;
  }
  if(!is_setup) {
    printf("ERROR: cannot add video frame, first call setup()\n");
    return;
  }
  if(vfmt != OVF_RGB24) {
    printf("ERROR: cannot add video frame ... only OIF_RGB24 is supported for now.\n");
    return;
  }

  in_image = vpx_img_wrap(in_image, VPX_IMG_FMT_RGB24, image_w, image_h, 0, (unsigned char*)pixels);
  int os = sws_scale(sws, in_image->planes, in_image->stride, 0, image_h, out_planes, out_strides);

  // Encode
  int enc_result = th_encode_ycbcr_in(td, ycbcr);
  if(enc_result == TH_EFAULT) {
    printf("ERROR: cannot th_encode_ycbcr_in()\n");
    return;
  }

  // Write
  ogg_page page;
  ogg_packet pkt;
  num_video_frames++;

  while(th_encode_packetout(td, last, &pkt) > 0) {
    if(ogg_stream_packetin(&video_oss, &pkt) < 0) {
      printf("ERROR: ogg_stream_packetin() error.\n");
      continue;
    }
    oggStreamPageOut(video_oss);
    oggStreamFlush(video_oss);
  }
}

void OggMaker::duplicateFrames(int dup) {
  if(!use_video) {
    return;
  }

  if(!is_setup) {
    printf("ERROR: cannot duplicateFrames, first call setup()\n");
    return;
  }

  int r = th_encode_ctl(td, TH_ENCCTL_SET_DUP_COUNT, (void*)&dup, sizeof(dup));
  if(r != 0) {
    printf("ERROR: cannot set DUP_COUNT.\n");
  }
  int last = 0;
  ogg_packet pkt;
  while(th_encode_packetout(td, last, &pkt) > 0) {
    if(ogg_stream_packetin(&video_oss, &pkt) < 0) {
      printf("ERROR: ogg_stream_packetin() error.\n");
      continue;
    }
    oggStreamPageOut(video_oss);
    oggStreamFlush(video_oss);
  }
}

// print debug info
void OggMaker::print() {
  printf("\n");
  printf("------------------------------------------------\n");
  printf("th_info.frame_width: %d\n", ti.frame_width);
  printf("th_info.frame_height: %d\n", ti.frame_height);
  printf("th_info.pic_width: %d\n", ti.pic_width);
  printf("th_info.pic_height: %d\n", ti.pic_height);
  printf("th_info.pic_x: %d\n", ti.pic_x);
  printf("th_info.pic_y: %d\n", ti.pic_y);
  printf("th_info.colorspace: %s\n", colorspaceToString(ti.colorspace).c_str());
  printf("th_info.pixel_fmt: %s\n", pixelformatToString(ti.pixel_fmt).c_str());
  printf("th_info.version_major: %d\n", ti.version_major);
  printf("th_info.version_minor: %d\n", ti.version_minor);
  printf("th_info.version_subminor: %d\n", ti.version_subminor);
  printf("th_info.fps_numerator: %d\n", ti.fps_numerator);
  printf("th_info.fps_denominator: %d\n", ti.fps_denominator);
  printf("th_info.aspect_numerator: %d\n", ti.aspect_numerator);
  printf("th_info.aspect_denominator: %d\n", ti.aspect_denominator);
  printf("th_info.target_bitrate: %d\n", ti.target_bitrate);
  printf("th_info.quality: %d\n", ti.quality);
  printf("------------------------------------------------\n");
  printf("\n");
}
