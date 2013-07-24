#ifndef ROXLU_HTTP_HEADERS_H
#define ROXLU_HTTP_HEADERS_H

#include <map>
#include <vector>

#include <string>
#include <http/HTTPHeader.h>

class HTTPHeaders {
 public:
  HTTPHeaders();
  ~HTTPHeaders();
  void add(const std::string name, const std::string value);
  void add(const HTTPHeader h);
  std::string join(std::string nameValueSep = ": ", std::string lineSep = "\r\n");
  void copy(const HTTPHeaders& other);
  bool contains(const std::string name);       /* case insensitive search for header name */
  HTTPHeader get(const std::string name);      /* case insensitive; returns the header with the given name */
  void print();
 public:
  std::vector<HTTPHeader> entries;
};

inline bool HTTPHeaders::contains(std::string name) {
  for(std::vector<HTTPHeader>::iterator it = entries.begin(); it != entries.end(); ++it) {
    HTTPHeader& h = *it;
    if(h.icompare(name)) {
      return true;
    }
  }
  return false;
}

inline HTTPHeader HTTPHeaders::get(const std::string name) {

  for(std::vector<HTTPHeader>::iterator it = entries.begin(); it != entries.end(); ++it) {
    HTTPHeader h = *it;
    if(h.icompare(name)) {
      return h;
    }
  }
  HTTPHeader empty;
  return empty;
}
#endif
