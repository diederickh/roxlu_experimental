#ifndef ROXLU_PLATFORM_OSXH
#define ROXLU_PLATFORM_OSXH

// See libebml, libbml_t.h for platform specific types

#include <sys/time.h>
#include <stdint.h>
#include <unistd.h> /* for getpid() in random.h */

typedef int8_t rx_int8;
typedef int16_t rx_int16;
typedef int32_t rx_int32;
typedef int64_t rx_int64;

typedef uint8_t rx_uint8;
typedef uint16_t rx_uint16;
typedef uint32_t rx_uint32;
typedef uint64_t rx_uint64;

#endif
