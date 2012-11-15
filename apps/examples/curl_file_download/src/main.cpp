#include <iostream>
#include <kurl/Kurl.h>

class MyApp {
public:
  int x;
  int y;
  bool downloaded;
};


void on_download_complete(KurlConnection* c, void* userdata) {
  printf("Download complete.\n");
  MyApp* app = static_cast<MyApp*>(userdata);
  app->downloaded = true;
}


int main() {
  printf("Curl download example\n");

  // Just some user data we receive in on_download_complete
  MyApp app;
  app.x = 100;
  app.y = 50;
  app.downloaded = false;

  // Use our wrapper to download
  Kurl k;
  k.download("http://roxlu.com/site/images/logo.png", "logo.png", on_download_complete, &app);

  while(!app.downloaded) {
    k.update(); // we need to call update as we're using non-blocking sockets. 
  }
  return 1;
};
