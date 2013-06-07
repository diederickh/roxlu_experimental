#ifndef ROXLU_HTTP_URL_H
#define ROXLU_HTTP_URL_H

#include <string>
#include <http/HTTPTypes.h>
#include <http/HTTPParameters.h>

class HTTPURL {
 public:
  HTTPURL();
  HTTPURL(std::string host, std::string path, HTTPProto proto = HTTP_PROTO_HTTP);
  ~HTTPURL();
  
  std::string toString();

  void setHost(std::string host);
  void setPath(std::string path); 
  void setProto(HTTPProto proto);
  void setPort(std::string port);
  void addQueryParameter(HTTPParameter p);
  void addQueryParameters(HTTPParameters& p);
  bool hasQueryParameters();
  std::string getQueryString();                                     /* get url encoding query string */

  std::string getHost();
  std::string getPath();
  std::string getPort();
  HTTPProto getProto();
  HTTPParameters& getQueryParameters();
  
 public:
  HTTPParameters query_parameters;
  HTTPProto proto;
  std::string host;
  std::string path;
  std::string port;
};

inline void HTTPURL::setHost(std::string h) {
  host = h;
}

inline void HTTPURL::setPath(std::string p) {
  path = p;
}

inline void HTTPURL::setProto(HTTPProto p) {
  proto = p;
}

inline std::string HTTPURL::toString() {
  std::string u;

  if(proto == HTTP_PROTO_HTTP) {
    u = "http://";
  }
  else if(proto == HTTP_PROTO_HTTPS) {
    u = "https://";
  }

  u += host + path;

  return u;
}

inline std::string HTTPURL::getHost() {
  return host;
}

inline std::string HTTPURL::getPort() {
  return port;
}

inline std::string HTTPURL::getPath() {
  return path;
}

inline HTTPProto HTTPURL::getProto() {
  return proto;
}

inline void HTTPURL::addQueryParameter(HTTPParameter p) {
  query_parameters.add(p);
}

inline void HTTPURL::addQueryParameters(HTTPParameters& p) {
  query_parameters.copy(p);
}

inline HTTPParameters& HTTPURL::getQueryParameters() {
  return query_parameters;
}

inline bool HTTPURL::hasQueryParameters() {
  return query_parameters.size();
}

#endif
