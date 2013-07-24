#include <roxlu/core/Log.h>
#include <http/HTTP.h>

//-------------------------------------------------------------------------------

void http_on_connection_event(HTTPConnection* c, HTTPConnectionEvent event, const char* data, size_t len, void* user) {
  HTTP* h = static_cast<HTTP*>(user);
  h->removeConnection(c);
}

//-------------------------------------------------------------------------------

HTTP::HTTP() 
  :loop(NULL)
{

  loop = uv_loop_new();
  if(!loop) {
    RX_ERROR("Cannot allocate an uv_loop");
    ::exit(EXIT_FAILURE);
  }

}

HTTP::~HTTP() {

  // destroy all open connections!
  for(std::vector<HTTPConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    delete *it;
  }
  connections.clear();

  if(loop) {
    uv_loop_delete(loop);
    loop = NULL;
  }
}

bool HTTP::update() {

  if(!loop) {
    RX_ERROR("Cannot update because no uv_loop handler was found");
    return false;
  }

  uv_run(loop,UV_RUN_ONCE);

  return true;
}

HTTPConnection* HTTP::sendRequest(HTTPRequest& r,                           /* the request that we will sent */
                                  httpconnection_event_callback eventCB,    /* this function gets called when we receive data, see HTTPConnection.h */
                                  void* user,                               /* the user pointer that gets passed into `eventCB` */
                                  SSL* ssl)                                 /* pass a SSL* when you want to make a secure connection */
{

  // create request string.
  std::string request_str;
  if(!r.toString(request_str)) {
    RX_ERROR("Cannot create request string");
    return NULL;
  }

  // create connection
  HTTPConnection* c = new HTTPConnection(loop, r.getURL().getHost(), r.getURL().getPort(), ssl);
  c->cb_close = http_on_connection_event;
  c->cb_close_user = this;
  c->addToOutputBuffer(request_str);
  c->connect(eventCB, user);

  connections.push_back(c);
  return c;
}

bool HTTP::removeConnection(HTTPConnection* c) {
  std::vector<HTTPConnection*>::iterator it = std::find(connections.begin(), connections.end(), c);
  if(it == connections.end()) {
    return false;
  }

  connections.erase(it);
  delete c;
  return true;
}
