#include <flv/FLV.h>

FLV::FLV()
  :cb_flush(NULL)
  ,cb_rewrite(NULL)
  ,cb_user(NULL)
  ,read_dx(0)
  ,sei_data(NULL)
  ,sei_size(0)
  ,framerate_pos(0)
  ,duration_pos(0)
  ,filesize_pos(0)
  ,datarate_pos(0)
  ,bytes_flushed(0)
  ,bytes_written(0)
  ,last_video_timestamp(0)
  ,total_num_frames(0)
  ,is_opened(false)
{
}

FLV::~FLV() {
  FLVCloseParams p;
  close(p);
}

int FLV::open(FLVHeader header) {
  flv_header = header;

  putTag("FLV");                                                                     // default flv first 3 bytes
  put8(1);                                                                           // version
  put8( 0x00 | header.has_audio << 2 | header.has_video); // has video + audio
  put32(9);                                                                          // data offest
  put32(0);                                                                          // previous tag size

  is_opened = true;

  flush();
  return 1;
}

int FLV::writeParams(FLVParams params) {
  if(!is_opened) {
    printf("WARNING: cannot write flv params; flv stream is closed\n");
    return 0;
  }

  size_t start_size = size();
  size_t data_size_pos = start_size + 1; 
  x264_param_t* p = params.x264_params;
  int is_encrypted = 0; 

  int metadata_count = 5 * flv_header.has_video +   // width, height, videodatarate, framerate, videocodecid
                       5 * flv_header.has_audio +   // audiodatarate, audiosamplerate, audiosamplesize, stereo, audiocodecid
                       4;                            // canSeekToEnd, duration, filesize

  // tag info header
  put8(0x00 | ((is_encrypted) ? 0x20 : 0x00) | FLV_TAG_SCRIPT_DATA); 
  put24(0);            // data length
  put24(0);            // timestamp
  put8(0);             // timestamp extended
  put24(0);            // stream id, always 0

  // meta data
  size_t data_size_start = size();
  put8(AMF0_TYPE_STRING);
  putAmfString("onMetaData");

  put8(AMF0_TYPE_ECMA_ARRAY);
  put32(metadata_count);            // 7 elements

  if(flv_header.has_video) {
    putAmfString("width");
    putAmfDouble(p->i_width);
  
    putAmfString("height");
    putAmfDouble(p->i_height);
    
    putAmfString("videodatarate");
    datarate_pos = bytes_written + 1;
    putAmfDouble(0);
  
    putAmfString("framerate");
    framerate_pos = bytes_written + 1;

    if(!p->b_vfr_input) {
      putAmfDouble((double)p->i_fps_num / p->i_fps_den);
    }
    else {
      printf("FLV WARNING: WE HAVE VFR_INPUT, WE NEED TO RECALC THE FPS AT THE END.\n");
      putAmfDouble(0);
    }

    putAmfString("videocodecid");
    putAmfDouble(FLV_VIDEOCODEC_AVC);
  }

  if(flv_header.has_audio) {
    putAmfString("audiodatarate");
    putAmfDouble(flv_header.sound_bitrate/1024.0);

    putAmfString("audiosamplerate");
    putAmfDouble(44100.0);
    
    putAmfString("audiodatarate");
    putAmfDouble(flv_header.sound_bits == FLV_SOUNDSIZE_16BIT ? 16.0 : 8.0);

    putAmfString("stereo");
    putAmfBool(flv_header.sound_type == FLV_SOUNDTYPE_STEREO);

    putAmfString("audiocodecid");
    putAmfDouble(FLV_SOUNDFORMAT_MP3);
  }

  putAmfString("streamName");
  put8(AMF0_TYPE_STRING);
  putAmfString("roxlu");

  putAmfString("canSeekToEnd");
  putAmfBool(false);

  putAmfString("duration");
  duration_pos = bytes_written + 1;
  putAmfDouble(0); 
  
  putAmfString("filesize");
  filesize_pos = bytes_written + 1;
  putAmfDouble(0);
  
  putAmfString("");
  put8(AMF0_TYPE_OBJECT_END);

  size_t data_size = size() - data_size_start;

  rewrite24(data_size, data_size_pos); // DataSize
  put32(size() - start_size); // PreviousTagSize

  dump();
  flush();

  return 1;
}

