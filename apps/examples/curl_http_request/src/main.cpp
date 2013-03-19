#include <iostream>
#include <curl/Kurl.h>

struct MyState {
  bool ready;
};

void on_request_ready(std::string& buffer, void* user) {
  MyState* state = static_cast<MyState*>(user);
  state->ready = true;
}


int main() {
  printf("Curl http_request example\n");
  MyState state;
  state.ready = false;
  
  Kurl k;
  k.get("http://roxlu.com/blender_header_export",
        on_request_ready,
        &state);
        
  // Kurl is asynchronously, so it won't block the main thread. 
  // You need to call update() repeatedly till everything is downloaded.
  while(!state.ready) {
    k.update(); // we need to call update as we're using non-blocking sockets. 
  }

  return 1;
};
