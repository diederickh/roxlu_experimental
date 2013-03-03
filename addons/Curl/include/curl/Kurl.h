#include <fstream>
#if defined(__APPLE__) || defined(__linux) 
#include <sys/socket.h>
#endif
#include <curl/curl.h>
#include <cstdlib>
#include <vector>

#include <iterator>
#include <roxlu/Roxlu.h>
#include <curl/Form.h>

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

int kurl_libcurl_debug_callback(CURL* handle, curl_infotype info, char* data, size_t nchars, void* user);

size_t kurl_callback_write_function(char* data, size_t size, size_t nmemb, void* userdata); // writes to file, or calls callback 

class KurlConnection;
typedef void (*kurl_cb_on_complete)(KurlConnection* c, void* userdata);
typedef void (*kurl_cb_on_write)(KurlConnection* c, char* data, size_t count, size_t nmemb, void* userdata);
typedef int (*kurl_cb_progress)(void* user, double dltotal, double dlnow, double ultotal, double ulnow);  // get info about the upload/donwloaded bytes, see Kurl::setProgresCallback + curl docs. Must return 0 to continue up/download, 1 stops everything :) 

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
  struct curl_httppost* http_post;
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
    void* completeData = NULL,
    kurl_cb_on_write writeCB = NULL, // when given you need to store the data yourself!
    void* writeData = NULL 
  );

  void setProgressCallback(kurl_cb_progress progresCB, void* progresUser);
  void setVerbose(bool verb);
  bool isVerbose();
  void printSupportedProtocols();
 private: 
  bool initProgressCallback(CURL* handle);
  bool setDebugCallback(CURL* handle);

 private:
  bool is_verbose;
  int still_running;
  CURLM* handle;
  std::vector<KurlConnection*> connections;

  kurl_cb_progress cb_progress; // callback that gets called when some network traffic occurs, see http://curl.haxx.se/libcurl/c/curl_easy_setopt.html#CURLOPTPROGRESSFUNCTION for more info
  void* cb_progress_user;
};

inline void Kurl::setProgressCallback(kurl_cb_progress progressCB, void* progressUser) {
  cb_progress = progressCB;
  cb_progress_user = progressUser;
}

inline void Kurl::setVerbose(bool verb) {
  is_verbose = verb;
}

inline bool Kurl::isVerbose() {
  return is_verbose;
}
