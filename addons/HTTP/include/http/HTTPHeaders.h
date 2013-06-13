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
  void print();
 public:
  std::vector<HTTPHeader> entries;
};

#endif
