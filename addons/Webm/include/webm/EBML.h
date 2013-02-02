/**
 * EBML writer for live-webm streams.
 *
 */
#ifndef ROXLU_EBML_H
#define ROXLU_EBML_H

extern "C" {
#   include <uv.h>
}
#include <stdlib.h> /* rand() */
#include <time.h> /* time() */
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <string>
#include <vector>
#include <algorithm>
#include <roxlu/core/Log.h>

/* Reference https://github.com/kinetiknz/nestegg/blob/master/src/nestegg.c */

/* General info */
#define EBML_DATA_SIZE_UNKNOWN 0x01FFFFFFFFFFFFFFLLU
#define EBML_TRACK_TYPE_VIDEO 1
#define EBML_TRACK_TYPE_AUDIO 2

/* EBML elements */
/* -------------------------------------------------------------------------*/
#define ID_EBML 0x1a45dfa3
#define ID_EBML_VERSION 0x4286
#define ID_EBML_READ_VERSION 0x42f7
#define ID_EBML_MAX_ID_LENGTH 0x42f2
#define ID_EBML_MAX_SIZE_LENGTH 0x42f3
#define ID_DOCTYPE 0x4282
#define ID_DOCTYPE_VERSION 0x4287
#define ID_DOCTYPE_READ_VERSION 0x4285

/* Global Elements */
#define ID_VOID 0xec
#define ID_CRC32 0xbf

/* WebM/Segment Elements */
#define ID_SEGMENT 0x18538067
#define ID_SEGMENT_UID 0x73a4
#define ID_TITLE 0x7ba9

/* Seek Head Elements */
#define ID_SEEK_HEAD 0x114d9b74
#define ID_SEEK 0x4dbb
#define ID_SEEK_ID 0x53ab
#define ID_SEEK_POSITION 0x53ac

/* Info Elements */
#define ID_INFO 0x1549a966
#define ID_TIMECODE_SCALE 0x2ad7b1
#define ID_DURATION 0x4489
#define ID_DATE_UTC 0x4461
#define ID_MUXING_APP 0x4d80
#define ID_WRITING_APP 0x5741

/* Tracks Elements */
#define ID_TRACKS 0x1654ae6b
#define ID_TRACK_ENTRY 0xae
#define ID_TRACK_NUMBER 0xd7
#define ID_TRACK_UID 0x73c5
#define ID_TRACK_TYPE 0x83
#define ID_TRACK_DEFAULT_DURATION 0x23e383
#define ID_TRACK_TIMECODE_SCALE 0x23314f

#define ID_CODEC_ID 0x86
#define ID_CODEC_NAME 0x258688
#define ID_CODEC_PRIVATE 0x63a2
#define ID_VIDEO 0xe0
#define ID_PIXEL_WIDTH 0xb0
#define ID_PIXEL_HEIGHT 0xba

/* Audio Elements */
#define ID_AUDIO 0xe1
#define ID_SAMPLING_FREQUENCY 0xb5
#define ID_CHANNELS 0x9f
#define ID_BIT_DEPTH 0x6264


/* Cues Elements */
#define ID_CUES 0x1c53bb6b
#define ID_CUE_POINT 0xbb
#define ID_CUE_TIME 0xb3
#define ID_CUE_TRACK_POSITIONS 0xb7
#define ID_CUE_TRACK 0xf7
#define ID_CUE_CLUSTER_POSITION 0xf1
#define ID_CUE_BLOCK_NUMBER 0x5378

/* Cluster Elements */
#define ID_CLUSTER 0x1f43b675
#define ID_TIMECODE 0xe7
#define ID_BLOCK_GROUP 0xa0
#define ID_SIMPLE_BLOCK 0xa3

// ---------------------------------------
#define EBML_TMP_BUFFER_SIZE 1024

enum EBMLDebugType {
  EBML_CHAR,
  EBML_INT,
  EBML_HEX,
  EBML_SKIP, /* skip some bytes */
  EBML_NONE
};

