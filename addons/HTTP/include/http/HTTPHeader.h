#ifndef ROXLU_HTTP_HEADER_H
#define ROXLU_HTTP_HEADER_H

#include <http/HTTPTypes.h>
#include <string>
#include <sstream>

//-------------------------------------------------------------------------------

class HTTPHeader {
 public:
  HTTPHeader();
  ~HTTPHeader();

  template< class T> HTTPHeader(const std::string n, T val, HTTPHeaderType t = HTTP_HEADER_NAME_VALUE) {
    type = t;
    name = n;
    setValue(val);
  }

  template< class T> void setValue(T val) {
    std::stringstream ss;  
    ss << val;
    value = ss.str();
  }

  void setName(const std::string n);
  void setType(HTTPHeaderType t);
  void print();

 public:
  std::string name;
  std::string value;
  HTTPHeaderType type;
};

//-------------------------------------------------------------------------------

inline void HTTPHeader::setName(const std::string n) {
  name = n;
}

inline void HTTPHeader::setType(HTTPHeaderType t) {
  type = t;
}

#endif
