#include <videorecorder/FLV.h>

FLV::FLV()
  :pos_file_size(0)
  ,pos_duration(0)
  ,pos_datarate(0)
  ,time_start(0)
  ,vfr_input(false)
  ,prev_dts(0)
  ,prev_cts(0)
  ,delay_time(0)
  ,init_delta(0)
  ,delay_frames(0)
  ,fps_num(0)
  ,fps_den(0)
  ,framenum(0)
  ,dts_compress(false)
  ,sei(NULL)
{
}

FLV::~FLV() {

}

// IO
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FLV::loadFile(const char* filepath) {
  buffer.loadFile(filepath);
}


void FLV::saveFile(const char* filepath) {

  // rewrite the file size.
  Buffer tmp;
  amf.putNumberAMF0(tmp, AMFNumber(buffer.size()));
  buffer.putBytes(tmp.getPtr(), tmp.size(), pos_file_size);

  // rewrite 

  // save the buffer
  buffer.saveFile(filepath);
}


// R E A D I N G
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int FLV::readHeader() {
  // should print 'FLV'
  char file_tag[4];
  buffer.getBytes(file_tag, 3);
  file_tag[3] = '\0';
  printf("File tag: %s\n", file_tag);

  // version
  rx_uint8 b8 = buffer.getByte();
  printf("Version: %d\n", b8);

  // Audio / Video flag
  b8 = buffer.getByte();
  printf("Audio/Video flag: %02x\n", b8);

  // Data offset
  rx_uint32 b32 = buffer.getBigEndianU32();
  printf("Data offset: %d\n", b32);

  // Previous tag size
  b32 = buffer.getBigEndianU32();
  printf("Previous tag size: %d\n", b32);
  return -1;
}

int FLV::readTag() {
  printf("++++++++\n");
  rx_uint8 tag_id = buffer.getByte();
  printType(tag_id);
	
  rx_uint32 b32  = buffer.getBigEndianU24();
  printf("Data size: %d\n", b32);
	
  b32 = buffer.getBigEndianU24();
  printf("Timestamp: %d\n", b32);
		
  rx_uint8 b8 = buffer.getByte();
  printf("Timestamp extended: %d\n", b8);

  b32 = buffer.getBigEndianU24();
  printf("Stream ID: %d\n", b32);

  switch(tag_id) {
    case FLV_TAG_SCRIPT_DATA: {
      AMFType* t = amf.parseType(buffer);
      t->print();
      delete t;
      t = NULL;

      t = amf.parseType(buffer);
      t->print();
      b32 = buffer.getBigEndianU24();
      printf("END? : %d\n", b32);
      break;
    }
    case FLV_TAG_VIDEO: {
      printf("Read video tag..\n");
      break;
    }
    default: printf("Unhandled tag type (%d)\n", tag_id); break;
  };
  printf("++++++++\n");
  return -1;
}


// W R I T I N G
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int FLV::writeHeader(bool hasVideo, bool hasAudio) {
  buffer.putU8('F'); 
  buffer.putU8('L'); 
  buffer.putU8('V');
  buffer.putU8(0x01); // version

  rx_uint8 flags = 0;
  if(hasVideo) {
    flags |= 0x01; 
  }
  if(hasAudio) {
    flags |= 0x04; 
  }
  buffer.putU8(flags); // audio + video flags
  buffer.putBigEndianU32(9); // data offset
  buffer.putBigEndianU32(0); // previous tag size 0

  return buffer.size();
}

