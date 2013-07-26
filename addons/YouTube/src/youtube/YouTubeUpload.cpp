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

// -----------------------------------------------------------------------------------

YouTubeUpload::YouTubeUpload()
  :curl(NULL)
{
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

YouTubeUpload::~YouTubeUpload() {
}

bool YouTubeUpload::upload(YouTubeVideo video) {
  CURLcode res;
  struct curl_slist* headers = NULL;
  size_t fsize;

  if(curl) {
    RX_ERROR("Cannot upload twice w/o shutting down first.");
    return false;
  }
  
  //fp = fopen(rx_to_data_path(video.filename).c_str(), "rb");
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

  fp = fopen(filepath.c_str(), "rb");
  if(!fp) {
    RX_ERROR("Cannot open file: `%s`", video.filename.c_str());
    return false;
  }

  curl = curl_easy_init();
  if(!curl) {
    RX_ERROR("Cannot initialize curl object");
    goto error;
  }

  RX_VERBOSE("Uploading to: %s", video.upload_url.c_str());

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

  //  res = curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)rx_get_file_size(rx_to_data_path(video.filename))); 
  res = curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);
  YT_CURL_ERR(res);

  // add headers
  headers = curl_slist_append(headers, "Content-Type: video/*");
  res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);                              
  YT_CURL_ERR(res);
  
  // request
  res = curl_easy_perform(curl);
  YT_CURL_ERR(res);

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

