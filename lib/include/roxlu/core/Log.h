#ifndef ROXLU_LOG_H
#define ROXLU_LOG_H

#include <iostream>

#if defined(_WIN32) 
#  include <stdarg.h>
#endif

#define RX_LOG_NO_COLOR
#define RX_LOG_LEVEL_ALL 4
#define RX_LOG_LEVEL_ERROR  1
#define RX_LOG_LEVEL_WARNING 2
#define RX_LOG_LEVEL_VERBOSE 3

#define RX_LOG_LEVEL_NONE 0 


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
#else                                                                             
#  define RX_VERBOSE(fmt, ...) { rx_verbose(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 
#endif
//#define RX_VERBOSE(fmt, ...) { rx_verbose(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 
//#define RX_VERBOSE(fmt, ...) { rx_verbose(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 

#if !defined(RX_LOG_LEVEL)
#  define RX_LOG_LEVEL RX_LOG_LEVEL_VERBOSE
#endif

#if defined(_MSC_VER) 
//#  define RX_VERBOSE(x) { printf("[verbose] [%s L%d] - ", __FUNCSIG__, __LINE__); printf x; printf("\n"); }
#  define RX_WARNING(x) { printf("[warning] [%s L%d] - ", __FUNCSIG__, __LINE__); printf x; printf("\n"); }
#  define RX_ERROR(x) { printf("[error] [%s L%d] - ", __FUNCSIG__, __LINE__); printf x; printf("\n"); }
#else 
#  if RX_LOG_LEVEL >= RX_LOG_LEVEL_VERBOSE 
#    if defined(RX_LOG_NO_COLOR)
//#      define RX_VERBOSE(x) {  printf("[verbose] [%s L%d] - ", __PRETTY_FUNCTION__, __LINE__); printf x; printf("\n"); }
#    else 
//#      define RX_VERBOSE(x) { printf("[verbose] \x1b[36m" "[%s L%d] - " "\x1b[33m", __PRETTY_FUNCTION__, __LINE__); printf x; printf("\x1b[0m\n"); }
#    endif
#  else
//#    define RX_VERBOSE(x) {}
#  endif

#  if RX_LOG_LEVEL >= RX_LOG_LEVEL_WARNING
#    if defined(RX_LOG_NO_COLOR)
#      define RX_WARNING(x) { printf("[%s L%d] - ", __PRETTY_FUNCTION__, __LINE__); printf x; printf("\n"); }
#    else 
#      define RX_WARNING(x) { printf("\x1b[35m[warning] \x1b[36m" "[%s L%d] - " "\x1b[33m", __PRETTY_FUNCTION__, __LINE__); printf x; printf("\x1b[0m\n"); }
#    endif
#  else 
#    define RX_WARNING(x) { }
#  endif

#  if RX_LOG_LEVEL >= RX_LOG_LEVEL_ERROR
#    if defined(RX_LOG_NO_COLOR)
#      define RX_ERROR(x) { printf("[%s L%d] - ", __PRETTY_FUNCTION__, __LINE__); printf x; printf("\n"); }
#    else
#      define RX_ERROR(x) { printf("\x1b[31m[error] \x1b[36m" "[%s L%d] - " "\x1b[31m", __PRETTY_FUNCTION__, __LINE__); printf x; printf("\x1b[0m\n"); }
#    endif
#  else 
#    define RX_ERROR(x) {}
#  endif
#endif // MSVC

#endif // ROXLU_LOG_H

