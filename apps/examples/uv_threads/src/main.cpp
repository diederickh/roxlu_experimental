extern "C" {
#  include <uv.h>
}

#include <iostream>
#include <vector>
#include <string>
#include <roxlu/Roxlu.h>


struct SomeType {
  int number;
};



static void threaded_function(void* user) {
  for(int i = 0; i < 10; ++i) {
    printf("++++ in threaded function (%d)\n", i);
    rx_sleep_millis(2000);
  }
}



int main() {
  SomeType my_type;
  my_type.number = 10;

  uv_thread_t my_thread;
  uv_thread_create(&my_thread, threaded_function, &my_type);

  for(int i = 0; i < 10; ++i) {
    printf("-- in the main thread (%d)\n", i);
    rx_sleep_millis(2000);
  }

  uv_thread_join(&my_thread);
  return EXIT_SUCCESS;
};