int FLV::writeParams(VideoParams* params) {
  // copy some specs
  x264_param_t* p = params->x264_param;
  vfr_input = p->b_vfr_input;
  fps_num = p->i_fps_num;
  fps_den = p->i_fps_den;
  timebase = (double)p->i_timebase_num / p->i_timebase_den;
  delay_frames = p->i_bframe ? (p->i_bframe_pyramid ? 2 : 1) : 0;
  /*
  printf("> VFR input: %d\n", vfr_input);
  printf("> FPS num: %lld\n", fps_num);
  printf("> FPS den: %lld\n", fps_den);
  printf("> Timebase: %f\n", timebase);
  printf("> Delay frames: %d\n", delay_frames);
  */	
  int now = buffer.size();

  // tag info
  buffer.putU8(FLV_TAG_SCRIPT_DATA);
  int size_pos = buffer.size();
  buffer.putBigEndianU24(0); // data length
  buffer.putBigEndianU24(0); // timestamp
  buffer.putBigEndianU32(0); // stream id

  // meta data
  int start_data = buffer.size();
  amf.putStringAMF0(buffer, AMFString("onMetaData", true));

  buffer.putU8(AMF0_TYPE_ECMA_ARRAY);
  buffer.putBigEndianU32(7); // we have 7 elements in the buffer
  amf.putStringAMF0(buffer, AMFString("width", false));
  amf.putNumberAMF0(buffer, AMFNumber(params->video_width));
  amf.putStringAMF0(buffer, AMFString("height", false));
  amf.putNumberAMF0(buffer, AMFNumber(params->video_height));

  amf.putStringAMF0(buffer, AMFString("framerate", false));
  if(p->b_vfr_input) {
    printf("VFR input.\n");
  }
  else {
    printf("NO VFR input.\n");
    printf("FPS: %f\n", (double)p->i_fps_num / p->i_fps_den);
    amf.putNumberAMF0(buffer, AMFNumber((double)p->i_fps_num / p->i_fps_den));
  }

  amf.putStringAMF0(buffer, AMFString("videocodecid", false));
  amf.putNumberAMF0(buffer, AMFNumber(params->video_codec_id));

  amf.putStringAMF0(buffer, AMFString("duration", false));
  pos_duration = buffer.size();
  amf.putNumberAMF0(buffer, AMFNumber(0));

  amf.putStringAMF0(buffer, AMFString("encoder", false));
  amf.putStringAMF0(buffer, AMFString("Lavf54.12.0", true)); 

  amf.putStringAMF0(buffer, AMFString("streamName", false));
  amf.putStringAMF0(buffer, AMFString("test", true)); 


  amf.putStringAMF0(buffer, AMFString("filesize", false));
  pos_file_size = buffer.size();
  amf.putNumberAMF0(buffer, AMFNumber(0));

  amf.putStringAMF0(buffer, AMFString("videodatarate", false));
  pos_datarate = buffer.size();
  amf.putNumberAMF0(buffer, AMFNumber(0));

  amf.putStringAMF0(buffer, AMFString("", false));
  buffer.putU8(AMF0_TYPE_OBJECT_END);

  // rewrite size
  int end_data = buffer.size();	
  rx_uint32 data_size = end_data - start_data;
  buffer.putBigEndianU24(data_size, size_pos);

  // previous tag size
  rx_uint32 tag_size = buffer.size() - now;
  buffer.putBigEndianU32(tag_size);
  return tag_size;
}

int FLV::writeHeaders(VideoParams* p) {
  x264_nal_t* nal = p->x264_nal;
  int sps_size = nal[0].i_payload;
  int pps_size = nal[1].i_payload;
  int sei_size = nal[2].i_payload;
  printf("> SPS size: %d\n", sps_size);
  printf("> PPS size: %d\n", pps_size);
  printf("> SEI size: %d\n", sei_size);

  // copy sei. write until before first frame (see x264 flv.c)
  sei = new rx_uint8[sei_size];
  if(!sei) {
    return -1;
  }
  sei_len = sei_size;
  memcpy(sei, nal[2].p_payload, sei_size);


  // SPS
  rx_uint8* sps = nal[0].p_payload + 4;

  // tag info
  // =========
  int now = buffer.size();
  buffer.putU8(FLV_TAG_VIDEO);
  int size_pos = buffer.size(); 
  buffer.putBigEndianU24(0); // data size (rewrite later)
  buffer.putBigEndianU24(0); // timestamp
  buffer.putU8(0); // timestamp extended
  buffer.putBigEndianU24(0); // stream id, always 0
	
  // video data
  // ==========
  int start_data = buffer.size();
  buffer.putU8(((1 << 4 | 7))); // keyframe AVC
  buffer.putU8(0); // AVC sequence header
  buffer.putBigEndianU24(0); // composition time
	
  // AVCDecoderConfigurationRecord
  // ==============================
  buffer.putU8(1); // configurationVersion
  buffer.putU8(sps[1]); // AVCProfileIndication
  buffer.putU8(sps[2]); // profile_compatibility
  buffer.putU8(sps[3]); // AVCLevelIndication
  buffer.putU8(0xff); // lengthSizeMinusOne
  buffer.putU8(0xe1); // numOfSequenceParameterSets
  printf("> Profile: %02X (%d), Profile(2): %02X (%d), Level: %02X (%d)\n", sps[1], sps[1], sps[2], sps[2], sps[3], sps[3]);

  buffer.putBigEndianU16(sps_size - 4); // sequenceParameterSetLength
  buffer.putBytes(sps, sps_size - 4); // sequenceParameterSetNalUnit

  // PPS (PictureParameterSets)
  buffer.putU8(1); // numOfPictureParameterSets
  buffer.putBigEndianU16(pps_size - 4); // pictureParameterSetLength
  buffer.putBytes(nal[1].p_payload + 4, pps_size - 4); // pictureParameterSetNalUnit

  // rewrite size 
  int data_size = buffer.size() - start_data;
  buffer.putBigEndianU24(data_size, size_pos);
  printf("> AVCDecoderConfigurationRecord: data size: %d\n", data_size);
	
  // previous tag size
  rx_uint32 tag_size = buffer.size() - now;
  buffer.putBigEndianU32(tag_size);
  printf("> Tag size: %d\n", tag_size);

  return tag_size;
}


#define convert_timebase_ms(timestamp, timebase) (rx_int64)((timestamp) * (timebase) * 1000 + 0.5)

