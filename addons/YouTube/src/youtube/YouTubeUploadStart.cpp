#include <youtube/YouTubeUploadStart.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <sstream>
#include <algorithm>

size_t youtube_upload_start_header_cb(void* ptr, size_t size, size_t nmemb, void* user) {
  size_t nbytes = size * nmemb;
  char* data = (char*) ptr;

  std::string header_line(data, nbytes);
  std::string header_line_lc;
  std::string to_find = "location";
  std::transform(header_line.begin(), header_line.end(), std::back_inserter(header_line_lc), ::tolower);

  if(header_line_lc.find(to_find) != std::string::npos) {
    header_line_lc = rx_string_replace(header_line, "Location: ", "");
    YouTubeUploadStart* ys = static_cast<YouTubeUploadStart*>(user);
    ys->location = header_line_lc.substr(0, header_line_lc.size()-2); // strip 0x0d,0x0a
  }

#if 0
  printf("-------------------------------------------------\n");
  for(size_t i = 0; i < nbytes; ++i) {
    printf("%c", data[i]);
  }
  printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
#endif

  return nbytes;
}

size_t youtube_upload_start_write_cb(char* ptr, size_t size, size_t nmemb, void* user) {
  YouTubeUploadStart* ys = static_cast<YouTubeUploadStart*>(user);
  size_t nbytes = size * nmemb;
  std::copy(ptr, ptr+size, std::back_inserter(ys->http_body));
  return nbytes;
}

// -----------------------------------------------------------------------------------

YouTubeUploadStart::YouTubeUploadStart() 
  :curl(NULL)
  ,http_code(0)
{
}

YouTubeUploadStart::~YouTubeUploadStart() {
  curl = NULL;
  http_code = 0;
  http_body.clear();
}

bool YouTubeUploadStart::start(YouTubeVideo video, std::string accessToken) {
  CURLcode res;
  struct curl_slist* headers = NULL;
  std::string length_header;
  std::string auth_header;
  std::stringstream ss_length;
  http_code = 0;
  http_body.clear();

  if(!video.bytes_total) {
    RX_ERROR("The bytes_total of the given video is invalid");
    return false;
  }

  if(!video.video_resource_json.size()) {
    RX_ERROR("Invalid json for upload start, length is 0");
    return false;
  }

  if(curl) {
    RX_ERROR("Currently already working....");
    return false;
  }

  curl = curl_easy_init();
  if(!curl) {
    RX_ERROR("Cannot initialize curl object");
    goto error;
  }
  
  res = curl_easy_setopt(curl, CURLOPT_URL, "https://www.googleapis.com/upload/youtube/v3/videos?uploadType=resumable&part=snippet%2Cstatus");
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);                               
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_POST, 1L);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, video.video_resource_json.c_str());
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
  YT_CURL_ERR(res);
  
  res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);                                      
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, youtube_upload_start_header_cb);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_WRITEHEADER, this);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, youtube_upload_start_write_cb);
  YT_CURL_ERR(res);

  res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
  YT_CURL_ERR(res);

  // add headers
  ss_length << "x-upload-content-length: " << video.bytes_total;
  length_header = ss_length.str();

  auth_header = "Authorization: Bearer " +accessToken;

  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, length_header.c_str());
  headers = curl_slist_append(headers, "x-upload-content-type: video/*");
  headers = curl_slist_append(headers, auth_header.c_str());
  res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);                              
  YT_CURL_ERR(res);

  // request!
  res = curl_easy_perform(curl);
  YT_CURL_ERR(res);

  // check result status
  res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  YT_CURL_ERR(res);

  // the start task shouldn't receive an body when everything is fine.
  if(http_body.size()) {
    parse();
  }

  if(http_code != 200) {
    RX_ERROR("Invalid http status: %ld", http_code);
    goto error;
  }
  
  // cleanup
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  curl = NULL;
  headers = NULL;
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


bool YouTubeUploadStart::parse() {

  if(!http_body.size()) {
    RX_ERROR("The received response is empty; cannot parse result of upload start action");
    return false;
  }
  
  if(http_code == 0) {
    RX_ERROR("We can only start parsing the http result when we got a valid http status code. make sure that you called start() before trying to parse the result");
    return false;
  }
  else if(http_code == 200) {
    RX_VERBOSE("Need to parse/handle 200 in upload start");
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
