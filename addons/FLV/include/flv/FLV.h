/**
 * Writing FLV files. 
 *
 * - Assuming little endian machine (@todo make endian independent)
 * - functions that return integer return 0 on error, 1> on success
 *
 * This class should be used like:
 * -------------------------------
 * FLV flv;
 * flv.setFlushCallback(some_cb, this); // gets called when there is enough data to be flushed
 *
 * FLVHeader header = { ... };
 * flv.writeHeader(header);
 * 
 * FLVParams params = { ...};
 * flv.writeParams(params);
 * 
 * // Then for each audio / video frame
 * flv.writeVideoPacket(...)
 * flv.writeAudioPacket(...) 
 * -------------------------------
 */

#ifndef ROXLU_AV_FLV_H
#define ROXLU_AV_FLV_H

#include <inttypes.h>

extern "C" { 
#include <x264.h>
}

#include <roxlu/Roxlu.h>
#include <algorithm>
#include <vector>
#include <string>
#include <assert.h>

#define AMF0_TYPE_NUMBER 0x00
#define AMF0_TYPE_BOOLEAN 0x01
#define AMF0_TYPE_STRING 0x02
#define AMF0_TYPE_OBJECT 0x03
#define AMF0_TYPE_MOVIECLIP 0x04 
#define AMF0_TYPE_NULL 0x05
#define AMF0_TYPE_UNDEFINED 0x06
#define AMF0_TYPE_REFERENCE 0x07
#define AMF0_TYPE_ECMA_ARRAY 0x08
#define AMF0_TYPE_OBJECT_END 0x09
#define AMF0_TYPE_STRICT_ARRAY 0x0A
#define AMF0_TYPE_DATE 0x0B
#define AMF0_TYPE_LONG_STRING 0x0C
#define AMF0_TYPE_UNSUPPORTED 0x0D
#define AMF0_TYPE_RECORDSET 0x0E
#define AMF0_TYPE_XML 0x0F
#define AMF0_TYPE_TYPED_OBJECT 0x10 

#define FLV_TAG_NONE 0x00
#define FLV_TAG_AUDIO 8
#define FLV_TAG_VIDEO 9
#define FLV_TAG_SCRIPT_DATA 18


enum FLVSoundCodec {
  FLV_SOUNDFORMAT_LINEAR_PCM_NE = 0 // native endan
  ,FLV_SOUNDFORMAT_ADPCM = 1
  ,FLV_SOUNDFORMAT_MP3 = 2
  ,FLV_SOUNDFORMAT_LINEAR_PCM_LE = 3 // little endian
  ,FLV_SOUNDFORMAT_NELLYMOSER_16KHZ = 4
  ,FLV_SOUNDFORMAT_NELLYMOSER_8KHZ = 5
  ,FLV_SOUNDFORMAT_G711_A_LAW = 7
  ,FLV_SOUNDFORMAT_G711_MU_LAW = 8
  ,FLV_SOUNDFORMAT_RESERVED = 9
  ,FLV_SOUNDFORMAT_AAC = 10
  ,FLV_SOUNDFORMAT_SPEEX = 11
  ,FLV_SOUNDFORMAT_MP3_8KHZ = 14
  ,FLV_SOUNDFORMAT_DEVICE_SPECIFIC = 15
};

enum FLVVideoCodec {
   FLV_VIDEOCODEC_SORENSON = 2
  ,FLV_VIDEOCODEC_SCREEN_VIDEO = 3
  ,FLV_VIDEOCODEC_ON2_VP6 = 4 // h263
  ,FLV_VIDEOCODEC_ON2_VP6_ALPHA = 5
  ,FLV_VIDEOCODEC_SCREEN_VIDEO2 = 6
  ,FLV_VIDEOCODEC_AVC = 7 // h264
};

enum FLVAudioSampleRate {
  FLV_SOUNDRATE_5_5KHZ = 0 
  ,FLV_SOUNDRATE_11KHZ = 1
  ,FLV_SOUNDRATE_22KHZ = 2
  ,FLV_SOUNDRATE_44KHZ = 3
};

enum FLVAudioBitDepth {
  FLV_SOUNDSIZE_8BIT = 0
  ,FLV_SOUNDSIZE_16BIT = 1
};

enum FLVSoundType {
   FLV_SOUNDTYPE_MONO = 0
  ,FLV_SOUNDTYPE_STEREO = 1
};

typedef size_t(*flv_write_data_cb)(char* data, size_t nbytes, void* user);
typedef void(*flv_rewrite_data_cb)(char* data, size_t nbytes, size_t pos, void* user);
typedef void(*flv_flush_data_cb)(void* user);
typedef void(*flv_close_cb)(void* user);


size_t flv_file_write(char* data, size_t nbytes, void* user);
void flv_file_rewrite(char* data,  size_t nbytes, size_t pos, void* user);
void flv_file_flush(void * user);
void flv_file_close(void* user);

