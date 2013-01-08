#include <iostream>
#include <roxlu/experimental/Input.h>
int main() {
  printf("TEST\n");
  Input input;
  input.onKeyDown('a');
  input.onKeyDown('b');
  input.onKeyDown('c');
  input.onKeyDown('d');
  input.onKeyDown('e');
  input.onKeyDown('f');
  return 1;
}
