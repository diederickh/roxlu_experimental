#include <youtube/YouTubeUploadStatus.h>
#include <pcre/PCRE.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>

size_t youtube_upload_status_header_cb(void* ptr, size_t size, size_t nmemb, void* user) {
  YouTubeUploadStatus* us = static_cast<YouTubeUploadStatus*>(user);
  size_t nbytes = size * nmemb;
  char* data = (char*)ptr;

  if(RE("Range: bytes=([0-9]+)-([0-9]+)", RE_Options(PCRE_CASELESS)).PartialMatch(data, &us->range_start, &us->range_end)) {
    
  }

  return nbytes;
}

// -----------------------------------------------------------------------------------

YouTubeUploadStatus::YouTubeUploadStatus() 
  :curl(NULL)
  ,range_start(0)
  ,range_end(0)
  ,status(0)
{
}

YouTubeUploadStatus::~YouTubeUploadStatus() {
}

bool YouTubeUploadStatus::checkStatus(YouTubeVideo video, std::string accessToken) {
  CURLcode res;
  struct curl_slist* headers = NULL;
  size_t fsize = 0;
  std::stringstream content_range_ss;
  std::string content_range;
  std::string auth_header;

  // validate
  // ----------------------------------
  if(!video.upload_url.size()) {
    RX_ERROR("The upload url is empty; so cannot check the status");
    return false;
  }

  std::string filepath = video.filename;
  if(video.datapath) {
    filepath = rx_to_data_path(filepath);
  }

  if(!rx_file_exists(filepath)) {
    RX_ERROR("Cannot find the video file: `%s`", filepath.c_str());
    return false;
  }
  
  fsize = rx_get_file_size(filepath);
  if(!fsize) {
    RX_ERROR("Cannot get the filesize fo the given file: `%s`", filepath.c_str());
    return false;
  }

  // send request
  // ----------------------------------
  curl = curl_easy_init();
  if(!curl) {
    RX_ERROR("Cannot initialize curl handle");
    return false;
  }

  res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);                                      
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_URL, video.upload_url.c_str());
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_HEADER, 1L); // output headers
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, youtube_upload_status_header_cb);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_WRITEHEADER, this);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,0);
  YT_CURL_ERR(res);

  // add header: Content-Range
  content_range_ss << "Content-Range: bytes */" << video.bytes_total;
  content_range = content_range_ss.str();
  headers = curl_slist_append(headers, content_range.c_str());

  // add header: Authorization: Bearer: {AUTH_TOKEN} 
  auth_header = "Authorization: Bearer " +accessToken;
  headers = curl_slist_append(headers, auth_header.c_str());

  // add header: Content-Length: 0
   headers = curl_slist_append(headers, "Content-Length: 0");
   headers = curl_slist_append(headers, "Accept:"); // disable accept

  res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  YT_CURL_ERR(res);

  // request!
  res = curl_easy_perform(curl);
  YT_CURL_ERR(res);

  // check result status (308 meaning it's a partial upload)n
  res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
  YT_CURL_ERR(res);

  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  headers = NULL;
  curl = NULL;
  return true;

 error:
  if(curl) {
    curl_easy_cleanup(curl);
    curl = NULL;
  }
  if(headers) {
    curl_slist_free_all(headers);
    headers = NULL;
  }
  return false;
}
