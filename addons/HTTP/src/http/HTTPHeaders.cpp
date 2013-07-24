#include <http/HTTPHeaders.h>

HTTPHeaders::HTTPHeaders() {
}

HTTPHeaders::~HTTPHeaders() {
}

void HTTPHeaders::add(const std::string name, const std::string value) {
  HTTPHeader h(name, value);
  add(h);
}

void HTTPHeaders::add(const HTTPHeader h) {
  entries.push_back(h);
}

std::string HTTPHeaders::join(std::string nameValueSep, std::string lineSep) {
  std::string result;
  for(std::vector<HTTPHeader>::iterator it = entries.begin(); it != entries.end(); ++it) {
    HTTPHeader& h = *it; 
    if(h.type == HTTP_HEADER_NAME_VALUE) {
      result += h.name +nameValueSep +h.value +lineSep;
    }
    else if(h.type == HTTP_HEADER_VALUE) {
      result += h.value +lineSep;
    }
  }
  return result;
}

void HTTPHeaders::copy(const HTTPHeaders& other) {
  for(std::vector<HTTPHeader>::const_iterator it = other.entries.begin(); it != other.entries.end(); ++it) {
    const HTTPHeader& h = *it;
    add(h.name, h.value);
  }
}

void HTTPHeaders::print() {
  for(std::vector<HTTPHeader>::iterator it = entries.begin(); it != entries.end(); ++it) {
    HTTPHeader& h = *it; 
    h.print();
  }
}