int FLV::writeVideoPacket(FLVVideoPacket pkt) {
  if(!is_opened) {
    printf("WARNING: cannot write video packet; flv stream is closed\n");
    return 0;
  }

  size_t start_size = size();
  size_t data_size_pos = start_size + 1; 
  int is_encrypted = 0; 

  // tag info header
  put8(0x00 |  ((is_encrypted) ? 0x20 : 0x00) | FLV_TAG_VIDEO);
  put24(0);                             // data length of this packet (rewritten at the end)
  put24(pkt.timestamp);                 // timestamp
  put8(0);                              // timestamp extended
  put24(0);                             // stream id, always 0
  size_t data_size_start = size();

  // video data (page 78)
  // @todo we need to set the correct offset for the composition time offets
  put8((pkt.is_keyframe) ? (0x01 << 4 | FLV_VIDEOCODEC_AVC) : (0x02 << 4 | FLV_VIDEOCODEC_AVC)); // keyframe or inter frame + codec id
  put8(0x01);                           // AVCPacketType, 1 = AVC NALU 
  put24(0);                             // CompositionTime, if AVCPacketType == 1 then we need to set the composition time offset

  if(sei_data) {
    put(sei_data, sei_size);
    delete[] sei_data; 
    sei_data = NULL;
    sei_size = 0;
  }

  // avc-video-packet
  put(pkt.nals_data, pkt.nals_size);

  size_t data_size = size() - data_size_start;
  rewrite24(data_size, data_size_pos);  // DataSize
  put32(size() - start_size);           // PreviousTagSize

  flush();

  last_video_timestamp = pkt.timestamp;
  total_num_frames++;
  return 1;
}

// This is what's call the "AVC sequence header" at page 78 of 52991022-flv.pdf, 
// "Adobe Flash Video File Format Specification".  This is the x264 header describing
// the used encoding settings. The SEI contains the used encoding settings.
int FLV::writeAvcSequenceHeader(FLVAvcSequenceHeader hdr) {
  if(!is_opened) {
    printf("WARNING: cannot write avc sequence header; flv stream is closed\n");
    return 0;
  }

  size_t start_size = size();
  size_t tag_size_pos= start_size + 1;
  int is_encrypted = 0;
  x264_nal_t* nal = hdr.nals;
  
  int sps_size = nal[0].i_payload;
  int pps_size = nal[1].i_payload;
  int sei_size = nal[2].i_payload;
  rx_uint8* sps = nal[0].p_payload + 4;

  sei_data = new rx_uint8[sei_size];
  memcpy((char*)sei_data, nal[2].p_payload, sei_size);

  // tag info header
  put8(FLV_TAG_VIDEO);                       // keyframe, avc codec, unencrypted
  put24(0);                                 // data length of this packet (rewritten at the end)
  put24(0);                                 // timestamp
  put8(0);                                  // timestamp extended
  put24(0);                                 // stream id, always 0
  size_t tag_size_start = size();

  put8((0x01 << 4 | FLV_VIDEOCODEC_AVC));   // keyframe
  put8(0);                                  // AVCPacketType, 0 = AVC Sequence Header
  put24(0);                                 // CompositionTime

  // SPS, AVCDecoderConfigurationRecord, ISO-14496-15, ( T-REC-H.264-201201, page 62, sec. 7.3.2.1.1 )
  put8(1);                                  // configurationVersion
  put8(sps[1]);                             // AVCProfileIndication
  put8(sps[2]);                             // profile_compatibility 
  put8(sps[3]);                             // AVCLevelIndication
  put8(0xff);                               // from flv.c of x264: 6 bits reserved (111 111) + 2 bits nal size length -1
  put8(0xe1);                               // from flv.c of x264: 2 bits reserved (111) + 5 bits number of sps(00001)
  
  put16(sps_size - 4);                      // sequencParameterSetLength
  put(sps, sps_size - 4);                   // sequenceParameterSetNalUnit

  // PPS
  put8(1);                                  // numOfPictureParameterSets
  put16(pps_size - 4);                      // pictureParameterSetLength
  put(nal[1].p_payload + 4, pps_size - 4);  // pictureParameterSetNalUnit

  // rewrite tag size + PreviousTagSize
  size_t data_size = size() - tag_size_start;
  rewrite24(data_size, tag_size_pos);
  put32(size() - start_size);
  //  flush();
  return 1;
}

