#ifndef ROXLU_LOG_H
#define ROXLU_LOG_H

#include <iostream>

#if defined(_WIN32) 
#  include <stdarg.h>
#endif

#if defined(__linux) 
#  include <stdarg.h>
#  include <stdio.h>
#endif

#define RX_LOG_NO_COLOR
#define RX_LOG_LEVEL_ALL 4
#define RX_LOG_LEVEL_ERROR  1
#define RX_LOG_LEVEL_WARNING 2
#define RX_LOG_LEVEL_VERBOSE 3

#define RX_LOG_LEVEL_NONE 0 

extern "C" {

typedef void(*roxlu_log_callback)(int level, void* user, int line, const char* function, const char* fmt, va_list args);

extern roxlu_log_callback roxlu_log_cb;
extern void* roxlu_log_user;
extern int roxlu_log_level;

void rx_log_set_callback(roxlu_log_callback cb, void* user);
void rx_log_set_level(int level); 
void rx_log_default_callback(int level, void* user, int line, const char* function, const char* fmt, va_list args); 

void rx_verbose(int line, const char* function, const char* fmt, ...);
void rx_warning(int line, const char* function, const char* fmt, ...);
void rx_error(int line, const char* function, const char* fmt, ...);

#if defined(_MSC_VER)
#  define RX_VERBOSE(fmt, ...) { rx_verbose(__LINE__, __FUNCSIG__, fmt, ##__VA_ARGS__); } 
#  define RX_WARNING(fmt, ...) { rx_warning(__LINE__, __FUNCSIG__, fmt, ##__VA_ARGS__); } 
#  define RX_ERROR(fmt, ...) { rx_error(__LINE__, __FUNCSIG__, fmt, ##__VA_ARGS__); } 
#else                                                                             
#  define RX_VERBOSE(fmt, ...) { rx_verbose(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 
#  define RX_WARNING(fmt, ...) { rx_warning(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 
#  define RX_ERROR(fmt, ...) { rx_error(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 
#endif

#if !defined(RX_LOG_LEVEL)
#  define RX_LOG_LEVEL RX_LOG_LEVEL_VERBOSE
#endif

// unset log macros based on the -DRX_LOG_LEVEL preprocessor variable
#if RX_LOG_LEVEL == RX_LOG_LEVEL_VERBOSE
#elif RX_LOG_LEVEL ==  RX_LOG_LEVEL_WARNING
  #undef RX_VERBOSE
#elif RX_LOG_LEVEL == RX_LOG_LEVEL_ERROR
  #undef RX_VERBOSE
  #undef RX_WARNING
#endif

#if RX_LOG_LEVEL == RX_LOG_LEVEL_NONE
  #undef RX_ERROR
  #undef RX_VERBOSE
  #undef RX_WARNING
#endif

// if not set anymore, define empties
#if !defined(RX_VERBOSE)
  #define RX_VERBOSE(fmt, ...) {}
#endif

#if !defined(RX_WARNING)
  #define RX_WARNING(fmt, ...) {}
#endif

#if !defined(RX_ERROR)
  #define RX_ERROR(fmt, ...) {}
#endif
} // extern C
#endif // ROXLU_LOG_H

