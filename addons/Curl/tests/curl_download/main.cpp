#include <stdio.h>
#include <iostream.h>
#include <kurl/Kurl.h>

int main() {
  printf("Curl download test.\n");
  Kurl k;
  k.download("http://test.localhost/stream2.html", "test.html");
  k.download("http://pbs.twimg.com/media/A69TwuiCYAAgT1u.jpg", "test.jpg");
  printf("Start!\n");
  while(true) {
    k.update();
  }
  return 0;
}