struct EBMLHeader {
  int ebml_version;
  int ebml_read_version;
  int ebml_max_id_length;
  int ebml_max_size_length;
  std::string doctype;
  int doctype_version;
  int doctype_read_version;
};

struct EBMLSegmentInfo {
  std::string title;                                                                /* title of this stream/file/video */
  std::string muxing_app;                                                           /* name of the muxing app, abritrary string */
  std::string writing_app;                                                          /* name of the writing application, can be an abitrary string */
  uint64_t timecode_scale;                                                          /* timecode in nanoseconds, used for the (simple blocks) 1000.000 means you're using millis seconds */
};

struct EBMLTrack {
  unsigned char type;                                                               /* track type, 1 = video, 2 = audio, 3 = complex, 0x10 = logo, 0x11 = subtitle, 0x12 = buttons, 0x20 = control */
  unsigned char number;                                                             /* track number, e.g. 1, 2, ... */
  uint64_t uid;                                                                     /* unique identifier for this track */
  std::string codec_id;                                                             /* codec id used for this track: http://matroska.org/technical/specs/codecid/index.html */

  /* video specs */
  int vid_pix_width;                                                                /* when type = 1, width of the video */
  int vid_pix_height;                                                               /* when type = 1, height of the video */

};

struct EBMLSimpleBlock {
  unsigned char track_number;                                                       /* the track number for which you want to append a simple block */
  uint64_t timestamp;                                                               /* absolute timestamp in millis since the start of the stream, we calculate the cluster-relative timecode based on this value */
  unsigned char flags;                                                              /* simple block flags, see spec */
  uint32_t nbytes;                                                                  /* number of bytes in this block */
  char* data;                                                                       /* pointer to the data for this simple block */
};

struct EBMLCluster {
  uint64_t timecode;                                                                /* the absolute timecode since the stream started in millis */
};

typedef size_t(*ebml_write_cb)(char* data, size_t nbytes, void* user);              /* output: write data */
typedef void(*ebml_close_cb)(void* user);                                           /* output: close  */
typedef size_t(*ebml_peek_cb)(char* dest, void* user);                              /* input: peek the next byte */
typedef size_t(*ebml_read_cb)(char* dest, size_t nbytes, void* user);               /* input: read nbyte sinto dest */
typedef size_t(*ebml_skip_cb)(size_t nbytes, void* user);                           /* input: skip nbytes from current pos */


size_t ebml_file_write(char* data, size_t nbytes, void* user);
void ebml_file_close(void* user);
size_t ebml_file_peek(char* dest, void* user);
size_t ebml_file_read(char*dest, size_t nbytes, void* user);
size_t ebml_file_skip(size_t nbytes, void* user);

struct EBMLFile {                                                                   /* EBML file stream, used as a helper to write/read ebml to/from a file */
  EBMLFile();
  ~EBMLFile();
  bool open(const std::string filepath);

private:
  FILE* fp;
};

class EBML {
 public:
  EBML();
  ~EBML();

  /* create ebml stream */
  int open(EBMLHeader header);                                                      /* 1) open the ebml stream, first to be called when creating a new stream  */
  int openSegment(EBMLSegmentInfo info);                                            /* 2) open the segment part, we open with an unknown data size because we focus this ebml encoder on live webm streaming */
  int openTracks(std::vector<EBMLTrack> tracks);                                    /* 3) open the tracks part with the track entries. */
  int openCluster(EBMLCluster cluster);                                             /* writes a new cluster element with an unknown size as we're focussing on streaming */
  int addSimpleBlock(EBMLSimpleBlock block);                                        /* after calling (1),(2) and (3), you can start adding simple blocks. we automatically create new clusters every X-millis */
  int close();

  /* buffer management */
  size_t flush(std::vector<char>* data); // flushes to output file 
  void clear(); // clears write buffers

