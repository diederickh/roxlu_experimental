#include <osc/Receiver.h>
#include <roxlu/core/Log.h>

namespace osc {

  // ---------------------------------------------------------------

  void lo_error_handler(int num, const char* m, const char* path) {
    RX_ERROR(OSC_SERVER_ERROR, num, m, path);
  }

  int lo_generic_handler(const char *path, 
                         const char *types, 
                         lo_arg **argv,
                         int argc, 
                         void *data, 
                         void *user) 
  {

    RX_VERBOSE(OSC_SERVER_UNHANDLED, path);
    return 0;
  }

  // ---------------------------------------------------------------

  Receiver::Receiver(std::string port) 
    :port(port)
    ,server(NULL)
  {

    server = lo_server_new(port.c_str(), lo_error_handler);

    if(!server) {
      RX_ERROR(OSC_ERR_NOT_SETUP);
    }

  }

  Receiver::~Receiver() {

    if(server) {
      lo_server_free(server);
      server = NULL;
    }
  }

  bool Receiver::start() {

    lo_server_add_method(server, NULL, NULL, lo_generic_handler, NULL);

    lo_fd = lo_server_get_socket_fd(server);
    
    return true;
  }

  bool Receiver::addMethod(std::string path, std::string typespec, lo_method_handler h, void* user) {

    if(!server) {
      RX_ERROR(OSC_ERR_NOT_SETUP);
      return false;
    }

    lo_server_add_method(server, path.c_str(), typespec.c_str(), h, user);

    return true;
  };

  void Receiver::update() {

    if(!server) {
      RX_ERROR(OSC_ERR_UPDATE);
      return;
    }


    FD_ZERO(&rfds);
    FD_SET(lo_fd, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 10000;

    retval = select(lo_fd + 1, &rfds, NULL, NULL, &tv);

    if(retval == -1) {
      RX_ERROR(OSC_ERR_SELECT);
      return;
    }
    else if(retval > 0) {

      if(FD_ISSET(lo_fd, &rfds)) {
        lo_server_recv_noblock(server, 0);
      }
    }

  }

} // namespace osc
