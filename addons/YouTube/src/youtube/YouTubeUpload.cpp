#include <stdio.h>
#include <youtube/YouTubeUpload.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>


size_t youtube_upload_read_cb(void* ptr, size_t size, size_t nmemb, void* user) {
  YouTubeUpload* up = static_cast<YouTubeUpload*>(user);
  size_t retcode;
  curl_off_t nread;

  retcode = fread(ptr, size, nmemb, up->fp);
  nread = (curl_off_t)retcode;

  RX_VERBOSE("Read %" CURL_FORMAT_CURL_OFF_T " bytes from file", nread);
  return retcode;
}


int youtube_upload_progress_cb(void* ptr, double dltotal, double dlnow, double ultotal, double ulnow) {
  YouTubeUpload* up = static_cast<YouTubeUpload*>(ptr);
  if(up->cb_progress) {
    return up->cb_progress(ultotal, ulnow, up->cb_user);
  }
  return 0; 
}

size_t youtube_upload_write_cb(char* ptr, size_t size, size_t nmemb, void* user) {
  YouTubeUpload* up = static_cast<YouTubeUpload*>(user);
  size_t num_bytes = size * nmemb;
  std::copy(ptr, ptr+num_bytes, std::back_inserter(up->http_body));

  std::string str(ptr, num_bytes);
  RX_VERBOSE("%s", str.c_str());
  RX_VERBOSE("Body contains; %ld chars", up->http_body.size());
  return num_bytes;
}

// -----------------------------------------------------------------------------------

YouTubeUpload::YouTubeUpload()
  :curl(NULL)
  ,cb_progress(NULL)
  ,cb_user(NULL)
  ,cb_ready(NULL)
  ,http_code(0)
{
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

YouTubeUpload::~YouTubeUpload() {
  cb_progress = NULL;
  cb_user = NULL;
  cb_ready = NULL;
  http_code = 0;
  http_body = "";
  errors.clear();
}

bool YouTubeUpload::upload(YouTubeVideo video, std::string accessToken, 
                           youtube_upload_progress_callback progressCB, 
                           youtube_upload_ready_callback readyCB,
                           void* user) 
{
  YouTubeUploadStatus upload_status;
  CURLcode res;
  struct curl_slist* headers = NULL;
  size_t fsize = 0;
  http_code = 0;
  http_body = "";

  cb_progress = progressCB;
  cb_ready = readyCB;
  cb_user = user;

  // validate
  // ------------------
  if(curl) {
    RX_ERROR("Cannot upload twice w/o shutting down first.");
    return false;
  }

  std::string filepath = video.filename;
  if(video.datapath) {
    filepath = rx_to_data_path(filepath);
  }

  if(!video.upload_url.size()) {
    RX_ERROR("Trying to upload a video which doesn't have a correct upload url. video.id: %d", video.id);
    return false;
  }

  if(!rx_file_exists(filepath)) {
    RX_ERROR("Cannot find the file to upload: `%s`", filepath.c_str());
    return false;
  }

  fsize = rx_get_file_size(filepath);
  if(!fsize) {
    RX_ERROR("Cannot get the filesize for: `%s`", filepath.c_str());
    return false;
  }

  // resumable upload
  // ------------------------
  if(video.bytes_uploaded > 0) {
    if(!upload_status.checkStatus(video, accessToken)) {
      RX_ERROR("Cannot retrieve status information for video which is necessary to check how many bytes have been uploaded already. This will fail.");
      return false;
    }
    else {
      fsize = fsize - upload_status.getNumBytesUploaded();
    }
  }

  // upload
  // ------------------------
  fp = fopen(filepath.c_str(), "rb");
  if(!fp) {
    RX_ERROR("Cannot open file: `%s`", video.filename.c_str());
    return false;
  }

  if(upload_status.isPartiallyUploaded()) {
    RX_VERBOSE("Skip some bytes from FP");
    if(fseek(fp, upload_status.getNumBytesUploaded(), SEEK_SET) != 0) {
      RX_ERROR("Error skipping some bytes for the upload");
    }
  }

  curl = curl_easy_init();
  if(!curl) {
    RX_ERROR("Cannot initialize curl object");
    goto error;
  }

  RX_VERBOSE("Uploading to: %s, filesize: %ld", video.upload_url.c_str(), fsize);

  // default options
  res = curl_easy_setopt(curl, CURLOPT_URL, video.upload_url.c_str());                    
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);                               
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);                                       
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_PUT, 1L);                                          
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_READFUNCTION, youtube_upload_read_cb);             
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_READDATA, this);                                   
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);                                      
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, youtube_upload_write_cb);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
  YT_CURL_ERR(res);

  if(cb_progress) {
    res = curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, youtube_upload_progress_cb);
    YT_CURL_ERR(res);

    res = curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
    YT_CURL_ERR(res);

    res = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    YT_CURL_ERR(res);
  }  

  // add headers
  headers = curl_slist_append(headers, "Content-Type: video/*");

  // when we have a partial upload; we need to add some more headers
  if(upload_status.isPartiallyUploaded()) {
    RX_VERBOSE("Added headers for partial upload");
    std::stringstream ss; 
    ss << "Content-Range: bytes " << upload_status.range_end + 1 << "-" << (video.bytes_total - 1) << "/" << video.bytes_total;
    std::string ss_str = ss.str();
    headers = curl_slist_append(headers, ss_str.c_str());
  }

  res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);                              
  YT_CURL_ERR(res);
  
  // request
  res = curl_easy_perform(curl);
  YT_CURL_ERR(res);

  // check result
  res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  YT_CURL_ERR(res);

  if(!parse()) {
    RX_ERROR("Cannot parse the response of the received json.");
  }

  if(http_code != 200) {
    RX_ERROR("Could not upload video; got a http status of: %ld", http_code);
    goto error;
  }

  // ready callback
  if(cb_ready) {
    cb_ready(video, cb_user);
  }

  curl_easy_cleanup(curl);
  fclose(fp);
  curl_slist_free_all(headers);
  fp = NULL;
  curl = NULL;
  headers = NULL;
  return true;

 error:
  if(curl) {
    curl_easy_cleanup(curl);
    curl = NULL;
  }
  if(fp) {
    fclose(fp);
    fp = NULL;
  }
  if(headers) {
    curl_slist_free_all(headers);
    headers = NULL;
  }
  return false;
}

bool YouTubeUpload::parse() {

  if(!http_body.size()) {
    RX_ERROR("The body of the http response is empty.");
    return false;
  }

  if(http_code == 0) {
    RX_ERROR("Cannot parse the request, because we need the http_code; make sure to set it before calling parse()");
    return false;
  }

  if(http_code == 200) {
    
  }
  else if(http_code >= 400) {
    std::vector<YouTubeError> errors;
    if(!youtube_parse_errors(http_body, errors)) {
      RX_ERROR("Cannot parse the error json in the upload start");
      return false;
    }

    for(std::vector<YouTubeError>::iterator it = errors.begin(); it != errors.end(); ++it) {
      (*it).print();
    }

  }

  return true;
}
