#ifndef ROXLU_PLATFORM_LINUXH
#define ROXLU_PLATFORM_LINUXH

// See libebml, libbml_t.h for platform specific types
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>

typedef int8_t rx_int8;
typedef int16_t rx_int16;
typedef int32_t rx_int32;
typedef int64_t rx_int64;

typedef uint8_t rx_uint8;
typedef uint16_t rx_uint16;
typedef uint32_t rx_uint32;
typedef uint64_t rx_uint64;

#endif
