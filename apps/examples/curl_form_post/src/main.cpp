#include <iostream>
#include <roxlu/Roxlu.h>
#include <curl/Kurl.h>


void on_post_complete(KurlConnection* c, void* user) {
  RX_VERBOSE(("upload ready."));
}

int main() {
  Kurl kurl;

  Form form;
  form.setURL("http://video.democratielab.prodemos.nl?act=poster");
  form.addInput("name", "roxlu");
  form.addInput("email", "diederick@apollomedia.nl");
  form.addFile("logo", rx_to_data_path("apollo.png"));
    
  kurl.post(form, on_post_complete);

  while(true) {
    kurl.update();
  }
  return 1;
};
