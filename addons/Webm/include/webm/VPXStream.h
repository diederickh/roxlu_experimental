/*

Protocol description
--------------------
We use a very simply protocol where we basically send only VPXCommands which all
have this format: COMMAND u(8) + DATA_SIZE u(32) + DATA u(8 * DATA_SIZE). All 
integers are stored in big endian format.

Stream flow
----------
(1) Client connects -----> 
                            <------- Server sends most important part of VPXSettings (1)
                            <------- Server start sending lots of VPX_CMD_FRAME commands (*)
(1) Client disconnects ----> 
                            <------- Server removes connection


*/
#ifndef ROXLU_VPX_STREAM_H
#define ROXLU_VPX_STREAM_H

extern "C" {
#  include <uv.h>
}

#include <roxlu/opengl/GL.h>
#include <stdio.h>
#include <roxlu/core/Log.h>
#include <webm/VPXEncoder.h>
#include <webm/VPXDecoder.h>
#include <algorithm>
#include <vector>
#include <string>

#define VPX_UV_ERR(r, okval, msg, ret)      \
  if(r != okval) { \
     RX_ERROR(msg);  \
     RX_ERROR("uv error: %s", uv_strerror(uv_last_error(uv_default_loop()))) \
     return ret; \
  }

/* server networking callbacks */
void vpx_server_on_new_connection(uv_stream_t* sock, int status);
void vpx_server_on_read(uv_stream_t* sock, ssize_t nread, uv_buf_t buf);
void vpx_server_on_shutdown(uv_shutdown_t* req, int status);
void vpx_server_on_close(uv_handle_t* handle);
uv_buf_t vpx_server_on_alloc(uv_handle_t* handle, size_t nbytes);

/* connections on server */
void vpx_connection_on_write(uv_write_t* req, int status);

/* client networking callback */
void vpx_client_on_resolved(uv_getaddrinfo_t* req, int status, struct addrinfo* res);
void vpx_client_on_connect(uv_connect_t* req, int status);
void vpx_client_on_read(uv_stream_t* handle, ssize_t read, uv_buf_t buf);
void vpx_client_on_write(uv_write_t* req, int status);
void vpx_client_on_shutdown(uv_shutdown_t* req, int status);
void vpx_client_on_close(uv_handle_t* handle);
void vpx_client_on_reconnect_timer(uv_timer_t* handle, int status);
uv_buf_t vpx_client_on_alloc(uv_handle_t* handle, size_t nbytes);

struct VPXBuffer {
  VPXBuffer();
  ~VPXBuffer();
  void reset();                                                                   /* resets the data vector and read dx */
  void clear();                                                                   /* clears the buffer */
  void addBytes(char* b, size_t nbytes);                                          /* copy bytes to our bufer */
  size_t size();
  char* ptr();                                                                    /* get ptr to the start of the buffer */
  char* readPtr();                                                                /* ptr()+read_dx */
  void print();

  void write(char* buf, size_t nbytes);                                           /* store arbitrary number of bytes */
  void wu8(uint8_t byte);                                                         /* store u8 */
  void wu16(uint16_t byte);                                                       /* store u16, BE */
  void wu32(uint32_t byte);                                                       /* store u32, BE */

  uint8_t ru8();                                                                  /* read u8, move read index */
  uint16_t ru16();                                                                /* read u16, move read index */
  uint32_t ru32();                                                                /* read u32, move read index */
  
  uint8_t pu8(int dx = 0);                                                        /* peek u8 */
  uint16_t pu16(int dx = 0);                                                      /* peek u16 */
  uint32_t pu32(int dx = 0);                                                      /* peek u32 */

  void flush(int nbytes);                                                         /* erase data from buffer */

  std::vector<uint8_t> data;
  int read_dx;
};

/* grabber callback */
void vpx_server_on_grabber_cb(unsigned char* ptr, size_t nbytes, void* user);

/* vpx codec callbacks */
void vpx_server_on_vpx_write(const vpx_codec_cx_pkt_t* pkt, int64_t pts, void* user);  /* called by encoder when there is encoded data */
 

#define VPX_SCREENGRABBER_FBOS 4

typedef void(*vpx_screengrabber_cb)(unsigned char* ptr, size_t nbytes, void* user); /* gets called by the vpx screengrabber when it has data (which is used to encode) */

