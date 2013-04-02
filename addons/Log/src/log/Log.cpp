#include <log/Log.h>
#include <roxlu/core/Log.h>
#include <algorithm>
#include <sstream>

namespace roxlu {

  void log_addon_log_callback(int level,
                              void* user,
                              int line,
                              const char* function,
                              const char* fmt,
                              va_list args)
  {

    Log* l = static_cast<Log*>(user);
    std::stringstream ss_tty;
    std::stringstream ss_file;

    if(l->write_time) {
      std::string timing =  rx_strftime(l->date_format.c_str());
      ss_tty << timing;
      ss_file << timing;
    }

    if(l->write_log_level) {
      if(level == RX_LOG_LEVEL_VERBOSE) {

        if(l->use_colors) {
          ss_tty << "\x1b[36m";
        }

        ss_tty << "[verbose] ";
        ss_file << "[verbose] ";

      }
      else if(level == RX_LOG_LEVEL_WARNING) {
        if(l->use_colors) {
          ss_tty << "\x1b[36m";
        }

        ss_tty << "[warning] ";
        ss_file << "[warning] ";
      }
      else if(level == RX_LOG_LEVEL_ERROR) {
        if(l->use_colors) {
          ss_tty << "\x1b[36m";
        }
                
        ss_tty << "[error] ";
        ss_file << "[warning] ";
      }
      else {
        printf("Unsupported log level.\n");
        return;
      }
    }
    
    {
      if(l->use_colors) {
        ss_tty << "\x1b[33m";
      }

      if(l->write_function_name || l->write_line_number) {
        ss_tty << "[ ";
        ss_file << "[ ";
      }

      if(l->write_function_name) {
        ss_tty << function << " ";
        ss_file << function << " ";
      }

      if(l->write_line_number) {
        ss_tty << "L" << line << " ";
        ss_file << "L" << line << " ";
      }

      if(l->write_function_name || l->write_line_number) {
        ss_tty << "] ";
        ss_file << "] ";
      }

      if(l->use_colors) {

        if(level == RX_LOG_LEVEL_VERBOSE) {
          ss_tty << "\x1b[32m";
        }
        else if(level == RX_LOG_LEVEL_WARNING) {
          ss_tty << "\x1b[33m";
        }

      }

    }

    LogMessage msg;
    char buf[1024 * 2];

    vsprintf(buf, fmt, args);

    ss_tty << buf;
    ss_file << buf;

    if(l->use_colors) {
        ss_tty << "\x1b[0m";
    }

    ss_tty << "\n";
    ss_file << "\n";

    msg.tty_message = ss_tty.str();
    msg.file_message = ss_file.str();
    msg.level = level;

    l->addMessage(msg);
  }

  // -------------------------------------------

  Log::Log() 
    :write_function_name(false)
    ,write_line_number(false)
    ,use_colors(true)
    ,write_to_file(true)
    ,write_to_console(true)
    ,write_time(true)
    ,write_log_level(true)
    ,max_file_size(1024 * 1024)
  {
    date_format = "%F %T ";
    loop = uv_default_loop();
    uv_mutex_init(&mutex);
  }

  Log::~Log() {   
    if(ofs.is_open()) {
      ofs.close();
    }
    uv_tty_reset_mode();
    uv_mutex_destroy(&mutex);
    loop = NULL;
  }

  void Log::mini() {
    logFunctionName(false);
    logLineNumber(false);
    logTime(false);
    logLevel(false);
  }

  bool Log::setup(std::string name, std::string path) {
    if(write_to_file) {
      log_file = rx_to_data_path(path +name +".log");
      log_name = name;
      log_path = path;
      if(!createLogFile()) {
        return false;
      }
    }

    if(write_to_console) {
      uv_tty_init(loop, &tty, 1, 0);
      uv_tty_set_mode(&tty, 0);
    }

    rx_log_set_callback(log_addon_log_callback, this);

    return true;
  }

  bool Log::createLogFile() {

    ofs.open(log_file.c_str(), std::ios_base::app | std::ios_base::out);

    if(!ofs.is_open()) {
      printf("ERROR: cannot open the log file. \n");
      return false;
    }

    return true;
  }

  void Log::addMessage(LogMessage msg) {
    uv_buf_t buf;
    uv_write_t req;

    uv_mutex_lock(&mutex);
    {
      if(write_to_console) {
        buf.base = (char*)msg.tty_message.c_str();
        buf.len = msg.tty_message.size();
        uv_write(&req, (uv_stream_t*)&tty, &buf, 1, NULL);
      }

      if(write_to_file && ofs.is_open()) {
        ofs << msg.file_message;
      }

    }

    uv_mutex_unlock(&mutex);

    uv_run(loop, UV_RUN_NOWAIT);

    rotateLog();
  }
  

  void Log::rotateLog() {
    if(rx_get_file_size(log_file) > max_file_size) {

      if(ofs.is_open()) {
        ofs.close();
      }

      std::string rotate_name = rx_to_data_path(log_path +log_name +rx_strftime("_%Y_%m_%d_%H_%M_%S.log"));
      rx_rename_file(log_file, rotate_name);
      
      createLogFile(); 

    }
  }


} // roxlu
