// thanks to: http://www.netrino.com/Embedded-Systems/How-To/Big-Endian-Little-Endian
#ifndef ROXLU_ENDIANNESSH
#define ROXLU_ENDIANNESSH

//--------------
#define ROXLU_LITTLE_ENDIAN 

#include <roxlu/core/platform/Platform.h>

// apache license, thanks to: http://www.google.com/codesearch#wZuuyuB8jKQ/chromium/src/third_party/sfntly/src/sfntly/port/endian.h
static inline rx_uint16 EndianSwap16(rx_uint16 value) {
  return (rx_uint16)((value >> 8) | (value << 8));
}

static inline rx_uint32 EndianSwap32(rx_uint32 value) {
  return (((value & 0x000000ff) << 24) |
          ((value & 0x0000ff00) <<  8) |
          ((value & 0x00ff0000) >>  8) |
          ((value & 0xff000000) >> 24));
}

static inline rx_uint64 EndianSwap64(rx_uint64 value) {
  return (((value & 0x00000000000000ffLL) << 56) |
          ((value & 0x000000000000ff00LL) << 40) |
          ((value & 0x0000000000ff0000LL) << 24) |
          ((value & 0x00000000ff000000LL) << 8)  |
          ((value & 0x000000ff00000000LL) >> 8)  |
          ((value & 0x0000ff0000000000LL) >> 24) |
          ((value & 0x00ff000000000000LL) >> 40) |
          ((value & 0xff00000000000000LL) >> 56));
}

#ifdef ROXLU_LITTLE_ENDIAN
  #define ToBE16(n) EndianSwap16(n)
  #define ToBE32(n) EndianSwap32(n)
  #define ToBE64(n) EndianSwap64(n)
  #define ToLE16(n) (n)
  #define ToLE32(n) (n)
  #define ToLE64(n) (n)
  #define FromBE16(n) EndianSwap16(n)
  #define FromBE32(n) EndianSwap32(n)
  #define FromBE64(n) EndianSwap64(n)
  #define FromLE16(n) (n)
  #define FromLE32(n) (n)
  #define FromLE64(n) (n)
#else  // ROXLU_LITTLE_ENDIAN
  #define ToBE16(n) (n)
  #define ToBE32(n) (n)
  #define ToBE64(n) (n)
  #define ToLE16(n) EndianSwap16(n)
  #define ToLE32(n) EndianSwap32(n)
  #define ToLE64(n) EndianSwap64(n)
  #define FromBE16(n) (n)
  #define FromBE32(n) (n)
  #define FromBE64(n) (n)
  #define FromLE16(n) EndianSwap16(n)
  #define FromLE32(n) EndianSwap32(n)
  #define FromLE64(n) EndianSwap64(n)
#endif

	
#endif