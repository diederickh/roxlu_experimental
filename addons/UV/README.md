# UV

A couple of simple c++ wrappers around libuv. We implement a basic tcp client and server, 
see `ServerSocket` and `ClientSocket`. The `ServerIPC` and `ClientIPC` can be used for 
inter process communication using a unix domain socket.

## ServerIPC and ClientIPC

These two classes let you use unix domain sockets / named pipes in a client/server like fashion.
The ClientIPC and ServerIPC support a handy feature that allows you to easily implement a message
based system between multiple applications.  See below for an example where we use Buffer class
to serialize and deserialize messages.

_Unices_
````c++
std::string socket_file_name = "/tmp/myapp.sock";
````

_Win32_
````c++
std::string socket_file_name = "\\\\.\\pipe\\myapp"; // "myapp" is the custom name that you can change
````

### TODO
- Make the way we handle incoming data in the server connections and clients similar; I think
  it's a good choice to keep a `std::vector<char>` buffer for each client connection (server/client side)

_Client and Server IPC using serialization supported by the Buffer class_
````c++
#include <iostream>
#include <roxlu/core/Log.h>
#include <roxlu/io/Buffer.h>
#include <uv/IPC.h>

bool must_run = true;
void sh(int signum);

struct Person {
  Person():age(0),score(0.0),alive(false) {}
  int age;
  std::string name;
  float score;
  bool alive;
  int result;

  void print() {
    printf("age: %d,  ", age);
    printf("name: %s, ", name.c_str());
    printf("score: %f, ", score);
    printf("alive: %d ", alive);
    printf("result: %d", result);
    printf("\n");
  }

  // adds the pack() and unpack() member functions
  BUFFERIZE(age, name, score, result, alive);
};

void on_flushed(std::string path, char* data, size_t nbytes, void* user) {
  RX_VERBOSE("/flushed called!");
}

void on_test(std::string path, char* data, size_t nbytes, void* user) {
  RX_VERBOSE("/test called, %ld", nbytes);
}

void on_person(std::string path, char* data, size_t nbytes, void* user) {
  Buffer b(data, nbytes);
  Person p;
  p.unpack(b);
  p.print();
}

int main() {
  signal(SIGINT, sh);

  ServerIPC server("/tmp/service.sock", false);
  ClientIPC client("/tmp/service.sock", false);

  // start the server
  if(!server.start()) {
    RX_ERROR("Cannot start server");
    return EXIT_FAILURE;
  }
  server.update();

  // connect to server
  if(!client.connect()) {
    RX_ERROR("Cannot connect to server");
    return EXIT_FAILURE;
  }
  client.update();


  // register our server methods
  server.addMethod("/test", on_test, NULL);
  server.addMethod("/person", on_person, NULL);

  // register our client methods; get executed when the server sends us this command
  client.addMethod("/flushed", on_flushed, NULL);

  // wait untill the client has been added to the list of clients of the server
  while(server.connections.size() == 0) {
    server.update();
    client.update();
  }

  // execute some test commands (client -> server)
  Person person_a;
  person_a.name = "roxlu";
  person_a.age = 100;
  person_a.score = 200.5;
  person_a.alive = true;
  person_a.result = 145;

  Buffer buffered;
  person_a.pack(buffered);

  // and call /person
  client.call("/person", buffered.ptr(), buffered.size());
  client.call("/test", NULL, 0);

  // and let the server call it's clients.
  server.call("/flushed", NULL, 0);
  server.update();
  while(must_run) {
    client.update();
    server.update();
  }

  return EXIT_SUCCESS;
};


void sh(int signum) {
  RX_VERBOSE("Interrupted");
  must_run = false;
}

````