  /* callbacks to read/write */
  void setCallbacks(
    ebml_write_cb writeCB,
    ebml_close_cb closeCB,
    ebml_peek_cb peekCB,
    ebml_read_cb readCB,
    ebml_skip_cb skipCB,
    void* user);

  /* parses a read buffer */
  void parse();

  /* writing (all to this->buffer) */
  void wu8(uint8_t b, std::vector<char>* dest);
  void wu16(uint16_t b, std::vector<char>* dest);
  void wu32(uint32_t b, std::vector<char>* dest);
  void write(char* data, size_t nbytes, std::vector<char>* dest);
  void writeID(uint64_t id, std::vector<char>* dest); 
  void writeDataSize(uint64_t size, std::vector<char>* dest);
  void writeDataSizeUnknown(std::vector<char>* dest);
  void writeReversed(char* data, size_t nbytes, std::vector<char>* dest);
  
  /* extended writing, writes: ID + datasize + data */
  void eu8(uint64_t id, uint8_t data, std::vector<char>* dest);                       /* write an unsigned int 8 element */
  void eu16(uint64_t id, uint16_t data, std::vector<char>* dest);                     /* write an unsigned int 16 element */
  void eu32(uint64_t id, uint32_t data, std::vector<char>* dest);                     /* write an unsigned int 32 element */
  void estr(uint64_t id, std::string str, std::vector<char>* dest);                   /* write an utf8 string element */
  void eunumber(uint64_t id, uint64_t data, std::vector<char>* dest);                 /* write an unsigned number using the minimum number of bytes */

  /* reading */
  uint8_t peek();
  uint64_t ru64();
  uint64_t ru56();
  uint64_t ru48();
  uint64_t ru40();
  uint32_t ru32();
  uint32_t ru24();
  uint16_t ru16();
  uint8_t ru8();
  int decodeSize(uint8_t b);                                                       /* returns how many number of bytes need to be read */
  int encodeSize(uint64_t b);                                                      /* based on value stored in 'b' return the number of bytes that you need to store */
  uint64_t readID();
  uint64_t readDataSize();
  size_t readData(char* dest, size_t nbytes);

  /* utils */
  uint64_t generateUID(int num);

 private:
  void printElement(uint64_t id, uint64_t dataSize, int dataType, char* buffer, size_t bufferSize = EBML_TMP_BUFFER_SIZE);
  std::string idToString(uint64_t id);

 private:
  void* cb_user;
  ebml_read_cb cb_read;
  ebml_skip_cb cb_skip;
  ebml_peek_cb cb_peek;
  ebml_write_cb cb_write;
  ebml_close_cb cb_close;

  std::vector<char> buffer;                                                       /* our main write buffer */
  std::vector<char> tmp_buffer;                                                   /* because we can't predict the size of some elements we use a intermediate buffer to store data */

  uint64_t time_cluster_started;                                                  /* timestamp in nanoseconds when the last cluster started. all simpleblocks base their relative time on this */
};

inline uint64_t EBML::ru64() {
  uint64_t result = 0;
  cb_read((char*)&result, 8, cb_user);
  //  0x00 00 00 00 00 00 00 FF
  //  0x00 00 00 00 00 00 FF 00
  //  0x00 00 00 00 00 FF 00 00
  //  0x00 00 00 00 FF 00 00 00
  //  0xFF 00 00 00 00 00 00 00
  //  0x00 FF 00 00 00 00 00 00
  //  0x00 00 FF 00 00 00 00 00
  //  0x00 00 00 FF 00 00 00 00
  result = ((result >> 56) & 0x00000000000000FF) |
    ((result >> 40) & 0x000000000000FF00) |
    ((result >> 24) & 0x0000000000FF0000) |
    ((result >> 8 ) & 0x00000000FF000000) |
    ((result << 56) & 0xFF00000000000000) |
    ((result << 40) & 0x00FF000000000000) |
    ((result << 24) & 0x0000FF0000000000) |
    ((result << 8 ) & 0x000000FF00000000);
  return result;
}

