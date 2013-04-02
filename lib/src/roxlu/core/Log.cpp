#include <roxlu/core/Log.h>

void rx_log_set_level(int level) {
  roxlu_log_level = level;
}

void rx_log_default_callback(int level, 
                             void* user, 
                             int line, 
                             const char* function, 
                             const char* fmt, 
                             va_list args) 
{

  if(level > roxlu_log_level) {
    return;
  }

  if(level == RX_LOG_LEVEL_VERBOSE) {
    printf("[verbose] [%s L%d] - ", function, line);
    vprintf(fmt, args);
  }
  else if(level == RX_LOG_LEVEL_WARNING) {
    printf("[warning] [%s L%d] - ", function, line);
    vprintf(fmt, args);
  }
  else if(level == RX_LOG_LEVEL_ERROR) {
    printf("[error] [%s L%d] - ", function, line);
    vprintf(fmt, args);
  }
  printf("\n");
}

void rx_verbose(int line, const char* function, const char* fmt, ...) {
  if(!roxlu_log_cb) {
    return;
  }

  va_list args;
  va_start(args, fmt);
  roxlu_log_cb(RX_LOG_LEVEL_VERBOSE, roxlu_log_user, line, function, fmt, args);
  va_end(args);
}

void rx_warning(int line, const char* function, const char* fmt, ...) {
  if(!roxlu_log_cb) {
    return;
  }

  va_list args;
  va_start(args, fmt);
  roxlu_log_cb(RX_LOG_LEVEL_WARNING, roxlu_log_user, line, function, fmt, args);
  va_end(args);
}

void rx_error(int line, const char* function, const char* fmt, ...) {
  if(!roxlu_log_cb) {
    return;
  }

  va_list args;
  va_start(args, fmt);
  roxlu_log_cb(RX_LOG_LEVEL_ERROR, roxlu_log_user, line, function, fmt, args);
  va_end(args);
}


void rx_log_set_callback(roxlu_log_callback cb, void* user) {
  roxlu_log_cb = cb;
  roxlu_log_user = user;
}

roxlu_log_callback  roxlu_log_cb = rx_log_default_callback;
void* roxlu_log_user = NULL;
int roxlu_log_level = RX_LOG_LEVEL_VERBOSE;
