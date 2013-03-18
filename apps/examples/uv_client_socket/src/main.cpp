#include <iostream>
#include <vector>
#include <string>
#include <uv/ClientSocket.h>

struct SomeType {
  std::vector<char> buffer;
};

static void connected_cb(ClientSocket* sock) {
  printf("Connected!\n\n-------------\n");
  std::string request = "GET / HTTP/1.0\r\nHost: www.roxlu.com\r\n\r\n";
  sock->write((char*)request.c_str(), request.size());
}

static void read_cb(char* buf, size_t nbytes, ClientSocket* sock) {
  SomeType* some_type = static_cast<SomeType*>(sock->user);

  for(int i = 0; i < nbytes; ++i) {
    printf("%c", buf[i]);
  }
  printf("-------------------------------\n");
  std::copy(buf, buf+nbytes, std::back_inserter(some_type->buffer));
  printf("+++++++++++++++++++++++++++++++\n");

  ::exit(EXIT_SUCCESS);
}


int main() {
  SomeType info_type;

  ClientSocket cs("www.roxlu.com", "80");
  cs.setup(connected_cb, read_cb, &info_type);
  cs.connect();

  while(true) {
    cs.update();
  }

  return EXIT_SUCCESS;
};

