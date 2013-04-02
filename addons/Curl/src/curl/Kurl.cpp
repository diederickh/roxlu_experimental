#include <curl/Kurl.h>

int kurl_libcurl_debug_callback(CURL* handle, 
                                curl_infotype info, 
                                char* data, 
                                size_t nchars, 
                                void* user) 
{
  std::string str;
  std::copy(data, data+nchars, std::back_inserter(str));
  RX_VERBOSE("%s", str.c_str());
  return 0;
}


// handles form posts
size_t kurl_callback_post_write_function(char* data, size_t size, size_t nmemb,  void* userdata) {
  if(nmemb <= 0) {
    return 0;
  }

  KurlConnectionPost* kc = static_cast<KurlConnectionPost*>(userdata);
  if(kc->write_callback) {
    kc->write_callback(kc, data, size, nmemb, kc->user);
    return size * nmemb;
  }

  return size * nmemb;

}

// handles downloads
size_t kurl_callback_download_write_function(char* data, size_t size, size_t nmemb, void* userdata) {
  if(nmemb <= 0) {
    return 0;
  }

  KurlConnectionDownload* kc = static_cast<KurlConnectionDownload*>(userdata);
  if(kc->write_callback) {
    kc->write_callback(kc, data, size, nmemb, kc->user);
    return size * nmemb;
  }

  kc->ofs.write(data, nmemb);
  if(!kc->ofs) {
    RX_ERROR(KURL_ERR_FILE_WRITE);
  }
  
  return size * nmemb;
}

// handles basic requests
size_t kurl_callback_get_write_function(char* data, size_t size, size_t nmemb, void* userdata) {
  if(nmemb <= 0) {
    return 0;
  }

  KurlConnectionGet* kg = static_cast<KurlConnectionGet*>(userdata);
  std::copy(data, data+(nmemb*size), std::back_inserter(kg->buffer));
  return size * nmemb;
}


// -----------------------------------
KurlConnection::KurlConnection(int type)
  :type(type)
  ,handle(NULL)
  ,complete_callback(NULL)
  ,user(NULL)
{
}

KurlConnection::~KurlConnection() {
}

// -----------------------------------
KurlConnectionDownload::KurlConnectionDownload()
  :KurlConnection(KURL_FILE_DOWNLOAD)
{
}

KurlConnectionDownload::~KurlConnectionDownload() {
}

// -----------------------------------
KurlConnectionPost::KurlConnectionPost()
  :KurlConnection(KURL_FORM)
{
}

KurlConnectionPost::~KurlConnectionPost() {
}

// -----------------------------------

KurlConnectionGet::KurlConnectionGet()
  :KurlConnection(KURL_GET)
  ,request_callback(NULL)
{
}

KurlConnectionGet::~KurlConnectionGet() {
}

// -----------------------------------

Kurl::Kurl() 
  :handle(NULL)
  ,still_running(0)
  ,cb_progress(NULL)
  ,cb_progress_user(NULL)
  ,is_verbose(false)
{
  handle = curl_multi_init();
  if(handle == NULL) {
    RX_ERROR(KURL_ERR_MULTI_INIT);
    ::exit(0);
  }
}

Kurl::~Kurl() {
  if(handle) {
    curl_multi_cleanup(handle);
  }
  handle = NULL;
}

