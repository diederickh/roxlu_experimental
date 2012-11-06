#include <fstream>
#include <sys/socket.h>

#include <curl/curl.h>
#include <vector.h>

// Wrapper around libcurl
size_t kurl_callback_write_function(char* data, size_t size, size_t nmemb, void* userdata);
void kurl_callback_close_function(void* userdata, curl_socket_t sock);

class KurlConnection;
typedef void (*kurl_cb_on_complete)(KurlConnection* c, void* userdata);

enum KurlTypes {
  KURL_NONE,
  KURL_FILE_DOWNLOAD
};

class Kurl;

struct KurlConnection {
  KurlConnection();
  ~KurlConnection();
  CURL* handle;
  int type;
  void* data; // user data
  Kurl* kurl;
  std::ofstream ofs;
  void* complete_data;
  kurl_cb_on_complete complete_callback;
};

class Kurl {
public:
  Kurl();
  ~Kurl();
  void update();
  bool download(const char* url, const char* filename, kurl_cb_on_complete completeCB, void* completeData);
private:
  int still_running;
  CURLM* handle;
  std::vector<KurlConnection*> connections;
  
};
