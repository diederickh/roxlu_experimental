#ifndef ROXLU_PLATFORM_WINDOWSH
#define ROXLU_PLATFORM_WINDOWSH


//#define WINVER 0x0502
//#define _WIN32_WINNT 0x0502

#include <time.h>
#include <windows.h>

#if (_MSC_VER < 1300)
   typedef signed char       int8_t;
   typedef signed short      int16_t;
   typedef signed int        int32_t;
   typedef unsigned char     uint8_t;
   typedef unsigned short    uint16_t;
   typedef unsigned int      uint32_t;
#else
   typedef signed __int8     int8_t;
   typedef signed __int16    int16_t;
   typedef signed __int32    int32_t;
   typedef unsigned __int8   uint8_t;
   typedef unsigned __int16  uint16_t;
   typedef unsigned __int32  uint32_t;
#endif

typedef signed __int64       int64_t;
typedef unsigned __int64     uint64_t;

#define atoll atol
	
#endif