class VPXScreenGrabber {                                                          /* helper class that we use to grab pixels from the gpu in a fast way */
 public:
  VPXScreenGrabber();
  ~VPXScreenGrabber();
  bool setup(VPXSettings cfg, vpx_screengrabber_cb grabCB, void* grabUser);
  void grabFrame();                                                               /* call this after each draw() you want to record. we will call the set callback when we actually grab data (based on the set fps) */
 private:
  bool mustGrabFrame();                                                           /* internally used to check if we need to grab a new frame to keep up with the set fps */
 private:
  bool is_setup;                                                                  /* you must call setup; this is used to check if setup is called */
  VPXSettings settings;                                                           /* settings that we use for encoding and grabbing pixels from the screen */
  int64_t frame_timeout;                                                          /* when we timeout we grab a new frame */
  int millis_per_frame;                                                           /* millis per frame, based on the vpxsettings */
  int num_bytes;                                                                  /* number of bytes in one raw frame */
  GLuint pbos[VPX_SCREENGRABBER_FBOS];                                            /* our pbos to boost pixel transfers */
  int read_dx;                                                                    /* we read from another pbo, in such a way that the gpu can sync up we glMapBuffer doesnt stall */
  int write_dx;                                                                   /* "" */
  vpx_screengrabber_cb cb_grabber;                                                /* we call this function when we've data to be processed; note that you're in the same thread as GL! */
  void* cb_user;                                                                  /* passed through to the callback */
};

enum VPXCommands {                                
  VPX_CMD_SETTINGS,                                                               /* when a client connects the server automatically sends the used VPX settings */
  VPX_CMD_FRAME                                                                   /* every time we have a new encoded VPX frame on the server (outstream), we send it with this command */ 
};
 
struct VPXCommand {                                                               /* basic structure we used in communication: */
  VPXCommand(unsigned char cmd);                                                  /*   - byte 1:  the command name */
  unsigned char cmd;                                                              /*   - byte 2,3,4,5: unsigned int, BE, size of data */
  VPXBuffer buffer;                                                               /*    - # bytes: the data for the command */
};

struct VPXOutStream;

struct VPXConnection {                                                            /* a VPXConnection represents a connection that is handled by the server (VPXOutStream) */
  VPXConnection(VPXOutStream& os);
  ~VPXConnection();
  void parseBuffer();                                                             /* parsed the incoming buffer (commands from a client) */
  void write(char* data, size_t nbytes);                                          /* write to socket */
  void writeCommand(VPXCommands cmd, VPXBuffer& buf);

  uv_tcp_t sock;                                                                  /* the socket, connected to the server */
  uv_shutdown_t shutdown_req;                                                     /* context, used to shutdown the connection */
  VPXOutStream& out_stream;                                                       /* the server to which the VPXConnection is connected */
  VPXBuffer buffer;                                                               /* input buffer, received form this socket */
  bool keyframe_sent;                                                             /* when a new client connects we force the encoder to generate a new keyframe, and after it outputs a keyframe we start sending to the client; we can't send any I/P frames before the first keyframe */
};

struct VPXInStream {
  VPXInStream(std::string host, std::string port);
  ~VPXInStream();
  bool connect(vpx_read_cb decodeCB, void* user);
  void update();

  void parseBuffer();                                                             /* (do not call), used to parse the incoming bitstream, is called for you */
  void clear();                                                                   /* resets all allocated data */
  void reconnect();                                                               /* when we get disconnected we automatically try to reconnect */

  std::string host;
  std::string port;

  VPXBuffer buffer;                                                               /* the received data (encoded VPX) */
  VPXSettings settings;                                                           /* is created after receiving the header from the server */
  VPXDecoder decoder;                                                             /* this decodes the incoming VPX data */
  int proto_version;                                                              /* protocol version, started with '1' */
  uv_loop_t* loop;
  uv_tcp_t sock;
  uv_shutdown_t shutdown_req;
  uv_getaddrinfo_t resolver_req;
  uv_connect_t connect_req;
  uv_timer_t timer_req;
  vpx_read_cb cb_decode;                                                          /* gets called when we have decoded pixels */
  void* cb_user;                                                                  /* gets passed into the cb_decode callback */
};

struct VPXOutStream {                                                             /* this is our 'server', clients can connect and get video data frome it */
public:
  VPXOutStream(int port);                                                         /* pass the port of the server to the ctor. clients can connect to this port */
  ~VPXOutStream();
  bool setup(VPXSettings settings);
  bool start();                                                                   /* call start when you want to start the server and accept clients */
  void grabFrame();
  void update();                                                                  /* call update() as much as possible; this processing the socket buffers */
  void removeConnection(VPXConnection* c);                                        /* removes a connection; you don't need to call this */


  /* privates, but public for callbacks */
  void writeHeader(VPXConnection* c);                                              /* sends the necessary settings to encode the video */
  void writeToAllConnections(VPXCommands cmd, char* buf, size_t nbytes);           /* write a command to all clients/connections */

public:
  bool is_setup;
  VPXScreenGrabber grabber;
  VPXEncoder encoder;
  VPXSettings settings;
  std::vector<VPXConnection*> connections;
  uv_tcp_t sock;
  uv_loop_t* loop;
  int port;
  unsigned int num_frames;
};

#endif
