#ifndef ROXLU_OSC_RECEIVER
#define ROXLU_OSC_RECEIVER

extern "C" {
#  include <lo/lo.h>
}

#include <string>
#include <sys/types.h>
#include <unistd.h>


#define OSC_ERR_LOOP "Cannot create the uv_loop we use for threading"
#define OSC_ERR_UPDATE "Cannot update the osc server as it's not started yet. Did you call start()?"
#define OSC_ERR_SELECT "Select error"
#define OSC_ERR_NOT_SETUP "No lo-server found. The setup probably went wrong"
#define OSC_SERVER_UNHANDLED "Unhandled path: %s"
#define OSC_SERVER_ERROR "OSC Error: %s - %s (%d)"

namespace osc {
  
  int lo_generic_handler(const char *path, const char *types, lo_arg **argv,
                         int argc, void *data, void *user_data);

  void lo_error_handler(int num, const char* m, const char* path);


  class Receiver {
  public:

    Receiver(std::string port);
    ~Receiver();

    bool addMethod(std::string path, std::string typespec, lo_method_handler h, void* user = NULL);
    bool start();
    void update();

  public:
    std::string port;
    lo_server server;

    struct timeval tv;
    fd_set rfds;
    int lo_fd;
    int lo_proto;
    int retval;
  };
};
#endif