struct FLVFileWriter {
  FLVFileWriter();
  ~FLVFileWriter();
  bool open(std::string filepath);
  FILE* fp;
};

struct FLVHeader {
  int has_audio; 
  int has_video;
  FLVSoundType sound_type;
  FLVAudioBitDepth sound_bits; 
  FLVAudioSampleRate sound_samplerate; 
  double sound_bitrate; // how many kbits e.g. 128000
}; 

// used to write the meta data
struct FLVParams {
  x264_param_t* x264_params;
  x264_nal_t* x264_nal;
};

struct FLVVideoPacket {
  rx_uint32 timestamp; // in millis, start at 0
  short int is_keyframe; // 1 = y, 0 = n
  rx_uint8* nals_data; // nal units data
  size_t nals_size; // bytes in nalus_data
};

struct FLVAudioPacket {
  rx_uint32 timestamp; // in millis
  rx_uint8* data;
  size_t size;
};

struct FLVAvcSequenceHeader {
  x264_nal_t* nals;
};

struct FLVCloseParams {
};

class FLV {
 public:
  FLV();
  ~FLV();
  
  /* flv */
  int open(FLVHeader header);
  int writeParams(FLVParams params);
  int writeAvcSequenceHeader(FLVAvcSequenceHeader hdr);
  int writeVideoPacket(FLVVideoPacket pkt);
  int writeAudioPacket(FLVAudioPacket pkt);
  int close(FLVCloseParams params);
  
  /* writing the bytestream */
  void put8(rx_uint8 v);
  void put16(rx_uint16 v);
  void put24(rx_uint32 v);
  void put32(rx_uint32 v);
  void put64(rx_uint64 v);
  void put(rx_uint8* data, size_t len);
  void putTag(std::string tag); // just write a string directly 
  void putAmfString(std::string str);
  void putAmfDouble(double v);
  void putAmfBool(bool flag);
  void rewrite24(rx_uint32 v, size_t dx); 
  rx_uint64 swap64(rx_uint64 v);
  rx_uint64 swapDouble(double v); // swap, but return a uint64

  /* reading the bytestream */
  rx_uint8 get8(); 
  rx_uint16 get16();
  rx_uint32 get24();
  rx_uint32 get32();
  rx_uint64 get64();
  double getAmfDouble();
  std::string getAmfString();
  bool hasDataToRead();

  /* utils */
  void setCallbacks(
    flv_write_data_cb writeCB, 
    flv_rewrite_data_cb rewriteCB, 
    flv_flush_data_cb flushCB, 
    flv_close_cb closeCB, 
    void* user
  );
  void flush();
  size_t size();
  char* ptr();

  /* debug */
  void dump(); // dumps the buffer with verbose info.
  void dumpAmf0(int type = -1);
  std::string flvTagToString(int tag);
  std::string flvAmf0TypeToString(int type); // converts an AMF type to string

 public:
  FLVHeader flv_header; // contains info about audio and video encoding

 private:
  /* callbacks */
  flv_write_data_cb cb_write; // gets called when there is some data to be written
  flv_flush_data_cb cb_flush; // gets called when all data needs to be flushed out, swooosh
  flv_rewrite_data_cb cb_rewrite; // gets called when some data needs to be rewritten (not always possible, aka sockets)
  flv_close_cb cb_close; // gets called when we're closing the bitstream.
  void* cb_user;

  /* buffer */
  size_t read_dx;
  std::vector<char> buffer;
  rx_uint8* sei_data;
  int sei_size;
  size_t bytes_written; // total accumulation of bytes written
  size_t bytes_flushed; // how many bytes have we actually flushed
  size_t framerate_pos; // position in buffer where the framerate is stored; used to rewrite later (if possible)
  size_t duration_pos; // position in buffer where the duration is stored; used to rewrite later (if possible)
  size_t filesize_pos; // position in buffer where filesize is stored; used to rewrite later (if possible)
  size_t datarate_pos; // position in buffere where the datarate is stored; is rewritten at the end.
  rx_uint64 last_video_timestamp; // used to rewrite the duration field
  rx_uint64 total_num_frames; // total num frames added, used to recalc. framerate
  bool is_opened; // check to make sure we don't open twice; and nicely cleanup
};

inline void FLV::setCallbacks(
  flv_write_data_cb writeCB, 
  flv_rewrite_data_cb rewriteCB, 
  flv_flush_data_cb flushCB, 
  flv_close_cb closeCB,
  void* user
) 
{
  cb_write = writeCB;
  cb_flush = flushCB;
  cb_rewrite = rewriteCB;
  cb_close = closeCB;
  cb_user = user;
}

