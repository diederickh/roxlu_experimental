/*

 # HTTPConnection

 The `HTTPConnection` represents a tcp connection between the application and
 the http-server. It uses an abstracted buffer for all I/O operations. This makes
 it easy for this class to switch between a SSL connection (which uses it's own 
 internal memory IO) and a basic plain text connection.
 
 Although the use of this abstracted buffer is a bit more work for the CPU, it
 makes using this class very easy; and the couple of extra CPU cycles are neglectible
 on most modern pc's. 

 */

#ifndef ROXLU_HTTP_CONNECTION_H
#define ROXLU_HTTP_CONNECTION_H

extern "C" {
#  include <uv.h>
}

#include <ssl/SSLContext.h>
#include <http/http_parser.h>
#include <http/HTTPRequest.h>
#include <http/HTTPBuffer.h>

// http_parser callback;
//-------------------------------------------------------------------------------
int httpconnection_on_message_begin(http_parser* p);
int httpconnection_on_status_complete(http_parser* p);
int httpconnection_on_url(http_parser* p, const char* at, size_t len);
int httpconnection_on_header_field(http_parser* p, const char* at, size_t len);
int httpconnection_on_header_value(http_parser* p, const char* at, size_t len);
int httpconnection_on_headers_complete(http_parser* p);
int httpconnection_on_body(http_parser* p, const char* at, size_t len);
int httpconnection_on_message_complete(http_parser* p);

// network callbacks
//-------------------------------------------------------------------------------
void httpconnection_on_resolved(uv_getaddrinfo_t* req, int status, struct addrinfo* res);
void httpconnection_on_connect(uv_connect_t* req, int status);
void httpconnection_on_read(uv_stream_t* handle, ssize_t nbread, uv_buf_t buf);
void httpconnection_on_write(uv_write_t* req, int status);
void httpconnection_on_shutdown(uv_shutdown_t* req, int status);
void httpconnection_on_close(uv_handle_t* req);
uv_buf_t httpconnection_on_alloc(uv_handle_t* handle, size_t nbytes);

// Buffer I/O callbacks
//-------------------------------------------------------------------------------
void httpconnection_on_flush_input_buffer(const char* data, size_t len, void* user);   /* gets called when data is added to the input buffer and it should be read/parsed */
void httpconnection_on_flush_output_buffer(const char* data, size_t len, void* user);  /* gets called when data is added to the output buffer and it should be flush/sent */

//-------------------------------------------------------------------------------

class HTTPConnection;
typedef void(*httpconnection_event_callback)(HTTPConnection* c, HTTPConnectionEvent event, const char* data, size_t len, void* user);
//-------------------------------------------------------------------------------

class HTTPConnection {

 public:
  HTTPConnection(uv_loop_t* loop, std::string host, std::string port, SSL* ssl = NULL);
  ~HTTPConnection();
  bool connect(httpconnection_event_callback eventCB, void* eventUser);        /* connect to the set host/port and send the output buffer when connected. When something happens we call the given callback (`cb`) and pass the user param */
  void addToOutputBuffer(const char* data, size_t len);                                                                             /* users use addToOutputBuffer to add data to the out queue, internally we use send() */
  void addToOutputBuffer(const std::string str);                                                                                    /* users use addToOutputBuffer to add data to the out queue, internally we use send() */
  void addToInputBuffer(const char* data, size_t len);                                                                              /* whenever we receive data on the socket it will be added to this output buffer (this might be SSL encrypted data) */
  void addToInputBuffer(const std::string str);                                                                                     /* whenever we receive data on the socket it will be added to this output buffer (this might be SSL encrypted data) */
  bool send(char* data, size_t len);                                                                                                /* write data to the socket */
  bool close();                                                                                                                     /* close the connection */
                                                                                                                                   
 public:                                                                                                                           
  SSL* ssl;                                                                                                                         /* SLL object, might be NULL */
  HTTPBufferBase* buffer;                                                                                                           /* the standard of SSL buffer that we use as I/O wrapper when sending or receiving data. */
  HTTPHeaders headers;                                                                                                              /* received headers */
  std::string last_header;                                                                                                          /* used when parsing headers */
  std::string host;                                                                                                                 /* host to which we connect */
  std::string port;                                                                                                                 /* port on which we connect */
                                                                                                                                    
  uv_loop_t* loop;                                                                                                                  /* the loop which we get from the HTTP context */
  uv_getaddrinfo_t resolver_req;                                                                                                    /* used when resolving the host name */
  uv_connect_t connect_req;                                                                                                         /* used when connecting to the server */
  uv_shutdown_t shutdown_req;                                                                                                       /* used when we request the socket to shutdown */
  uv_tcp_t* sock;                                                                                                                   /* the cross platform socket wrapper */
  uv_write_t write_req;                                                                                                             /* write request; used when writing data over the socket */
  http_parser parser;                                                                                                               /* joyents excellent http parser */
  http_parser_settings parser_settings;                                                                                             /* struct with info/callback data for the http_parser */
  httpconnection_event_callback cb_event;                                                                                           /* gets called when something happens with the connection (e.g. disconnected) */
  httpconnection_event_callback cb_close;                                                                                           /* gets called when the connection closes (cb_event is called too) */          
  void* cb_event_user;                                                                                                              /* is passed into the event handler */       
  void* cb_close_user;                                                                                                              /* is passed into the close handler */
};

#endif
