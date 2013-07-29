/*

  # YouTubeUploadStatus

  This class will retrieve the current status of an upload in case uploading a
  video failed at some point.

 */
#ifndef ROXLU_YOUTUBE_UPLOAD_STATUS_H
#define ROXLU_YOUTUBE_UPLOAD_STATUS_H

#include <youtube/YouTubeTypes.h>
#include <curl/curl.h>
#include <string>

size_t youtube_upload_status_header_cb(void* ptr, size_t size, size_t nmemb, void* user);
size_t youtube_upload_status_write_cb(char* ptr, size_t size, size_t nmemb, void* user);

class YouTubeUploadStatus {
 public:
  YouTubeUploadStatus();
  ~YouTubeUploadStatus();
  bool checkStatus(YouTubeVideo video, std::string accessToken);                    /* queries youtube and asks how many bytes have been uploaded already */
  bool isPartiallyUploaded();                                                       /* returns true if some bytes were already uploaded */
  size_t getNumBytesUploaded();                                                     /* get the number of bytes which have been uploaded already; returns 0 when no bytes have been uploaded yet */
 public:
  CURL* curl;
  long status;                                                                      /* the http status code; 308 means that something has been uploaded already */
  int range_start;                                                                  /* extracted from the result headers, see documentation for more info: https://developers.google.com/youtube/v3/guides/using_resumable_upload_protocol#Check_Upload_Status */
  int range_end;                                                                    /* extracted from the result headers, see documentation for more info: https://developers.google.com/youtube/v3/guides/using_resumable_upload_protocol#Check_Upload_Status */
};

inline size_t YouTubeUploadStatus::getNumBytesUploaded() {
  if(range_end == 0) {
    return 0;
  }
  return range_end + 1;
}

inline bool YouTubeUploadStatus::isPartiallyUploaded() {
  return status == 308;
}

#endif
