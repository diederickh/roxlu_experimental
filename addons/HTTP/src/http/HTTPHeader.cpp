#include <roxlu/core/Log.h>
#include <http/HTTPHeader.h>

HTTPHeader::HTTPHeader() {
}

HTTPHeader::~HTTPHeader() {
}

void HTTPHeader::print() {
  if(type == HTTP_HEADER_NAME_VALUE) {
    RX_VERBOSE("%s = %s", name.c_str(), value.c_str());
  }
  else if(type == HTTP_HEADER_VALUE) {
    RX_VERBOSE("%s", value.c_str());
  }
  else {
    RX_WARNING("No print() function for type: %d", type);
  }
}