void Kurl::update() {
  curl_multi_perform(handle, &still_running);  
  
  int q = 0;
  CURLMsg* msg = NULL;
  while((msg = curl_multi_info_read(handle, &q)) != NULL) {

    if(msg->msg == CURLMSG_DONE) {
      std::vector<KurlConnection*>::iterator it = connections.begin();
      while(it != connections.end()) {
        KurlConnection* c = *it;

        if(c->handle != msg->easy_handle) {
          ++it;
          continue;
        }

        // DOWNLOAD
        if(c->type == KURL_FILE_DOWNLOAD) {
          KurlConnectionDownload* cd = static_cast<KurlConnectionDownload*>(*it);
          if(cd->ofs.is_open()) {
            cd->ofs.close();
          }

          if(cd->complete_callback) {
            cd->complete_callback(c, c->user);
          }

          delete cd;
          connections.erase(it);
          break;
        }

        // FORM POSTS
        else if(c->type == KURL_FORM) {
          KurlConnectionPost* cp = static_cast<KurlConnectionPost*>(*it);
          curl_formfree(cp->http_post);

          if(cp->complete_callback) {
            cp->complete_callback(cp, cp->user);
          }

          delete cp;
          connections.erase(it);
          break;
        }

        // GET
        else if(c->type == KURL_GET) {

          KurlConnectionGet* kg = static_cast<KurlConnectionGet*>(*it);

          if(kg->request_callback) {
            kg->request_callback(kg->buffer, kg->user);
          }

          delete kg;
          connections.erase(it);
          break;
        }

        ++it;
      }
      curl_multi_remove_handle(handle, msg->easy_handle);
      curl_easy_cleanup(msg->easy_handle);
    }
  }
}

