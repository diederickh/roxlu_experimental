#ifndef ROXLU_HTTP_H
#define ROXLU_HTTP_H

extern "C" {
#  include <uv.h>
};

#include <vector>
#include <ssl/SSLContext.h>
#include <http/HTTPConnection.h>
#include <http/HTTPRequest.h>
#include <http/HTTPParameters.h>
#include <http/HTTPParameter.h>

//-------------------------------------------------------------------------------

void http_on_connection_event(HTTPConnection* c, HTTPConnectionEvent event, char* data, size_t len, void* user);

//-------------------------------------------------------------------------------

class HTTP {
 public:
  HTTP();
  ~HTTP();

  bool update();                                                                                                         /* call this as often as possible; it will make sure all data is transferred */
  HTTPConnection* sendRequest(HTTPRequest& r, httpconnection_event_callback eventCB = NULL, void* user = NULL, SSL* ssl = NULL); /* create a new request, when SSL is not NULL, the request will make use of SSL to encrypt the transfer. the callback function is called when we received data from the remove server, user is passed into this function then.  */
  bool removeConnection(HTTPConnection* c);                                                                              /* delete and removes the connection */

 public:
  uv_loop_t* loop;
  std::vector<HTTPConnection*> connections;
};

#endif