inline void FLV::flush() {
  if(cb_write == NULL) {
    printf("VERBOSE: FLV: cannot write data, write callback not yet set.\n");
    return;
  }
  if(!is_opened) {
    printf("VERBOSE: FLV: cannot write data, not yet opened.\n");
    return;
  }
  bytes_flushed += cb_write(&buffer[0], buffer.size(), cb_user);
  buffer.clear();
}

inline size_t FLV::size() {
  return buffer.size();
}

inline void FLV::put8(rx_uint8 v) {
  buffer.push_back(v);
  bytes_written += 1;
}

inline void FLV::put16(rx_uint16 v) {
  buffer.push_back(v >> 8);
  buffer.push_back(v);
  bytes_written += 2;
}

inline void FLV::put24(rx_uint32 v) {
  put16(v >> 8);
  put8(v);
}

inline void FLV::rewrite24(rx_uint32 v, size_t dx) {
  buffer[dx + 0] = (v >> 16);
  buffer[dx + 1] = (v >> 8);
  buffer[dx + 2] = v;
}

inline void FLV::put32(rx_uint32 v) {
  buffer.push_back(v >> 24);
  buffer.push_back(v >> 16);
  buffer.push_back(v >> 8);
  buffer.push_back(v);
  bytes_written += 4;
}

inline void FLV::put64(rx_uint64 v) {
  put32(v >> 32);
  put32(v);
}

inline void FLV::put(rx_uint8* data, size_t len) {
  std::copy(data, data+len, std::back_inserter(buffer));
  bytes_written += len;
}

inline void FLV::putTag(std::string tag) {
  put((rx_uint8*)tag.c_str(), tag.size());
}

inline void FLV::putAmfString(std::string str) {
  rx_uint16 len = str.size();
  put16(len);
  put((rx_uint8*)str.c_str(), len);
}

inline void FLV::putAmfDouble(double v) {
  put8(AMF0_TYPE_NUMBER);
  rx_uint64 tmp = 0;
  memcpy((char*)&tmp, (char*)&v, 8);
  put64(tmp);
}

inline void FLV::putAmfBool(bool flag) {
  put8(AMF0_TYPE_BOOLEAN);
  put8(!!flag);
}

inline rx_uint8 FLV::get8() {
  assert(buffer.size() > 0);
  return buffer[read_dx++];
}

inline rx_uint16 FLV::get16() {
  rx_uint16 v;
  rx_uint8* p = (rx_uint8*)&v;
  p[0] = buffer[read_dx + 1];
  p[1] = buffer[read_dx + 0];
  read_dx += 2;
  return v;
}

inline rx_uint32 FLV::get24() {
  rx_uint32 v;
  rx_uint8* p = (rx_uint8*)&v;
  p[0] = buffer[read_dx + 2];
  p[1] = buffer[read_dx + 1];
  p[2] = buffer[read_dx + 0];
  p[3] = 0;
  read_dx += 3;
  return v;
}

inline rx_uint32 FLV::get32() {
  rx_uint32 v;
  rx_uint8* p = (rx_uint8*)&v;
  p[0] = buffer[read_dx + 3];
  p[1] = buffer[read_dx + 2];
  p[2] = buffer[read_dx + 1];
  p[3] = buffer[read_dx + 0];
  read_dx += 4;
  return v;
}

inline rx_uint64 FLV::swap64(rx_uint64 v) {
  rx_uint64 r; 
  char* dest = (char*)&r;
  char* src = (char*)&v;
  dest[0] = src[7];
  dest[1] = src[6];
  dest[2] = src[5];
  dest[3] = src[4];
  dest[4] = src[3];
  dest[5] = src[2];
  dest[6] = src[1];
  dest[7] = src[0];
  return r;
}

inline rx_uint64 FLV::swapDouble(double v) {
  rx_uint64 src = 0;
  memcpy((char*)&src, (char*)&v, sizeof(double));
  src = swap64(src);
  return src;
}

inline rx_uint64 FLV::get64() { 
  rx_uint64 v;
  rx_uint8* p = (rx_uint8*)&v;
  p[0] = buffer[read_dx + 7];
  p[1] = buffer[read_dx + 6];
  p[2] = buffer[read_dx + 5];
  p[3] = buffer[read_dx + 4];
  p[4] = buffer[read_dx + 3];
  p[5] = buffer[read_dx + 2];
  p[6] = buffer[read_dx + 1];
  p[7] = buffer[read_dx + 0];
  read_dx += 8;
  return v;
}

inline double FLV::getAmfDouble() {
  rx_uint64 d = get64();
  double r = 0;
  memcpy((char*)&r, (char*)&d, 8);
  return r;
}

inline std::string FLV::getAmfString() {
  std::string str;
  rx_uint16 len = get16();
  std::copy(ptr(), ptr() + len , std::back_inserter(str));
  read_dx += len;
  return str;
}

inline char* FLV::ptr() {
  return (char*)&buffer[read_dx];
}

inline bool FLV::hasDataToRead() {
  return read_dx < (size()-1);
}

#endif
