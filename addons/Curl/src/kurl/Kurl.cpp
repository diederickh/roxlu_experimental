#include <kurl/Kurl.h>

size_t kurl_callback_write_function(char* data, size_t size, size_t nmemb, void* userdata) {
  if(nmemb > 0) {
        KurlConnection* kc = static_cast<KurlConnection*>(userdata);
    if(kc->write_callback) {
      kc->write_callback(kc, data, size, nmemb, kc->write_data);
    }
    else {
      kc->ofs.write(data, nmemb);
    }
  }
  return size * nmemb;
}

// -----------------------------------
KurlConnection::KurlConnection()
  :type(KURL_NONE)
  ,handle(NULL)
  ,complete_data(NULL)
  ,complete_callback(NULL)
{
}

KurlConnection::~KurlConnection() {
}

// -----------------------------------
Kurl::Kurl() 
  :handle(NULL)
  ,still_running(0)
{
  handle = curl_multi_init();
  if(handle == NULL) {
    printf("ERROR: cannot curl_multi_init\n");
    ::exit(0);
  }
}

Kurl::~Kurl() {
  if(handle) {
    curl_multi_cleanup(handle);
  }
}

void Kurl::update() {
  curl_multi_perform(handle, &still_running);  
  
  int q = 0;
  CURLMsg* msg = NULL;
  while((msg = curl_multi_info_read(handle, &q)) != NULL) {
    if(msg->msg == CURLMSG_DONE) {
      for(std::vector<KurlConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
        KurlConnection* c = *it;
        if(c->handle == msg->easy_handle) {
          if(c->type == KURL_FILE_DOWNLOAD) {
            c->ofs.close();
          }
          if(c->complete_callback) {
            c->complete_callback(c, c->complete_data);
          }

          delete c;
          it = connections.erase(it);
          break;
        }
      }
      curl_multi_remove_handle(handle, msg->easy_handle);
      curl_easy_cleanup(msg->easy_handle);
    }
  }
}

bool Kurl::download(
                    const char* url, 
                    const char* filename, 
                    kurl_cb_on_complete completeCB, 
                    void* completeData,
                    kurl_cb_on_write writeCB, // when given you need to store the data yourself!
                    void* writeData
                    ) 
{
  KurlConnection* c = new KurlConnection();
  c->handle = curl_easy_init();

  if(!c->handle) {
    printf("ERROR: cannot curl_easy_init()\n");
    return false;
  }
  
  // Only open a file when now writeCB is given
  if(writeCB != NULL) {
    c->ofs.open(filename, std::ios::out | std::ios::binary);
    if(!c->ofs.is_open()) {
      printf("ERROR: Kurl::download, ofs.is_open() failed\n");
      return false;
    }
  }

  c->type = KURL_FILE_DOWNLOAD;
  c->complete_data = completeData;
  c->complete_callback = completeCB;
  c->write_callback = writeCB;
  c->write_data = writeData;

  curl_easy_setopt(c->handle, CURLOPT_URL, url);
  curl_easy_setopt(c->handle, CURLOPT_WRITEDATA, c);
  curl_easy_setopt(c->handle, CURLOPT_WRITEFUNCTION, kurl_callback_write_function);
  curl_easy_setopt(c->handle, CURLOPT_FOLLOWLOCATION, 1);
  
  curl_multi_add_handle(handle, c->handle);
  connections.push_back(c);
  return true;
}

