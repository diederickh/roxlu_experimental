#ifndef ROXLU_BUTTONS_CLIENTH
#define ROXLU_BUTTONS_CLIENTH

#include <buttons/Buttons.h>
#include <buttons/Types.h>
#include <buttons/Element.h>
#include <buttons/Server.h>

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <roxlu/Roxlu.h> 

namespace buttons {

  void buttons_client_on_resolved(uv_getaddrinfo_t* req, int status, struct addrinfo* res);
  void buttons_client_on_connect(uv_connect_t* req, int status);
  void buttons_client_on_read(uv_stream_t* handle, ssize_t read, uv_buf_t buf);
  uv_buf_t buttons_client_on_alloc(uv_handle_t* handle, size_t nbytes);
  void buttons_client_on_write(uv_write_t* req, int status);
  void buttons_client_on_shutdown(uv_shutdown_t* req, int status);
  void buttons_client_on_close(uv_handle_t* handle);
  void buttons_client_on_reconnect_timer(uv_timer_t* handle, int status);

  class Client : public ButtonsListener {
  public:
    Client(std::string host, std::string port);                                   /* Client which connects to a server on the given port */
    ~Client();                                                                
    bool connect();                                                               /* Conect with gui sever */
    void reconnect();                                                             /* (private) Restarts the reconnect sequence when we get disconnected or can't connect to the server. Do not call this manually; is handled internally but we need it to be public for our callbacks */
    void update();                                                                /* Call this repeatetly */
    void draw();                                                                  /* Draw the retrieved guis */
    void onMouseMoved(int x, int y);                                          
    void onMouseUp(int x, int y);                                             
    void onMouseDown(int x, int y);                                           
                                                                              
    void sendCommand(CommandData cmd);                                            /* Send a command to the gui server */ 
    void write(char* data, size_t nbytes);                                        /* Send data over socket */
    void parseBuffer();                                                           /* Parses the incoming bitstream */ 
    void handleCommand(CommandData& cmd);                                         /* Handles the commands we find in parseBuffer() */
    void getScheme();                                                             /* Ask the remove server for the gui scheme */
    void operator()(unsigned int dx);                                             /* operator for button clicks */
    void clear();                                                                 /* (private) Deallocates and destroys all created guis + elements */


  public:
    void onEvent(ButtonsEventType event, 
                 const Buttons& buttons, 
                 const Element* target, 
                 void* targetData);

  private:
    void parseScheme(CommandData& cmd);                                          /* Decodes a serialized scheme and creates the necessary gui elements on the client */

                                                                             
  public:        
    ButtonsBuffer buffer;                                                        /* Stores incoming data from the server. These are commands + schemes */
    ClientServerUtils util;                                                      /* Used to parse the incoming commands */
    uv_loop_t* loop;                                                             /* The UV loop handle, which "runs" everything */
    uv_tcp_t sock;                                                               /* The uv socket wrapper */
    uv_getaddrinfo_t resolver_req;                                               /* Used to resolve the DNS to get to the IP of the server */
    uv_connect_t connect_req;                                                    /* Connect request context */
    uv_shutdown_t shutdown_req;                                                  /* Used to shutdown the connection with the server (when server disconnects us) */
    uv_timer_t timer_req;                                                        /* When we can't connect or get disconnected we start the reconnect sequence using this timer */
    std::string host;                                                            /* Host on which the gui server runs */
    std::string port;                                                            /* Port we connect to on the gui sever */
    std::map<unsigned int, std::map<unsigned int, buttons::Element*> > elements; /* Indexed by buttons-id with all elements */
    std::map<unsigned int, buttons::Buttons*> buttons;                           /* The buttons/gui objects, indexed by ID */

    /* these represent the 'properties' on the server */
    std::vector<float*> value_floats;
    std::vector<int*> value_ints;
    std::vector<bool*> value_bools;
    std::vector<float*> value_float_arrays;
    std::vector<int*> value_int_arrays;
  };

} // buttons
#endif
