#include <ogg/VorbisWriter.h>

// @todo: cleanup; this works, but it's not ready for stable use yet (e.g. samplerate is hardcoded)

VorbisWriter::VorbisWriter() 
  :fp(NULL)
  ,is_setup(false)
{
}

VorbisWriter::~VorbisWriter() {
  close();
}

void VorbisWriter::open(const std::string filepath) {
  /*
  #ifdef OMT
  fp = fopen(filepath.c_str(), "w+");
  if(!fp) {
    printf("ERROR: cannot open vorbis file...\n");
    fp = NULL;
    return;
  }
  
  // encode setup
  int r = 0;
  vorbis_info_init(&vi);
  r = vorbis_encode_init_vbr(&vi, 1, 16000, .4); // assuming 1 channel, 16Khz
  printf("init: %d\n", r);

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
  srand((unsigned)time(NULL));
  ogg_stream_init(&os, rand());
  
  // write headers
  ogg_packet header;
  ogg_packet header_comm;
  ogg_packet header_code;
  
  vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
  ogg_stream_packetin(&os, &header);
  ogg_stream_packetin(&os, &header_comm);
  ogg_stream_packetin(&os, &header_code);

  while(true) {
    int result = ogg_stream_flush(&os, &og);
    if(result == 0) {
      printf("VORBIS == 0\n");
      break;
    }
    fwrite(og.header, 1, og.header_len, fp);
    fwrite(og.body, 1, og.body_len, fp);
  }
  printf("DONE!\n");
#endif
  is_setup = true;
  */
}

void VorbisWriter::close() {

  if(fp == NULL) {
    return;
  }
  fclose(fp);
  fp = NULL;
  if(is_setup) {
    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);
  }
}

// @todo, nicely set e_o_s flag
void VorbisWriter::onAudioIn(const void* input, unsigned long nframes) {
  if(!is_setup) {
    printf("ERROR: onAudioIn(), first call open().\n");
    return;
  }
  int num_channels = 1;
  size_t nbytes = nframes * sizeof(short int) * num_channels;
  float** buffer = vorbis_analysis_buffer(&vd, nbytes);
  short int* input_ptr = (short int*)input;

  int dest_dx = 0;
  for(int i = 0; i < nframes; ++i) {
    int src_dx = i * num_channels;

    for(int j = 0; j < num_channels; ++j) {
      float src_value = (input_ptr[src_dx] / 32768.0f);
      // printf("%f\n", src_value);
      buffer[0][dest_dx++] = src_value;
      ++src_dx;
    }
  }
  int r = vorbis_analysis_wrote(&vd, nframes);
  if (r != 0) {
    printf("ERROR: error with vorbis_analysis_wrote\n");
  }

  // write out 
  while(vorbis_analysis_blockout(&vd, &vb) == 1) {
    vorbis_analysis(&vb, NULL);
    vorbis_bitrate_addblock(&vb);
    while(vorbis_bitrate_flushpacket(&vd, &op)) {
      ogg_stream_packetin(&os, &op);
      while(true) {
        int result = ogg_stream_pageout(&os, &og);
        if(result == 0) {
          break;
        }
        fwrite(og.header, 1, og.header_len, fp);
        fwrite(og.body, 1, og.body_len, fp);
      }
    }
  }
}