int FLV::writeAudioPacket(FLVAudioPacket pkt) {
  if(!is_opened) {
    printf("WARNING: cannot write audio packet; flv stream is closed\n");
    return 0;
  }

  size_t start_size = size();
  size_t tag_size_pos= start_size + 1;
  int is_encrypted = 0;

  // tag info header
  put8(0x00 |  ((is_encrypted) ? 0x20 : 0x00) | FLV_TAG_AUDIO);
  put24(0);                         // data length, is rewritten later
  put24(pkt.timestamp);             // timestamp
  put8(0);                          // timestamp extended
  put24(0);                         // stream id, always 0
  size_t tag_size_start = size();

  // audio tag header
  put8((FLV_SOUNDFORMAT_MP3 << 4) |                  // SoundFormat
       (flv_header.sound_samplerate << 2) |          // Sound rate
       (flv_header.sound_bits << 1) |                // sound size (compressed always 1)
       (flv_header.sound_type << 0));                // Sound type mono or stereo
  put(pkt.data, pkt.size);

  // rewrite
  size_t data_size = size() - tag_size_start;
  rewrite24(data_size, tag_size_pos);
  put32(size() - start_size);
  flush();

  return 1;
}

int FLV::close(FLVCloseParams p) {
  if(!is_opened) {
    printf("WARNING: trying to close FLV, but we're not opened. Maybe you already closed flv?\n");
    return 0;
  }

  cb_flush(cb_user);
  
  double d = 0.0;
  rx_uint64 v = 0.0;


  if(total_num_frames > 0) {
    // framerate
    double framerate = double(last_video_timestamp) / total_num_frames;
    v = swapDouble(framerate);
    cb_rewrite((char*)&v, sizeof(rx_uint64), framerate_pos, cb_user);

    // duration
    d = double(last_video_timestamp) / 1000.0;
    v = swapDouble(d);
    cb_rewrite((char*)&v, sizeof(rx_uint64), duration_pos, cb_user);

    // filesize
    v = swapDouble(bytes_flushed);
    cb_rewrite((char*)&v, sizeof(rx_uint64), filesize_pos, cb_user);

    // datarate
    d = (bytes_flushed * 8) / last_video_timestamp;
    v = swapDouble(d);
    cb_rewrite((char*)&v, sizeof(rx_uint64), datarate_pos, cb_user);
  }

  cb_close(cb_user);

  read_dx = 0;
  bytes_written = 0;
  bytes_flushed = 0;
  framerate_pos = 0;
  duration_pos = 0;
  filesize_pos = 0;
  datarate_pos = 0;
  last_video_timestamp = 0;
  total_num_frames = 0;
  sei_size = 0;
  is_opened = false;
  buffer.clear();
  return 1;
}


