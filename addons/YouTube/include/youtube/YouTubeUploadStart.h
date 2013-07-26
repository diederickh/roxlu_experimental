/*
  # YouTubeUploadStart

  We're using the resumable upload feature of the YouTube Data API, which 
  is a 2 (or multiple) step process. First we retrieve a unique upload url 
  for the video which is done by this class; then we use `YouTubeUpload` to 
  perform the actual upload.

 */
#ifndef ROXLU_YOUTUBE_UPLOAD_START_H
#define ROXLU_YOUTUBE_UPLOAD_START_H

#include <youtube/YouTubeModel.h>
#include <youtube/YouTubeTypes.h>
#include <curl/curl.h>
#include <string>
#include <iterator>

size_t youtube_upload_start_header_cb(void* ptr, size_t size, size_t nmemb, void* user);

class YouTubeUploadStart {
 public:
  YouTubeUploadStart();
  ~YouTubeUploadStart();
  bool start(YouTubeVideo video, std::string accessToken);          /* call this to retrieve the URL that you should use to upload the video to */
  std::string getLocation();                                        /* get the location to which we need to upload the video */
 public:
  CURL* curl;
  std::string location;                                             /* the parsed location to which we should upload the video */
};

inline std::string YouTubeUploadStart::getLocation() {
  return location;
}
#endif
