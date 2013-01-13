#include <ogg/VorbisWriter.h>

// @todo: cleanup; this works, but it's not ready for stable use yet (e.g. samplerate is hardcoded)

VorbisWriter::VorbisWriter() 
  :fp(NULL)
  ,is_setup(false)
  ,num_channels(0)
  ,samplerate(0)
  ,bytes_per_sample(0)
  ,format(VW_FLOAT32)
{
}

VorbisWriter::~VorbisWriter() {
  close();
}

void VorbisWriter::open(const std::string filepath, int sampleRate, int nchannels, size_t bytesPerSample, VorbisWriterFormat f) {
  fp = fopen(filepath.c_str(), "wb+");
  if(!fp) {
    printf("ERROR: cannot open vorbis file...\n");
    fp = NULL;
    return;
  }
  
  // encode setup
  samplerate = sampleRate;
  num_channels = nchannels;
  bytes_per_sample = bytesPerSample;
  format = f;

  int r = 0;
  vorbis_info_init(&vi);
  r = vorbis_encode_init_vbr(&vi, num_channels, sampleRate, .4); // .4 = quality

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
      break;
    }
    fwrite(og.header, 1, og.header_len, fp);
    fwrite(og.body, 1, og.body_len, fp);
  }

  is_setup = true;
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

  is_setup = false;
}

// @todo, nicely set e_o_s flag
void VorbisWriter::onAudioIn(const void* input, unsigned long nframes) {
  if(!fp) {
    return;
  }
  if(!is_setup) {
    printf("ERROR: onAudioIn(), first call open().\n");
    return;
  }

  float** buffer = vorbis_analysis_buffer(&vd, nframes);

  // check if we need to convert the input.
  if(format == VW_INT16) {
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
  else if(format == VW_FLOAT32) {
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
