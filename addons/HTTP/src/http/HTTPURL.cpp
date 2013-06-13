#include <roxlu/core/Log.h>
#include <http/HTTPURL.h>

HTTPURL::HTTPURL() 
  :proto(HTTP_PROTO_HTTP)
  ,port("80")
  ,path("/")
{
}

HTTPURL::HTTPURL(std::string host, std::string path, HTTPProto proto) 
  :host(host)
  ,path(path)
  ,proto(proto)
  ,port("80")
{
  if(proto == HTTP_PROTO_HTTPS) {
    port = "443";
  }
}

HTTPURL::~HTTPURL() {
}

std::string HTTPURL::getQueryString() {
  std::string result;
  HTTPParameters tmp;

  tmp.copy(query_parameters);
  tmp.percentEncode();
  result = tmp.getQueryString();
  return result;
}
