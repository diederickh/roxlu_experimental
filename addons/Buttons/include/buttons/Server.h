#ifndef ROXLU_BUTTONS_SERVERH
#define ROXLU_BUTTONS_SERVERH

extern "C" {
#  include <uv.h>
}

#include <buttons/Types.h>
#include <buttons/Element.h>
#include <buttons/Slider.h>
#include <buttons/Toggle.h>
#include <buttons/Button.h>
#include <buttons/Radio.h>
#include <buttons/Pad.h>

#include <assert.h>
#include <string>
#include <vector>
#include <deque>
#include <roxlu/core/Log.h>

#define BUTTONS_UV_ERR(r, okval, msg, ret)      \
  if(r != okval) { \
    RX_ERROR(msg);  \
    RX_ERROR("uv error: %s", uv_strerror(uv_last_error(uv_default_loop()))) \
    return ret; \
  }

namespace buttons {

  void buttons_server_on_new_connection(uv_stream_t* sock, int status);
  void buttons_server_on_read(uv_stream_t* sock, ssize_t nread, uv_buf_t buf);
  void buttons_server_on_shutdown(uv_shutdown_t* req, int status);
  void buttons_server_on_close(uv_handle_t* handle);
  uv_buf_t buttons_server_on_alloc(uv_handle_t* handle, size_t nbytes);

  void buttons_connection_on_write(uv_write_t* req, int status); 

  enum CommandDataName {    // Structure which is used to hold deserialized data for gui types
     BDATA_SCHEME           // scheme: following data contains gui definitions
    ,BDATA_GET_SCHEME       // scheme: when a client connects it asks for the scheme by sending this
    ,BDATA_GUI              // scheme: flag there comes a gui definition
    ,BDATA_CHANGED          // value change on client or server
    ,BDATA_SLIDERF          // contains slider float
    ,BDATA_SLIDERI          // contains slider integer
    ,BDATA_TOGGLE           // contains toggle boolean value
    ,BDATA_BUTTON           // contains button data
    ,BDATA_RADIO            // contains the selected index of a radio
    ,BDATA_COLOR            // color data
    ,BDATA_VECTOR           // vector (float 2) data
    ,BDATA_PADF             // float pad value (2 floats, x/y percentage)
    ,BDATA_PADI             // int pad value (2 floats, x/y percentage)
  };


  struct CommandData {      // "All-value" datatype for handling gui changes.
  CommandData() 
      :element(NULL)
      ,buttons(NULL)
      ,sliderf(NULL)
      ,slideri(NULL)
      ,toggle(NULL)
      ,sliderf_value(0.0f)
      ,slideri_value(0)
      ,buttons_id(0)
      ,element_id(0)
      ,toggle_value(false)
      ,button_value(0)
      ,radio_value(0)
    {
      memset(color_value, 0, 4 * sizeof(unsigned int));
      memset(vector_value, 0, 2 * sizeof(float));
      memset(pad_value, 0, 2 * sizeof(float));
    }
    ButtonsBuffer buffer;
    CommandDataName name;
    Element* element;

    Buttons* buttons;
    Sliderf* sliderf;
    Slideri* slideri;
    Toggle* toggle;

    bool toggle_value;
    float sliderf_value;
    int slideri_value;
    int button_value;
    int radio_value; // selected index
    unsigned int color_value[4];
    float vector_value[2];
    float pad_value[2]; // x, y percentage

    unsigned int buttons_id;
    unsigned int element_id;
  };

  class Server;

  class ClientServerUtils {                            /* Utils for creating buffers which are send from client <--> server */
  public:
    ClientServerUtils();
    ~ClientServerUtils();

    bool serialize(const Buttons& buttons, 
                   const Element* target, 
                   ButtonsBuffer& result, 
                   void* targetData);

    bool deserialize(ButtonsBuffer& buffer, 
                     CommandData& data, 
                     std::map<unsigned int, std::map<unsigned int, Element*> >& elements);
  };


  struct ServerCommand {
    ServerCommand(char name);
    void setData(char* data, size_t nbytes);

    char name;
    ButtonsBuffer buffer;
  };


  class Connection {                                  /* Represents a connection from a client to the server */
  public:
    Connection(Server& server);
    ~Connection();
    void parseBuffer();                                /* Parse incoming bitstreamd and decode commands */
    void handleCommand(CommandData& cmd);              /* When parsing of the buffer returns a command we perform the correct action in here */
    void sendCommand(ServerCommand& cmd);
    void write(char* buf, size_t nbytes);              /* Writes data on the sockets */

  public:
    ButtonsBuffer buffer;                              /* Holds all received data */
    ClientServerUtils util;                            /* Client server utilities, used to serialize/deserialize a bitstream */
    Server& server;
    uv_tcp_t sock;
    uv_shutdown_t shutdown_req;
  };
  
  class Server : public ButtonsListener {
  public:
    Server(int port);                                  /* Pass the port on which the server must listen to the c'tor */
    ~Server();
    bool start();                                      /* Start the server so it's ready to accept new connections */
    void update();                                     /* Call update() repeatetly to process incoming data */
    void removeConnection(Connection* c);              /* Remove a connection when it disconnects */
    void addButtons(Buttons& gui);                     /* Add a buttons object you want to share across the network */
    void sendScheme(Connection* con);                  /* Sends the GUI scheme to the client */
    void onEvent(ButtonsEventType event,               /* When you change values on the server these are sent to all clients */
                 const Buttons& buttons, 
                 const Element* target, 
                 void* targetData);
    void sendToAll(ServerCommand& cmd);                /* Sends the given server command to all connected clients */

  private:
    void createScheme();                               /* Creates a binary scheme of the guis which is used on the client to create the gui */
    void clear();                                      /* Frees all allocated memory */

  public:
    ButtonsBuffer scheme;                              /* Contains the binary description of the added guis/elements. when a client conencts and asks for the scheme we send this */
    ClientServerUtils util;                            /* Client server utilities, used to serialize/deserialize a bitstream */
    std::map<unsigned int, std::map<unsigned int, buttons::Element*> > elements; /* Indexed by buttons-id with all elements */
    std::vector<Connection*> connections;              /* Collection of all the connected clients. */
    std::vector<Buttons*> buttons;                     /* Collection of buttons we share with all clients */
    uv_tcp_t sock;
    uv_loop_t* loop;
    int port;
  };

} // buttons
#endif