inline uint64_t EBML::ru56() {
  RX_WARNING(("FIX - validate"));
  uint64_t result = 0;
  return result;
}

inline uint64_t EBML::ru48() {
  RX_WARNING(("FIX - validate"));
  uint64_t result = 0;
  return result;
}

inline uint64_t EBML::ru40() {
  RX_WARNING(("FIX - validate"));
  uint64_t result = 0;
  /*
  cb_read((char*)&result, 5, cb_user);
  unsigned char* p = (unsigned char*)&result;
  printf("\n\nRU40:\n--------------------------\n");
  for(int i = 0; i < 5; ++i) {
    printf("%02X ", p[i]);
  }
  printf("\n------------------------------\n");
  */
  result = ((result >> 32) & 0x00000000000000FF);
  return result;
}


inline uint32_t EBML::ru32() {
  uint32_t result = 0;
  cb_read((char*)&result, 4, cb_user);
  result = ((result >> 24) & 0x000000FF) | ((result >> 8) & 0x0000FF00) | ((result << 8) & 0x00FF0000) | ((result << 24) & 0xFF000000);
  return result;
}

inline uint32_t EBML::ru24() {

  uint32_t result = 0;
  readData((char*)&result, 3);
  result = ((result >> 16) & 0x000000FF) | (result & 0x0000FF00) | ((result << 16) & 0x00FF0000); //| ((result << 16) & 0x00FF0000);

  /*
  unsigned char* p = (unsigned char*)&result;
  for(int i = 0; i < 3; ++i) {
    printf("%02X ", p[i]);
  }
  printf("\n");
  */

  RX_VERBOSE(("reading 3 bytes of size, which gives -> %d", result));
  return result;
}

inline uint16_t EBML::ru16() {
  uint16_t result = 0;
  cb_read((char*)&result, 2, cb_user);
  result = ((result >> 8) & 0x00FF) | ((result << 8) & 0xFF00);
  return result;
}

inline uint8_t EBML::ru8() {
  uint8_t result = 0;
  cb_read((char*)&result, 1, cb_user);
  return result;
}

inline uint8_t EBML::peek() {
  uint8_t result = 0;
  cb_peek((char*)&result, cb_user);
  return result;
}

// Returns how many bytes need to be 'read/written' 
inline int EBML::decodeSize(uint8_t b) {
  if((b & 0x80) == 0x80) {
    return 1;
  }
  else if((b & 0x40) == 0x40) {
    return 2;
  }
  else if((b & 0x20) == 0x20) {
    return 3;
  }
  else if((b & 0x10) == 0x10) {
    return 4;
  }
  else if((b & 0x08) == 0x08) {
    return 5;
  }
  else if((b & 0x04) == 0x04) {
    return 6;
  }
  else if((b & 0x02) == 0x02) {
    return 7;
  }
  else if((b & 0x01) == 0x01) {
    return 8;
  }
  else {
    RX_ERROR(("ERROR: canot decode size: %02X.", b));
    return 0;
  }
}

/* @todo we should use < and return directly instead of 'growing' */
inline int EBML::encodeSize(uint64_t number) {
  /*
  0x00 00 00 00 00 00 00 00 
  0x00 00 00 00 00 00 01 00 // 2
  0x00 00 00 00 00 01 00 00 // 3
  0x00 00 00 00 01 00 00 00 // 4
  0x00 00 00 01 00 00 00 00 // 5
  0x00 00 01 00 00 00 00 00 // 6
  0x00 01 00 00 00 00 00 00 // 7
  0x01 00 00 00 00 00 00 00 // 8
  */

  int num_bytes = 1;

  if(number >= 0x0000000000000100) { 
    num_bytes = 2;
  }
  if(number >= 0x0000000000010000) {
    num_bytes = 3;
  }
  if(number >= 0x0000000001000000) {
   num_bytes = 4;
  }
  if(number >= 0x0000000100000000) {
    num_bytes = 5;
  }
  if(number >= 0x0000010000000000) {
    num_bytes = 6;
  }
  if(number >= 0x0001000000000000) {
    num_bytes = 7;
  }
  if(number >=  0x0100000000000000) {
    num_bytes = 8;
  }
  return num_bytes;
}

