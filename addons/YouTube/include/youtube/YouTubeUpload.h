/*

  # YouTubeUpload
  
  This class performs the actual upload of a video; it's the second step in 
  the resumable upload process (see `YouTubeUploadStart` for the first step).

  @todo check status code from the result

 */
#ifndef ROXLU_YOUTUBE_UPLOAD_H
#define ROXLU_YOUTUBE_UPLOAD_H

#include <youtube/YouTubeModel.h>
#include <youtube/YouTubeTypes.h>
#include <curl/curl.h>
  
size_t youtube_upload_read_cb(void* ptr, size_t size, size_t nmemb, void* stream);

class YouTubeUpload {
 public:
  YouTubeUpload();
  ~YouTubeUpload();
  bool upload(YouTubeVideo video);                           /* call this to perform the upload request */
 public:
  CURL* curl;
  FILE* fp;
};

#endif
