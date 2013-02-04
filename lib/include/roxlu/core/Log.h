#ifndef ROXLU_LOG_H
#define ROXLU_LOG_H

#define RX_LOG_LEVEL_ALL 4
#define RX_LOG_LEVEL_ERROR  1
#define RX_LOG_LEVEL_WARNING 2
#define RX_LOG_LEVEL_VERBOSE 3

#define RX_LOG_LEVEL_NONE 0 

#define RX_LOG_LEVEL RX_LOG_LEVEL_VERBOSE


#if defined(_MSC_VER)
#  define RX_VERBOSE(x) { printf("[verbose] [%s L%d] - ", __FUNCSIG__, __LINE__); printf x; printf("\n"); }
#  define RX_WARNING(x) { printf("[warning] [%s L%d] - ", __FUNCSIG__, __LINE__); printf x; printf("\n"); }
#  define RX_ERROR(x) { printf("[error] [%s L%d] - ", __FUNCSIG__, __LINE__); printf x; printf("\n"); }
#else 
#  if RX_LOG_LEVEL >= RX_LOG_LEVEL_VERBOSE 
#    define RX_VERBOSE(x) { printf("[verbose] \x1b[36m" "[%s L%d] - " "\x1b[33m", __PRETTY_FUNCTION__, __LINE__); printf x; printf("\x1b[0m\n"); }
#  else
#    define RX_VERBOSE(x) {}
#  endif

#  if RX_LOG_LEVEL >= RX_LOG_LEVEL_WARNING
#    define RX_WARNING(x) { printf("\x1b[35m[warning] \x1b[36m" "[%s L%d] - " "\x1b[33m", __PRETTY_FUNCTION__, __LINE__); printf x; printf("\x1b[0m\n"); }
#  else 
#    define RX_WARNING(x) { }
#  endif

#  if RX_LOG_LEVEL >= RX_LOG_LEVEL_ERROR
#    define RX_ERROR(x) { printf("\x1b[31m[error] \x1b[36m" "[%s L%d] - " "\x1b[31m", __PRETTY_FUNCTION__, __LINE__); printf x; printf("\x1b[0m\n"); }
#  else 
#    define RX_ERROR(x) {}
#  endif
#endif // MSVC

#endif // ROXLU_LOG_H

