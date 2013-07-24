#ifndef ROXLU_HTTP_HEADER_H
#define ROXLU_HTTP_HEADER_H

#include <http/HTTPTypes.h>
#include <string>
#include <algorithm>
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
  bool icompare(const std::string n); /* case insensitive check on the name */

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

inline bool HTTPHeader::icompare(const std::string n) {
  std::string n_lower = n;
  std::string name_lower = name;
  
  std::transform(n_lower.begin(), n_lower.end(), n_lower.begin(), ::tolower);
  std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);

  return n_lower == name_lower;
}

#endif
