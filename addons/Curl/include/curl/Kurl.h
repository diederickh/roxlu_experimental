#ifndef ROXLU_KURL_H
#define ROXLU_KURL_H

#include <fstream>

#if defined(__APPLE__) || defined(__linux) 
#  include <sys/socket.h>
#endif

#include <curl/curl.h>
#include <cstdlib>
#include <vector>
#include <iterator>
#include <string>
#include <roxlu/Roxlu.h>
#include <curl/Form.h>

#define KURL_ERR_MULTI_INIT "Cannot curl_multi_init"
#define KURL_ERR_EASY_INIT "Cannot initialize a curl handle with curl_easy_init()"
#define KURL_ERR_DOWNLOAD_FILE "Cannot open the file: %s"
#define KURL_ERR_FILE_WRITE "Cannot write to file in Kurl."
#define KURL_ERR_FORM_NOT_SETUP "Given form is not setup/complete."
#define KURL_ERR_FORM_INPUT "Cannot add input to form."
#define KURL_ERR_FORM_FILE "Cannot add file to form."
#define KURL_ERR_FORM_URL "Failed settings the url for the form."
#define KURL_ERR_FORM_POSTOBJ "Failed to create curlopt_httppost."
#define KURL_ERR_FORM_WRITE_FUNCTION  "Failed to set curlopt_writefunction for form."
#define KURL_ERR_FORM_WRITE_DATA "Failed to set curopt_writedata for form."
#define KURL_ERR_FORM_VERBOSE "Failed to set verbose option for form."
#define KURL_ERR_FORM_ADD_HANDLE "Failed to add a multi handle"
#define KURL_ERR_PROG_ENABLE "Failed to enable progress monitoring"
#define KURL_ERR_PROG_SET "Failed to set progress function"
#define KURL_ERR_PROG_DATA "Failed to set progress data"
#define KURL_ERR_DEBUG_FUNC "Failed to set debug callback"
#define KURL_VERB_VERSION "CURL version: %s"

#define RETURN_CURLCODE(result, msg, rvalue, ptr )      \
  if(result != CURLE_OK) {                              \
    RX_ERROR(msg);                                    \
    if(ptr != NULL) {                                   \
        delete ptr;                                     \
        ptr = NULL;                                     \
    }                                                   \
    return rvalue;                                      \
  }                                          

#define RETURN_CURLFORMCODE(result, msg, rvalue, ptr) \
  if(result != 0) {                                   \
    RX_ERROR(msg);                                  \
    if(ptr != NULL) {                                 \
        delete ptr;                                   \
        ptr = NULL;                                   \
    }                                                 \
    return rvalue;                                    \
 }

int kurl_libcurl_debug_callback(CURL* handle, curl_infotype info, char* data, size_t nchars,  void* user);
size_t kurl_callback_download_write_function(char* data, size_t size, size_t nmemb,  void* userdata);                /* writes to file, or calls callback  */
size_t kurl_callback_post_write_function(char* data, size_t size, size_t nmemb, void* userdata);                     /* gets called when a post gets some data; we pass through of do nothing */
size_t kurl_callback_get_write_function(char* data, size_t size, size_t nmemb, void* userdata);                      /* handles a basic http request, stores data into a buffer */

class KurlConnection;

typedef void (*kurl_cb_request)(std::string& buffer, void* userdata);
typedef void (*kurl_cb_on_complete)(KurlConnection* c, void* userdata);
typedef void (*kurl_cb_on_write)(KurlConnection* c, char* data, size_t count, size_t nmemb, void* userdata);
typedef int (*kurl_cb_progress)(void* user, double dltotal, double dlnow, double ultotal, double ulnow);  // get info about the upload/donwloaded bytes, see Kurl::setProgresCallback + curl docs. Must return 0 to continue up/download, 1 stops everything :) 

enum KurlTypes {
  KURL_NONE,
  KURL_FILE_DOWNLOAD,
  KURL_FORM,
  KURL_GET
};

class Kurl;

struct KurlConnection {
  KurlConnection(int type = KURL_NONE);
  ~KurlConnection();
  CURL* handle;
  
  int type;
  Kurl* kurl;

  kurl_cb_on_complete complete_callback;                                           /* gets called when the connection is 'ready' */
  kurl_cb_on_write write_callback;                                                 /* gets called when there is new data to be written */
  void* user; /* user data */
};

struct KurlConnectionDownload : public KurlConnection {
  KurlConnectionDownload();
  ~KurlConnectionDownload();
  std::ofstream ofs;                                                               /* we can write data directory to a file */
};

struct KurlConnectionPost : public KurlConnection {
  KurlConnectionPost();
  ~KurlConnectionPost();
  struct curl_httppost* http_post;
};

struct KurlConnectionGet : public KurlConnection {
  KurlConnectionGet();
  ~KurlConnectionGet();
  kurl_cb_request request_callback;
  std::string buffer;
};

class Kurl {
public:
  Kurl();
  ~Kurl();
  void update();

  bool download(std::string url,
                std::string filename,
                kurl_cb_on_complete completeCB, 
                void* user = NULL,
                kurl_cb_on_write writeCB = NULL);                                                 /* when given you need to store the data yourself! */
  
  bool get(std::string url, 
           kurl_cb_request requestCB, 
           void* user);

  bool post(Form& fm,
            kurl_cb_on_complete completeCB, 
            void* user = NULL,
            kurl_cb_on_write writeCB = NULL);                                                /* when given you need to store the data yourself! */


  void setProgressCallback(kurl_cb_progress progresCB, void* progresUser);
  void setVerbose(bool verb);
  bool isVerbose();
  void printSupportedProtocols();
  bool isStillRunning();
 private: 
  bool initProgressCallback(CURL* handle);
  bool setDebugCallback(CURL* handle);

 private:
  bool is_verbose;
  int still_running;
  CURLM* handle;
  std::vector<KurlConnection*> connections;

  kurl_cb_progress cb_progress;                                                    /* callback that gets called when some network traffic occurs, see http://curl.haxx.se/libcurl/c/curl_easy_setopt.html#CURLOPTPROGRESSFUNCTION for more info */
  void* cb_progress_user;                                                          /* user data, gets passed into the progress callback */
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

inline bool Kurl::isStillRunning() {
  return still_running;
}

#endif