bool Kurl::download(std::string url,
                    std::string filename,
                    kurl_cb_on_complete completeCB, 
                    void* user,
                    kurl_cb_on_write writeCB ) 
{
  KurlConnectionDownload* c = new KurlConnectionDownload();
  c->handle = curl_easy_init();

  if(!c->handle) {
    RX_ERROR(KURL_ERR_EASY_INIT);
    delete c;
    return false;
  }
  
  if(!setDebugCallback(c->handle)) {
    delete c;
    return false;
  }

  if(writeCB == NULL) {
    c->ofs.open(filename.c_str(), std::ios::out | std::ios::binary);
    if(!c->ofs.is_open()) {
      RX_ERROR(KURL_ERR_DOWNLOAD_FILE, filename.c_str());
      return false;
    }
  }

  c->complete_callback = completeCB;
  c->user = user;

  curl_easy_setopt(c->handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(c->handle, CURLOPT_WRITEDATA, c);
  curl_easy_setopt(c->handle, CURLOPT_WRITEFUNCTION, kurl_callback_download_write_function);
  curl_easy_setopt(c->handle, CURLOPT_FOLLOWLOCATION, 1);
  
  curl_multi_add_handle(handle, c->handle);

  initProgressCallback(c->handle);

  connections.push_back(c);

  update(); // we directly start the request so 'isStillRunning()' returns a valid result

  return true;
}


bool Kurl::get(std::string url,
               kurl_cb_request completeCB, 
               void* user
)
{
  KurlConnectionGet* c = new KurlConnectionGet();
  c->handle = curl_easy_init();

  if(!c->handle) {
    RX_ERROR(KURL_ERR_EASY_INIT);
    delete c;
    return false;
  }
  
  if(!setDebugCallback(c->handle)) {
    delete c;
    return false;
  }

  c->request_callback = completeCB;
  c->user = user;

  curl_easy_setopt(c->handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(c->handle, CURLOPT_WRITEDATA, c);
  curl_easy_setopt(c->handle, CURLOPT_WRITEFUNCTION, kurl_callback_get_write_function);
  curl_easy_setopt(c->handle, CURLOPT_FOLLOWLOCATION, 1);
  
  curl_multi_add_handle(handle, c->handle);

  initProgressCallback(c->handle);

  connections.push_back(c);

  update(); // we directly start the request so 'isStillRunning()' returns a valid result

  return true;
}


bool Kurl::post(Form& f,
                kurl_cb_on_complete completeCB, 
                void* user,
                kurl_cb_on_write writeCB
) 
{

  if(!f.isSetup()) {
    RX_WARNING(KURL_ERR_FORM_NOT_SETUP);
    return false;
  }

  CURLcode result;
  CURLFORMcode form_result;
  struct curl_httppost* post_curr = NULL;
  struct curl_httppost* post_last = NULL;

  KurlConnectionPost* c = new KurlConnectionPost();

  c->handle = curl_easy_init();
  if(!c->handle) {
    RX_ERROR(KURL_ERR_EASY_INIT);
    delete c;
    return false;
  }

  c->complete_callback = completeCB;
  c->write_callback = writeCB;
  c->user = user;

  if(!setDebugCallback(c->handle)) {
    delete c;
    return false;
  }

  for(std::vector<FormInput>::iterator it = f.inputs.begin(); it != f.inputs.end(); ++it) {
    form_result = curl_formadd(&post_curr, &post_last, CURLFORM_COPYNAME, (*it).name.c_str(), 
                               CURLFORM_COPYCONTENTS, (*it).value.c_str(), CURLFORM_END);
    RETURN_CURLCODE(form_result, KURL_ERR_FORM_INPUT, false, c);
  }

  for(std::vector<FormFile>::iterator it = f.files.begin(); it != f.files.end(); ++it) {
    form_result = curl_formadd(&post_curr, &post_last, CURLFORM_COPYNAME, (*it).name.c_str(), 
                               CURLFORM_FILE, (*it).filepath.c_str(), CURLFORM_END);
    RETURN_CURLCODE(form_result, KURL_ERR_FORM_FILE, false, c);
  }
  
  result = curl_easy_setopt(c->handle, CURLOPT_URL, f.url.c_str());
  RETURN_CURLCODE(result, KURL_ERR_FORM_URL, false, c);
  
  result = curl_easy_setopt(c->handle, CURLOPT_HTTPPOST, post_curr);
  RETURN_CURLCODE(result, KURL_ERR_FORM_POSTOBJ, false, c);

  result = curl_easy_setopt(c->handle, CURLOPT_WRITEFUNCTION, kurl_callback_post_write_function);
  RETURN_CURLCODE(result, KURL_ERR_FORM_WRITE_FUNCTION, false, c);

  result = curl_easy_setopt(c->handle, CURLOPT_WRITEDATA, c);
  RETURN_CURLCODE(result, KURL_ERR_FORM_WRITE_DATA, false, c);

  if(is_verbose) {
    result = curl_easy_setopt(c->handle, CURLOPT_VERBOSE, TRUE);
    RETURN_CURLCODE(result, KURL_ERR_FORM_VERBOSE, false, c);
  }

  CURLMcode mcode = curl_multi_add_handle(handle, c->handle);
  RETURN_CURLFORMCODE(mcode, KURL_ERR_FORM_ADD_HANDLE, false, c);

  c->http_post = post_curr;

  initProgressCallback(c->handle);

  connections.push_back(c);

  update(); // we directly start the request so 'isStillRunning()' returns a valid result

  return true;
}


bool Kurl::initProgressCallback(CURL* handle) {
  if(!cb_progress) {
    return false;
  }

  if(!handle) {
    return false;
  }

  char* fake = NULL;
  CURLcode result;

  result = curl_easy_setopt(handle, CURLOPT_NOPROGRESS, false);
  RETURN_CURLCODE(result, KURL_ERR_PROG_ENABLE, false, fake);

  result = curl_easy_setopt(handle, CURLOPT_PROGRESSFUNCTION, cb_progress);
  RETURN_CURLCODE(result, KURL_ERR_PROG_SET, false, fake);

  curl_easy_setopt(handle, CURLOPT_PROGRESSDATA, cb_progress_user);
  RETURN_CURLCODE(result, KURL_ERR_PROG_DATA, false, fake);
  
  return true;
}

bool Kurl::setDebugCallback(CURL* handle) {
  if(!is_verbose) {
    return true;
  }

  CURLcode result = curl_easy_setopt(handle, CURLOPT_DEBUGFUNCTION, kurl_libcurl_debug_callback);
  if(result != CURLE_OK) {
    RX_ERROR(KURL_ERR_DEBUG_FUNC);
    return false;
  }
  return true;
}

void Kurl::printSupportedProtocols() {
  curl_version_info_data* info = curl_version_info(CURLVERSION_NOW);
  RX_VERBOSE(KURL_VERB_VERSION, info->version);
  int i = 0;
  while(info->protocols[i] != NULL) {
    RX_VERBOSE("- %s", info->protocols[i]);
    ++i;
  }
}
