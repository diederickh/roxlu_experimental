#include <iostream>
#include <log/Log.h>

struct MyExampleData {
  std::string txt;
};

void my_custom_log_func(int level,                /* RX_LOG_LEVEL_VERBOSE, RX_LOG_LEVEL_WARNING, RX_LOG_LEVEL_ERROR */
                        void* user,               /* the pointer you passed to `rx_log_set_callback()` */
                        int line,                 /* the line of the caller */
                        const char* function,     /* the function of the caller */
                        const char* fmt,          /* the format string for printf() */
                        va_list args)             /* variadic argument list */
{
  MyExampleData* md = static_cast<MyExampleData*>(user);
  printf("%s", md->txt.c_str());
  vprintf(fmt, args);
  printf("\n");
}
                        

int main() {
  roxlu::Log log;
  rx_create_dir(rx_to_data_path("log"));
  log.setup("logger");
  
  // Log some different levels.
  // -------------------------------------
  RX_VERBOSE("A verbose message");
  RX_WARNING("A warning... app won't crash, but better look at it!");
  RX_ERROR("Ooops an error occured.. we can't continue");

  // Change the level we should show...
  // -------------------------------------
  rx_log_set_level(RX_LOG_LEVEL_ERROR);
  RX_VERBOSE("This won't be logged because the log level is error...");
  RX_WARNING("This won't be logged either because ERROR is the log level...");

  log.mini(); // <-- output minimal data

  rx_log_set_level(RX_LOG_LEVEL_WARNING);
  RX_WARNING("This warning is shown...");
  RX_ERROR("This error is shown too... ");
  RX_VERBOSE("But this isn't ...");

  log.maxi(); // <-- output maximum info

  rx_log_set_level(RX_LOG_LEVEL_VERBOSE);
  RX_VERBOSE("Pfew... show me!");
  RX_ERROR("And an error we want to show");
  RX_WARNING("Of course we want a warning message too");

  rx_log_set_level(RX_LOG_LEVEL_NONE);
  RX_VERBOSE("Nope.. we don'g log anything");
  RX_ERROR("Not even errors");
  RX_WARNING("Or errors..");

  // Set a custom log callback + custom data pointer 
  // ----------------------------------------------
  MyExampleData* my_data = new MyExampleData();
  my_data->txt = ">>> ";
  rx_log_set_callback(my_custom_log_func, my_data);

  RX_VERBOSE("Verbose for custom function");
  RX_WARNING("Warning for custom function");
  RX_ERROR("Error for custom function");
  return 0;
}
