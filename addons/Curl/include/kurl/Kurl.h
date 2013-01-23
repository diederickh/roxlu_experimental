#include <fstream>
#if defined(__APPLE__) || defined(__linux) 
#include <sys/socket.h>
#endif
#include <curl/curl.h>
#include <cstdlib>
#include <vector>

#include  <kurl/Form.h>

#define RETURN_CURLCODE(result, msg, rvalue, ptr )      \
  if(result != CURLE_OK) {                              \
    printf(msg);                                        \
    if(ptr != NULL) {                                   \
        delete ptr;                                     \
        ptr = NULL;                                     \
    }                                                   \
    return rvalue;                                      \
  }                                          

#define RETURN_CURLFORMCODE(result, msg, rvalue, ptr) \
  if(result != 0) {                                   \
    printf(msg);                                      \
    if(ptr != NULL) {                                 \
        delete ptr;                                   \
        ptr = NULL;                                   \
    }                                                 \
    return rvalue;                                    \
 }

size_t kurl_callback_write_function(char* data, size_t size, size_t nmemb, void* userdata); // writes to file, or calls callback 

class KurlConnection;
typedef void (*kurl_cb_on_complete)(KurlConnection* c, void* userdata);
typedef void (*kurl_cb_on_write)(KurlConnection* c, char* data, size_t count, size_t nmemb, void* userdata);

enum KurlTypes {
  KURL_NONE,
  KURL_FILE_DOWNLOAD,
  KURL_FORM
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
  void* write_data;
  kurl_cb_on_complete complete_callback;
  kurl_cb_on_write write_callback;
};

class Kurl {
public:
  Kurl();
  ~Kurl();
  void update();

  bool download(
    const char* url, 
    const char* filename, 
    kurl_cb_on_complete completeCB, 
    void* completeData,
    kurl_cb_on_write writeCB = NULL, // when given you need to store the data yourself!
    void* writeData = NULL 
  );

  bool post(Form& fm,
    kurl_cb_on_complete completeCB, 
    void* completeData,
    kurl_cb_on_write writeCB = NULL, // when given you need to store the data yourself!
    void* writeData = NULL 
  );

private:
  int still_running;
  CURLM* handle;
  std::vector<KurlConnection*> connections;
};
