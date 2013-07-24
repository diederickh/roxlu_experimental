# HTTP, SSL, OAUTH

The HTTP addon implements a basic HTTP and HTTPS (secure, SSL) client. This HTTP
addon implements a couple of features like file uploads, form posts, basic http
requests and secure http requests. The main goal of this addon is to simplify interfacing
public APIs (like the twitter, instagram or flickr ones). Therefore we have a simple OAUTH 1.0 
implementation which takes care of all the encoding and signature creation for making 
authorized requests. It does not handle getting the access token and secret yet. Often you 
can get these in another way for an authorized application (e.g. twitter gives you this 
information in on their [developers website](http://developer.twitter.com).

#### How to use this addon

All HTTP requests are managed by one `HTTP` context. Therefore create a `HTTP` 
object and make sure to call the `update()` function regularly. Calling `update()` 
will write/read all pending data (in and out) from the sockets. You can sent a HTTP 
request by calling `HTTP::sendRequest()`.  The `sendRequest()` function has the 
following definition (see comments for their meaning)

_Sending a request_
````c++
HTTPConnection* sendRequest(HTTPRequest& r,                                /* the HTTPRequest object. make sure to set the URL */
                            httpconnection_event_callback eventCB = NULL,  /* the event callback which is called when we've got a http-status and/or when we received data */
                            void* user = NULL,                             /* this will be passed into the `eventCB` as user */
                            SSL* ssl = NULL                                /* when you want to make a secure connection pass a SSL object, see the SSLContext::allocateSSL() for more info */
);
````

_The callback that gets called when data is received on the socket_

The callback, which you pass for the `eventCB` parameters for `HTTP::sendRequest`, must 
follow this scheme: 

````c++
void your_own_callback(HTTPConnection* c, HTTPConnectionEvent event, const char* data, size_t len, void* user)
````

The `HTTPConnection` is a pointer to the underlying socket connection and is used to connect 
and transfer data. `HTTPConnectionEvent` is an integer value and represents the type of event for
which the callback got called. When we've parsed the http response headers and know the http status, 
the `HTTPConnectionEvent` will have a value like `HTTP_ON_STATUS`, when we read data from the body 
section it will have a value of `HTTP_ON_BODY`. Use this event value to execute the appropriate 
code. `data` will contain data in case you receive the `HTTP_ON_BODY` event. 
**note: this is not a NULL terminated string**. 


#### Examples

_Sending a simple http request_

````c++

// the callback that gets called
void request_cb(HTTPConnection* c, HTTPConnectionEvent event, const char* data, size_t len, void* user) {
  if(event == HTTP_ON_STATUS) {
    RX_VERBOSE("STATUS: %d", c->parser.status_code);
  }
  else if(event == HTTP_ON_BODY) {
    std::string str;
    str.assign(data, data+len);
    printf("%s", str.c_str());
  }
}

HTTP h;
HTTPRequest r(HTTPURL("test.localhost", "/test.html"));
h.sendRequest(r, request_cb, NULL);

// call HTTP::update() regularly (this is just an example)
while(true) {
  h.update();            
}

````

_Creating a secure request (SSL)_

````c++

// your callback 
void request_cb(HTTPConnection* c, HTTPConnectionEvent event, const char* data, size_t len, void* user) {
  if(event == HTTP_ON_STATUS) {
    RX_VERBOSE("STATUS: %d", c->parser.status_code);
  }
  else if(event == HTTP_ON_BODY) {
    std::string str;
    str.assign(data, data+len);
    printf("%s", str.c_str());
  }
}

// Somewhere you need to define a HTTP context (this is just an example)
HTTP h;

// create a secure request
SSLContext ssl_ctx;
HTTPURL url("test.localhost", "/info.php", HTTP_PROTO_HTTPS);
HTTPRequest r(url, HTTP_METHOD_GET, HTTP_VERSION_1_1);
SSL* ssl = ssl_ctx.allocateSSL(); // the ssl ownership is taken by the HTTPConnection
h.sendRequest(r, request_cb, NULL, ssl);

// call HTTP::update() regularly (this is just an example)
while(true) {
  h.update();            
}
    
````

_Simple form post_

````c++
// your callback
void request_cb(HTTPConnection* c, HTTPConnectionEvent event, const char* data, size_t len, void* user) {
  if(event == HTTP_ON_STATUS) {
    RX_VERBOSE("STATUS: %d", c->parser.status_code);
  }
  else if(event == HTTP_ON_BODY) {
    std::string str;
    str.assign(data, data+len);
    printf("%s", str.c_str());
  }
}

// Somewhere you need to define a HTTP context (this is just an example)
HTTP h;

// create the request
HTTPURL url("test.localhost", "/simple_form/index.php");
HTTPRequest r(url, HTTP_METHOD_POST, HTTP_VERSION_1_1);

r.addContentParameter(HTTPParameter("name", "roxlu"));
r.addContentParameter(HTTPParameter("age", "120"));
r.addContentParameter(HTTPParameter("score", "100"));

h.sendRequest(r, request_cb);

// call HTTP::update() regularly (this is just an example)
while(true) {
  h.update();            
}
````

_Simple form post with file upload_

````c++
// your callback
void request_cb(HTTPConnection* c, HTTPConnectionEvent event, const char* data, size_t len, void* user) {
  if(event == HTTP_ON_STATUS) {
    RX_VERBOSE("STATUS: %d", c->parser.status_code);
  }
  else if(event == HTTP_ON_BODY) {
    std::string str;
    str.assign(data, data+len);
    printf("%s", str.c_str());
  }
}

// Somewhere you need to define a HTTP context (this is just an example)
HTTP h;

// Setup the request
HTTPURL url("test.localhost", "/simple_form/index.php");
HTTPRequest r(url, HTTP_METHOD_POST, HTTP_VERSION_1_1);
r.addContentParameter(HTTPParameter("name", "Diederick Huijbers"));
r.addContentParameter(HTTPParameter("age", "90"));
r.addContentParameter(HTTPParameter("score", "100"));

// Add a file parameter
HTTPParameter file;
file.setName("file");
file.addFile("test.jpg", true);
r.addContentParameter(file);

h.sendRequest(r, request_cb);

// call HTTP::update() regularly (this is just an example)
while(true) {
  h.update();            
}
````

_Uploading multiple files_
````c++

// your callback
void request_cb(HTTPConnection* c, HTTPConnectionEvent event, const char* data, size_t len, void* user) {
  if(event == HTTP_ON_STATUS) {
    RX_VERBOSE("STATUS: %d", c->parser.status_code);
  }
  else if(event == HTTP_ON_BODY) {
    std::string str;
    str.assign(data, data+len);
    printf("%s", str.c_str());
  }
}

// Create the request and add some form elements + 2 files
HTTPURL url("test.localhost", "/simple_form/index.php");
HTTPRequest r(url, HTTP_METHOD_POST, HTTP_VERSION_1_1);
r.addContentParameter(HTTPParameter("name", "Diederick Huijbers"));
r.addContentParameter(HTTPParameter("age", "90"));
r.addContentParameter(HTTPParameter("score", "100"));

HTTPParameter param;
param.setName("media[]");

HTTPFile f1("test.jpg", true);
f1.setContentType("image/jpeg");
param.addFile(f1);

HTTPFile f2("img.png", true);
f2.setContentType("image/png");
param.addFile(f2);

r.addContentParameter(param);

h.sendRequest(r, request_cb);

// call HTTP::update() regularly (this is just an example)
while(true) {
  h.update();            
}
````