void FLV::dump() {
  read_dx = 0;
  rx_uint8 type = 0;
  rx_uint8 tag = 0;
  rx_uint32 len = 0;
  rx_uint32 data = 0;
  printf("----------------------------------\n");
  printf("Signature: %c\n", get8());
  printf("Signature: %c\n", get8());
  printf("Signature: %c\n", get8());
  printf("Version: %d\n", get8());
  type = get8();
  printf("TypeFlagsReserved: %d\n", (type & 0xf8));
  printf("TypeFlagsAudio: %d\n", (type & 0x04));
  printf("TypeFlagsReserved: %d\n", (type & 0x02));
  printf("TypeFlagsVideo: %d\n", (type & 0x01));
  len = get32();
  printf("DataOffset: %d\n", len);
  printf("----------------------------------\n");
  len = get32();
  printf("PreviousTagSize0: %d\n", len);
  printf("----------------------------------\n");
  size_t tag_count = 0;
  while(hasDataToRead()) {
    tag = get8();
    printf("Tag%ld.Reserved: %d\n", tag_count, (tag & 0xC0));
    printf("Tag%ld.Filter: %d\n", tag_count,  (tag & 0x20));
    printf("Tag%ld: %s\n", tag_count, flvTagToString(tag).c_str());
    len = get24();
    printf("Tag%ld.DataSize: %d\n", tag_count, len);
    data = get24();
    printf("Tag%ld.Timestamp: %d\n", tag_count, data);
    type = get8();
    printf("Tag%ld.TimestampExtended: %d\n", tag_count, type);
    data = get24();
    printf("Tag%ld.StreamID: %d\n", tag_count, data);
    switch(tag) {
      case FLV_TAG_SCRIPT_DATA: {
        if((tag & 0x20) == 0x20) {
          printf("ERROR: we do not parse encrypted data.\n");
          read_dx += len;
          continue;
        }
        dumpAmf0(get8());
        break;
      }
    }
    tag_count++;
    printf("----------------------------------\n");
    break;
  }
}

void FLV::dumpAmf0(int type) {
  rx_uint32 len = 0;
  double number = 0;

  while(hasDataToRead()) {
    if(type == -1) {
      type = get8();
    }
    switch(type) {
      case AMF0_TYPE_STRING: {
        printf("amf0_type_string: %s\n", getAmfString().c_str());
        dumpAmf0();
        return;
      }
      case AMF0_TYPE_NUMBER: {
        number = getAmfDouble();
        printf("amf0_type_number: %f\n", number);
        return;
      };
      case AMF0_TYPE_ECMA_ARRAY: {
        len = get32();
        printf("amf0_type_emca_array: size = %d\n", len);
        printf("--\n");
        for(rx_uint32 i = 0; i < len; ++i) {
          printf("amf0_type_emca_array: %s\n", getAmfString().c_str());
          dumpAmf0();
        }
        printf("amf0_type_ecma_array (end string): '%s'\n", getAmfString().c_str());
        printf("amf0_type_ecma_array (end marker): %s\n", flvAmf0TypeToString(get8()).c_str());
        printf("--\n");
        return;
      };
      case AMF0_TYPE_BOOLEAN: {
        printf("amf0_type_boolean: %d\n", get8());
        return;
      }
      default: {
        printf("ERROR: FLV::dumpAmf0(), we do not handle the type '%s' yet.\n", flvAmf0TypeToString(type).c_str());
        ::exit(EXIT_FAILURE);
      };
    }
  }
}

std::string FLV::flvTagToString(int tag) {
  switch(tag) {
    case FLV_TAG_NONE: return "FLV_TAG_NONE";
    case FLV_TAG_AUDIO: return "FLV_TAG_AUDIO";
    case FLV_TAG_VIDEO: return "FLV_TAG_VIDEO";
    case FLV_TAG_SCRIPT_DATA: return "FLV_TAG_SCRIPT_DATA";
    default: return "UNKNOWN";
  }
}

