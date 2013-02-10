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
  class Client : public ButtonsListener {
  public:
    Client(std::string host, std::string port);                                   /* Client which connects to a server on the given port */
    ~Client();                                                                
    bool connect();                                                               /* Conect with gui sever */
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

  public:
    void onEvent(ButtonsEventType event, 
                 const Buttons& buttons, 
                 const Element* target, 
                 void* targetData);

  private:
    void parseScheme(CommandData& cmd);                                          /* Decodes a serialized scheme and creates the necessary gui elements on the client */
    void clear();                                                                /* Deallocates and destroys all created guis + elements */
                                                                             
  public:                                                                    
    ButtonsBuffer buffer;                                                        /* Stores incoming data from the server. These are commands + schemes */
    ClientServerUtils util;                                                      /* Used to parse the incoming commands */
    uv_loop_t* loop;                                                             /* The UV loop handle, which "runs" everything */
    uv_tcp_t sock;                                                               /* The uv socket wrapper */
    uv_getaddrinfo_t resolver_req;                                               /* Used to resolve the DNS to get to the IP of the server */
    uv_connect_t connect_req;                                                    /* Connect request context */
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
