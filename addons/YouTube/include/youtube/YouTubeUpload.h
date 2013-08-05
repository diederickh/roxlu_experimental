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
#include <youtube/YouTubeUploadStatus.h>
#include <curl/curl.h>
#include <string>
  
size_t youtube_upload_read_cb(void* ptr, size_t size, size_t nmemb, void* stream);
int youtube_upload_progress_cb(void* ptr, double dltotal, double dlnow, double ultotal, double ulnow);       /* gets called when we are uploading the video; ultotal may be 0 at start */
// size_t youtube_upload_write_cb(char* ptr, size_t size, size_t nmemb, void* user);

typedef int(*youtube_upload_progress_callback)(double ultotal, double ulnow, void* user);                    /* if you pass this into upload(), you will be notified by the upload state. This function must return `0` if you want to continue the upload or `1` if you want to stop uploading. ultotal might be 0 when the upload starts */

class YouTubeUpload {
 public:
  YouTubeUpload();
  ~YouTubeUpload();
  bool upload(YouTubeVideo video, std::string accessToken, youtube_upload_progress_callback progressCB, void* user);                   /* call this to perform the upload request, video should contain: filename, datapath, bytes_uploaded, upload_url. When bytes_uploaded > 0, we will try to resume a previously upload attempt  */
 public:
  CURL* curl;
  FILE* fp;
  void* cb_user;
  youtube_upload_progress_callback cb_progress;
};

#endif