std::string FLV::flvAmf0TypeToString(int type) {
  switch(type) {
    case AMF0_TYPE_NUMBER: return "AMF0_TYPE_NUMBER"; 
    case AMF0_TYPE_BOOLEAN: return "AMF0_TYPE_BOOLEAN";
    case AMF0_TYPE_STRING: return "AMF0_TYPE_STRING";
    case AMF0_TYPE_OBJECT: return "AMF0_TYPE_OBJECT";
    case AMF0_TYPE_MOVIECLIP: return "AMF0_TYPE_MOVIECLIP";
    case AMF0_TYPE_NULL: return "AMF0_TYPE_NULL";
    case AMF0_TYPE_UNDEFINED: return "AMF0_TYPE_UNDEFINED";
    case AMF0_TYPE_REFERENCE: return "AMF0_TYPE_REFERENCE";
    case AMF0_TYPE_ECMA_ARRAY: return "AMF0_TYPE_ECMA_ARRAY";
    case AMF0_TYPE_OBJECT_END: return "AMF0_TYPE_OBJECT_END";
    case AMF0_TYPE_STRICT_ARRAY: return "AMF0_TYPE_STRICT_ARRAY";
    case AMF0_TYPE_DATE: return "AMF0_TYPE_DATE";
    case AMF0_TYPE_LONG_STRING: return "AMF0_TYPE_LONG_STRING";
    case AMF0_TYPE_UNSUPPORTED: return "AMF0_TYPE_UNSUPPORTED";
    case AMF0_TYPE_RECORDSET: return "AMF0_TYPE_RECORDSET";
    case AMF0_TYPE_XML: return "AMF0_TYPE_XML";
    case AMF0_TYPE_TYPED_OBJECT: return "AMF_TYPE_TYPED_OBJECT";
    default: return "UNKNOWN";
  }
};


FLVFileWriter::FLVFileWriter() 
  :fp(NULL)
{
}

FLVFileWriter::~FLVFileWriter() {
  if(fp != NULL) {
    fclose(fp);
    fp = NULL;
  }
}

bool FLVFileWriter::open(std::string filepath) {
  RX_VERBOSE("open file: %s", filepath.c_str());

  fp = fopen(filepath.c_str(), "wb");
  if(!fp) {
    RX_ERROR(("cannot open the given file: %s", filepath.c_str()));
    return false;
  }
  return true;
}

size_t flv_file_write(char* data, size_t nbytes, void* user) {
  if(nbytes <= 0) {
    return 0;
  }
  FLVFileWriter* f = static_cast<FLVFileWriter*>(user);
  if(f->fp == NULL) {
    RX_WARNING(("was asked to write to a file, but the file is not open."));
    return 0;
  }

  size_t written  = fwrite(data, nbytes, 1, f->fp);

  if(written != 1) {
    RX_WARNING(("failed to write to file."));
    return 0;
  }
  return nbytes;
}

void flv_file_rewrite(char* data, size_t nbytes, size_t pos, void* user) {
  FLVFileWriter* f = static_cast<FLVFileWriter*>(user);
  if(f->fp == NULL) {
    RX_WARNING(("was asked to rewrite to a file, but the file is not open."));
    return ;
  }

  RX_VERBOSE("rewrite at %d.", int(pos));
  long int curr_pos = ftell(f->fp);
  fseek(f->fp, pos, SEEK_SET);

  size_t written = fwrite(data, nbytes, 1, f->fp);
  if(written != 1) {
    RX_ERROR(("cannot rewrite some bytes."));
  }

  fseek(f->fp, curr_pos, SEEK_SET);
}

void flv_file_flush(void* user) {
  FLVFileWriter* f = static_cast<FLVFileWriter*>(user);
  if(f->fp == NULL) {
    RX_WARNING(("was asked to flush file, but the file is not open."));
    return;
  }

  RX_VERBOSE("flush data");
  fflush(f->fp);
}

void flv_file_close(void* user) {
  FLVFileWriter* f = static_cast<FLVFileWriter*>(user);
  if(f->fp == NULL) {
    RX_WARNING(("was asked to flush file, but the file is not open.\n"));
    return;
  }

  RX_VERBOSE("close flv file.");
  fclose(f->fp);
  f->fp = NULL;
}
