#include <curl/Kurl.h>

int kurl_libcurl_debug_callback(CURL* handle, 
                                curl_infotype info, 
                                char* data, 
                                size_t nchars, 
                                void* user) 
{
  std::string str;
  std::copy(data, data+nchars, std::back_inserter(str));
  RX_VERBOSE(("%s", str.c_str()));
  return 0;
}

size_t kurl_callback_write_function(char* data, size_t size, size_t nmemb, void* userdata) {
  if(nmemb > 0) {
    KurlConnection* kc = static_cast<KurlConnection*>(userdata);
    if(kc->write_callback) {
      kc->write_callback(kc, data, size, nmemb, kc->write_data);
    }
    else {
      if(kc->type == KURL_FILE_DOWNLOAD) {
        kc->ofs.write(data, nmemb);
        if(!kc->ofs) {
          RX_ERROR(("ERROR: cannot write to file in Kurl."));
        }
      }
      else if(kc->type == KURL_FORM) {
        if(kc->kurl->isVerbose()) {
          for(size_t i = 0; i < nmemb; ++i) {
            printf("%c", data[i]);
          }
        }
      }
      else {
        RX_VERBOSE(("VERBOSE: no callback for this type of connection (Kurl) "));
      }
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
  ,http_post(NULL)
{
  //  printf("> KurlConnection::KurlConnection()\n");
}

KurlConnection::~KurlConnection() {
  //  printf("< KurlConnection::~KurlConnection()\n");
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
    RX_ERROR(("ERROR: cannot curl_multi_init"));
    ::exit(0);
  }
  curl_version_info_data* info = curl_version_info(CURLVERSION_NOW);
  RX_VERBOSE(("CURL version: %s", info->version));
  int i = 0;
  while(info->protocols[i] != NULL) {
    RX_VERBOSE(("- %s", info->protocols[i]));
      ++i;
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
          else if(c->type == KURL_FORM && c->http_post != NULL) {
            curl_formfree(c->http_post);
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
    RX_ERROR(("ERROR: cannot curl_easy_init()"));
    delete c;
    return false;
  }
  
  if(!setDebugCallback(c->handle)) {
    delete c;
    return false;
  }

  // Only open a file when no writeCB is given
  if(writeCB == NULL) {
    c->ofs.open(filename, std::ios::out | std::ios::binary);
    if(!c->ofs.is_open()) {
      RX_ERROR(("ERROR: Kurl::download, ofs.is_open() failed\n"));
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

  initProgressCallback(c->handle);

  connections.push_back(c);
  return true;
}


bool Kurl::post(Form& f,
                kurl_cb_on_complete completeCB, 
                void* completeData,
                kurl_cb_on_write writeCB, 
                void* writeData
) {
  if(!f.isSetup()) {
    RX_WARNING(("WARNING: given form is not setup/complete."));
    return false;
  }

  CURLcode result;
  CURLFORMcode form_result;
  struct curl_httppost* post_curr = NULL;
  struct curl_httppost* post_last = NULL;

  KurlConnection* c = new KurlConnection();

  c->handle = curl_easy_init();
  if(!c->handle) {
    RX_ERROR(("ERROR: cannot curl_easy_init() for form.\n"));
    delete c;
    return false;
  }

  c->type = KURL_FORM;
  c->complete_data = completeData;
  c->complete_callback = completeCB;
  c->write_callback = writeCB;
  c->write_data = writeData;

  if(!setDebugCallback(c->handle)) {
    delete c;
    return false;
  }

  for(std::vector<FormInput>::iterator it = f.inputs.begin(); it != f.inputs.end(); ++it) {
    form_result = curl_formadd(&post_curr, &post_last, CURLFORM_COPYNAME, (*it).name.c_str(), 
                               CURLFORM_COPYCONTENTS, (*it).value.c_str(), CURLFORM_END);
    RETURN_CURLCODE(form_result, "ERROR: cannot add input to form.\n", false, c);
  }

  for(std::vector<FormFile>::iterator it = f.files.begin(); it != f.files.end(); ++it) {
    form_result = curl_formadd(&post_curr, &post_last, CURLFORM_COPYNAME, (*it).name.c_str(), 
                               CURLFORM_FILE, (*it).filepath.c_str(), CURLFORM_END);
    RETURN_CURLCODE(form_result, "ERROR: cannot add file to form.\n", false, c);
  }
  
  result = curl_easy_setopt(c->handle, CURLOPT_URL, f.url.c_str());
  RETURN_CURLCODE(result, "ERROR: Failed settings the url for the form.\n", false, c);
  
  result = curl_easy_setopt(c->handle, CURLOPT_HTTPPOST, post_curr);
  RETURN_CURLCODE(result, "ERROR: Failed to create curlopt_httppost.\n", false, c);

  result = curl_easy_setopt(c->handle, CURLOPT_WRITEFUNCTION, kurl_callback_write_function);
  RETURN_CURLCODE(result, "ERROR: Failed to set curlopt_writefunction for form.\n", false, c);

  result = curl_easy_setopt(c->handle, CURLOPT_WRITEDATA, c);
  RETURN_CURLCODE(result, "ERROR: Failed to set curopt_writedata for form.\n", false, c);

  if(is_verbose) {
    result = curl_easy_setopt(c->handle, CURLOPT_VERBOSE, TRUE);
    RETURN_CURLCODE(result, "ERROR: Failed to set verbose option for form.\n", false, c);
  }


  curl_multi_add_handle(handle, c->handle); // @todo add error check

  c->http_post = post_curr;

  initProgressCallback(c->handle);

  connections.push_back(c);

  //result = curl_easy_setopt(c->handle, CURLOPT_TIMEOUT, 1000);
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
  RETURN_CURLCODE(result, "ERROR: Failed to enable progress monitoring", false, fake);

  result = curl_easy_setopt(handle, CURLOPT_PROGRESSFUNCTION, cb_progress);
  RETURN_CURLCODE(result, "ERROR: Failed to set progress function", false, fake);

  curl_easy_setopt(handle, CURLOPT_PROGRESSDATA, cb_progress_user);
  RETURN_CURLCODE(result, "ERROR: Failed to set progress data", false, fake);
  
  return true;
}

bool Kurl::setDebugCallback(CURL* handle) {
  if(!is_verbose) {
    return true;
  }

  CURLcode result = curl_easy_setopt(handle, CURLOPT_DEBUGFUNCTION, kurl_libcurl_debug_callback);
  if(result != CURLE_OK) {
    RX_ERROR(("ERROR: failed to set debug callback"));
    return false;
  }
  return true;
}