inline uint64_t EBML::readID() {
  uint8_t size = peek();
  int num_bytes = decodeSize(size);

  switch(num_bytes) {
    case 1: { 
      return ru8();
    };
    case 2: { 
      return ru16();
    }
    case 3: { 
      return ru24();
    }
    case 4: { 
      return ru32();
    }
    case 5: { 
      RX_WARNING(("ID - TODO: Need to read 5 bytes."));
      return ru40();
    }
    case 6: { 
      RX_WARNING(("ID - TODO: Need to read 6 bytes."));
      break;
    }
    case 7:  { 
      RX_WARNING(("ID - TODO: Need to read 7 bytes."));

      break;
    }
    case 8: {
      RX_VERBOSE(("Reading u64"));
      return ru64();
    }
  };

  return 0;
}

inline uint64_t EBML::readDataSize() {
  uint8_t size = peek();
  int num_bytes = decodeSize(size);
  // printf("Decode size: %02X, num_bytes: %d \n", size, num_bytes);
  switch(num_bytes) {
    case 1: { 
      //printf("size u8\n");
      return (ru8() & 0x7F);
    };
    case 2: { 
      //printf("size u16\n");
      return (ru16() & 0x3FFF); // @todo need to test this
    }
    case 3: { 

      RX_WARNING(("TODO: Need to read 3 bytes, @todo validate."));
      return (ru24() & 0x1FFFFF); // @todo need to test this
    }
    case 4: { 
      return (ru32() & 0x0FFFFFFF); // @todo need to test this
    }
    case 5: { 
      RX_WARNING(("TODO: Need to read 5 bytes.")); // @todo need to test this
      return (ru40() & 0x0000007FFFFFFFFF);
    }
    case 6: {  
      RX_WARNING(("TODO: Need to read 6 bytes.")); // @todo need to test this
      break;
    }
    case 7:  { 
      RX_WARNING(("TODO: Need to read 7 bytes.")); // @todo need to test this
      break;
    }
    case 8: {
      uint64_t b = ru64();
      if(b == EBML_DATA_SIZE_UNKNOWN) {
        return b;
      }
     return (b & 0x00FFFFFFFFFFFFFF);
    }
  };
  return 0;
}

inline size_t EBML::readData(char* dest, size_t nbytes) {
  return cb_read(dest, nbytes, cb_user);
}

inline void EBML::writeID(uint64_t id, std::vector<char>* dest) {
  int num_bytes = encodeSize(id);
  if(id == ID_TRACK_ENTRY) {
    RX_VERBOSE(("ID TRACK ENTRY NUM BYTES: %d", num_bytes));
  }
  writeReversed((char*)&id, num_bytes, dest);
}

inline void EBML::writeDataSize(uint64_t size, std::vector<char>* dest) {
  if(size < 0x7F) {
    RX_VERBOSE(("Data size: 1 bytes: %lld.", size));
    size = 0x80 | size;
    write((char*)&size, 1, dest);
  }
  else if(size < 0x7FF) {
    size = 0x0000000000004000 | size;
    writeReversed((char*)&size, 2, dest);
    RX_ERROR(("@todo Data size: 2 bytes."));
  }
  else if(size < 0x7FFFFF) {
    size = 0x00000000003FFF | size;

    printf("--\n");
    unsigned char* p = (unsigned char*)&size;
    for(int i = 7; i >= 0; i--) {
      printf("%02X ", p[i]);
    }
    printf("--\n");
    writeReversed((char*)&size, 3, dest);
    RX_ERROR(("@todo Data size: 3 bytes. "));
  }
  else if(size < 0x7FFFFFFF) {
    RX_ERROR(("@todo Data size: 4 bytes."));
  }
  else if(size < 0x7FFFFFFFFF) {
    RX_ERROR(("@todo Data size: 5 bytes."));
  }
  else if(size < 0x7FFFFFFFFFFF) {
    RX_ERROR(("@todo Data size: 6 bytes."));
  }
  else if(size < 0x7FFFFFFFFFFFFF) {
    RX_ERROR(("@todo Data size: 7 bytes."));
  }
  else if(size < 0x7FFFFFFFFFFFFFFF) {
    RX_ERROR(("@todo Data size: 8 bytes."));
  }
  else {
    RX_ERROR(("WARNING: Data size, not handled"));
  }
}