int FLV::writeVideoPacket(VideoPacket* pkt) {
  int now = buffer.size();

  // get the delay time for the first frame.
  if(framenum == 0) {
    delay_time = pkt->dts * -1;
    if(!dts_compress && delay_time) {
      printf("> Frame: initial delay: %lld ms\n", convert_timebase_ms(pkt->pts + delay_time, timebase));
    }
    printf("> Frame: delay time: %lld\n", delay_time);
  }

  rx_int64 dts;
  rx_int64 cts;
  rx_int64 offset;

  if(dts_compress) {
    printf("> Frame: ERROR we do not support compressed dts.\n");
  }
  else {
    dts = convert_timebase_ms(pkt->dts + delay_time, timebase);
    cts = convert_timebase_ms(pkt->pts + delay_time, timebase); 
  }
  offset = cts - dts;

  // for all but first frame
  if(framenum > 0) {
    if(prev_dts == dts) {
      printf("> Frame: WARNING: duplicate dts: %lld\n", dts);
    }
    if(prev_cts == cts) {
      printf("> Frame: WARNING: duplicate cts: %lld\n", cts);
    }
  }

  prev_dts = dts;
  prev_cts = cts;

  // tag info
  //==========
  buffer.putU8(FLV_TAG_VIDEO);
  int size_pos = buffer.size(); 
  buffer.putBigEndianU24(0); // data size (rewrite later)
  buffer.putBigEndianU24(pkt->time_dts); // timestamp; based on system time
  buffer.putU8(pkt->time_dts >> 24);  // timestamp extended; based on system time
  //buffer.putBigEndianU24(dts);  // timestamp, based on x264_picture_t.i_dts
  //buffer.putU8(dts >> 24); // timestamp extended
  buffer.putBigEndianU24(0); // stream id
  //printf("> Frame: dts >> 24: %lld\n", ((dts >> 24)));
  //printf("> Frame: dts: %lld, cts: %lld, offset: %lld\n", dts, cts, offset);


  // video data / avcvideopacket
  // ===========================
  int start_data = buffer.size();
  rx_uint8 frame_type = (pkt->is_keyframe) ? (1 << 4 | 7) : (2 << 4 | 7);
  buffer.putU8(frame_type); // frame type + codec id
  buffer.putU8(1); // AVC NALU
  buffer.putBigEndianU24(offset); // composition time offset (when we have AVC NALU)

  if(sei) { // only once after we got the first frame
    //printf("> Frame, put SEI (%d bytes)\n", sei_len);
    buffer.putBytes(sei, sei_len);
    delete[] sei;
    sei = NULL;
  }
	
  // the nal units (avcvideopacket)
  buffer.putBytes(pkt->data, pkt->data_size);

  // rewrite data size
  int data_size = buffer.size() - start_data;
  buffer.putBigEndianU24(data_size, size_pos);
  //printf("> Frame data size: %d\n", data_size);

  // previous tag size:
  rx_uint32 tag_size = buffer.size() - now;
  buffer.putBigEndianU32(tag_size);
  //printf("> Frame tag size: %d\n", tag_size);
  //printf("--\n");

  framenum++;
  return tag_size;
}


// We only support speex now.
int FLV::writeAudioPacket(AudioPacket* p) {
  printf("> Frame: audio.\n");

  // tag info
  // ========
  int now = buffer.size();
  buffer.putU8(FLV_TAG_AUDIO);
  int size_pos = buffer.size();
  buffer.putBigEndianU24(0); // data size (rewrite later)
  //buffer.putBigEndianU24(p->dts); // timestamp 
  buffer.putBigEndianU24(p->time_dts); // time_dts is a timestamp which is calculatd based on system time
  buffer.putU8(0); // timestamp extended
  buffer.putBigEndianU24(0); // stream id, always 0

  // audio tag header
  // ================
  int start_data = buffer.size();
  int format = (FLV_SOUNDFORMAT_SPEEX << 4) | 1 << 1 | 0x00; // speex: format = 11, soundrate = 0, soundsize = 1, soundtype = 0
  buffer.putU8(format);
  buffer.putBytes(p->data, p->data_size);

  // rewrite data size 
  int data_size = buffer.size() - start_data;
  buffer.putBigEndianU24(data_size, size_pos);

  // previous tag size
  rx_uint32 tag_size = buffer.size() - now;
  buffer.putBigEndianU32(tag_size);
  printf("> Frame audio was %d bytes.\n--\n", tag_size);
  return -1;
}

// D E B U G 
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FLV::printType(rx_uint8 t) {
  switch(t) {
    case FLV_TAG_SCRIPT_DATA: printf("TagType: Script Data\n"); break;
    case FLV_TAG_AUDIO: printf("TagType: Audio\n"); break;
    case FLV_TAG_VIDEO: printf("TagType: Video\n"); break;
    default: printf("TagType: Unknown\n");
  };
}
