#include <roxlu/core/Log.h>
#include <http/HTTPRequest.h>

HTTPRequest::HTTPRequest() 
  :form_encoding(HTTP_FORM_ENCODING_NONE)
  ,method(HTTP_METHOD_GET)
  ,version(HTTP_VERSION_1_1)
{
}

HTTPRequest::HTTPRequest(HTTPURL url, HTTPMethod m, HTTPVersion v)
  :url(url)
  ,method(m)
  ,version(v)
  ,form_encoding(HTTP_FORM_ENCODING_NONE)
{
  generateBoundary();
}

HTTPRequest::~HTTPRequest() {
}

void HTTPRequest::addHeader(HTTPHeader h) {
  headers.add(h);
}

void HTTPRequest::addContentParameter(HTTPParameter p) {
  content_parameters.add(p);
}

// returns the string that is used as the first line in a http request
std::string HTTPRequest::getHTTPString() {
  std::string req;

  if(method == HTTP_METHOD_GET) {
    req = "GET";
  }
  else if(method == HTTP_METHOD_POST) {
    req = "POST";
  }
  else {
    RX_ERROR("Unhandled HTTPMethod!");
  }

  std::string path = url.getPath();
  if(url.hasQueryParameters()) {
    path += "?" +url.getQueryString();
  }
  req += " " +path +" HTTP/" +((version == HTTP_VERSION_1_0) ? "1.0" : "1.1");
  return req;
}

void HTTPRequest::addDefaultHTTPHeaders() {
  addHeader(HTTPHeader("Host", url.getHost()));

  if(getFormEncoding() == HTTP_FORM_MULTIPART) {
    addHeader(HTTPHeader("Content-Type", "multipart/form-data; boundary=\"" +getBoundary() +"\""));
  }
  else if(getFormEncoding() == HTTP_FORM_URL_ENCODED) {
    addHeader(HTTPHeader("Content-Type", "application/x-www-form-urlencoded"));
  }
}

bool HTTPRequest::createBody(std::string& result) {
 if(content_parameters.size()) {
    if(isPost() && content_parameters.hasFileParameter()) {
      return content_parameters.toBoundaryString(getBoundary(), result);
    }
    else {
      if(content_parameters.size()) {
        content_parameters.percentEncode();
        result = content_parameters.getQueryString();
      }
    }
  }

 return true;
}


// create the request string
bool HTTPRequest::toString(std::string& result) {

  // create the content string
  std::string body;
  if(!createBody(body)) {
    RX_ERROR("Cannot create request body");
    return false;
  }

  // create the headers.
  addDefaultHTTPHeaders();
  addHeader(HTTPHeader("Content-Length", body.size()));

  // construct the request
  result = getHTTPString() +"\r\n";
  result += headers.join();
  result += "\r\n";
  result += body;

  return true;
}

// The boundary string is used when posting multipart/form-data 
void HTTPRequest::generateBoundary() {
  if(boundary.size()) {
    RX_ERROR("Already created an boundary. Not updating");
    return;
  }
  
  std::stringstream ss;
  ss << uv_hrtime();
  boundary = "ROXLU" +ss.str();
}

void HTTPRequest::print() {
  headers.print();
}

// Returns the previously set form encoding or detects which one needs to be used
HTTPFormEncoding HTTPRequest::getFormEncoding() {
  if(form_encoding != HTTP_FORM_ENCODING_NONE) {
    return form_encoding;
  }

  if(isPost() && content_parameters.hasFileParameter()) {
    form_encoding = HTTP_FORM_MULTIPART;
  }
  else {
    form_encoding = HTTP_FORM_URL_ENCODED;
  }

  return form_encoding;
}