inline void EBML::writeDataSizeUnknown(std::vector<char>* dest) {
  uint64_t src = EBML_DATA_SIZE_UNKNOWN;
  char* p = (char*)&src;
  for(int i = 7; i >= 0; --i) {
    dest->push_back(p[i]);
  }
}

inline void EBML::wu8(uint8_t b, std::vector<char>* dest) {
  write((char*)&b, 1, dest);
}

inline void EBML::wu16(uint16_t b, std::vector<char>* dest) {
  writeReversed((char*)&b, 2, dest);
}

inline void EBML::wu32(uint32_t b, std::vector<char>* dest) {
  writeReversed((char*)&b, 4, dest);
}

inline void EBML::write(char* data, size_t nbytes, std::vector<char>* dest) {
  std::copy(data, data+nbytes, std::back_inserter(*dest));
}

inline void EBML::writeReversed(char* data, size_t nbytes, std::vector<char>* dest) {
  for(int i = nbytes-1; i >= 0; i--) {
    //printf("%02X ", (unsigned char)data[i]);
    dest->push_back(data[i]);
  }
}

inline size_t EBML::flush(std::vector<char>* data) {
  size_t nbytes = data->size();
  cb_write((char*)&(*data)[0], nbytes, cb_user);
  data->clear();
  return nbytes;
}

inline void EBML::clear() {
  tmp_buffer.clear();
  buffer.clear();
}

inline void EBML::eu8(uint64_t id, uint8_t data, std::vector<char>* dest) {

  writeID(id, dest);
  RX_VERBOSE(("DATA TO WRITE: %hhd", data));
  writeDataSize(1, dest);
  wu8(data, dest);
}

inline void EBML::eu16(uint64_t id, uint16_t data, std::vector<char>* dest) {
  writeID(id, dest);
  writeDataSize(2, dest);
  wu16(data, dest);
}

inline void EBML::eu32(uint64_t id, uint32_t data, std::vector<char>* dest) {
  writeID(id, dest);
  writeDataSize(4, dest);
  wu32(data, dest);
}

inline void EBML::eunumber(uint64_t id, uint64_t data, std::vector<char>* dest) {
  int num_bytes = encodeSize(data);
  RX_VERBOSE(("enumber, num bytes: %d", num_bytes));

  writeID(id, dest);
  writeDataSize(num_bytes, dest);
  writeReversed((char*)&data, num_bytes, dest);
}

inline void EBML::estr(uint64_t id, std::string str, std::vector<char>* dest) {
  writeID(id, dest);
  writeDataSize(str.size(), dest);
  write((char*)str.c_str(), str.size(), dest);
}

inline int EBML::close() {
  cb_close(cb_user);
  return 1;
}

/* https://github.com/krad-radio/krad_radio/blob/devel/lib/krad_ebml/krad_ebml.c */
inline uint64_t EBML::generateUID(int num) {
  uint64_t t;
  uint64_t r;
  uint64_t uid = 0;

  srand( (unsigned)time( NULL ) );
  t = time(NULL) * num;
  r = rand();
  r = r << 32;
  r += rand();
  uid = t ^ r;
  return uid;
}
#endif
