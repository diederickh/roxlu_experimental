#ifndef ROXLU_YOUTUBE_H
#define ROXLU_YOUTUBE_H

#include <string>
#include <jansson.h>
#include <youtube/YouTubeModel.h>
#include <youtube/YouTubeUpload.h>
#include <youtube/YouTubeUploadStart.h>
#include <youtube/YouTubeRefreshToken.h>
#include <youtube/YouTubeExchangeKey.h>
#include <youtube/YouTubeServerIPC.h>
#include <youtube/YouTubeClientIPC.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>

extern "C" {
#  include <uv.h>  
};

int youtube_on_upload_progress(double ultotal, double ulnow, void* user);             /* gets called by the `YouTubeUpload` handler when uploading a video; this is used to keep track how many bytes have been uploaded */
void youtube_on_upload_ready(YouTubeVideo video, void* user);                         /* gets called by the `YouTubeUpload` handler when uploading a video and when the video has been uploaded completely */

class YouTube {
 public:
  YouTube();
  ~YouTube();
  bool setup(std::string clientID, std::string clientSecret);                                     /* setup the YouTube API client, the client-id and secret can be found in the developers API console of google dev. */
  void setUploadProgressCallback(youtube_upload_progress_callback progressCB, void* user);        /* set the function which will be called while uploading; this function should return 0 to continue uploading, if  you want to stop uploading let it return 1 */
  void setUploadReadyCallback(youtube_upload_ready_callback readyCB, void* user);                 /* this function gets called when a video has been uploaded */
  void update();
  void print();                                                                                   /* print some debug info */
  bool hasAccessToken();                                                                          /* checks if there is a stored access token */
  bool exchangeAuthCode(std::string code);                                                        /* call this function with the auth-code you get when you granted the application access using the popup you get when clicking on the "GO" button form the html/indx.html page */
  bool addVideoToUploadQueue(YouTubeVideo video);                                                 /* add a new video to the upload queue; you need to call `update()` regurlarly which will make sure the queue will be processed. Make sure that you've set these fields: filename, datapath and title. You can also set the `video_resource_json`, if not set we will generate one */
 public:                                                                                          /* these are actually privates but are used by callbacks so they must be public */
  void checkUploadQueue();                                                                        /* check if there are videos in the upload queue */
  void checkAccessTokenTimeout();                                                                 /* checks if the access token timed out, and if so it will retrieve a new one */
  bool setRefreshToken(std::string rtoken);                                                       /* set the token we use to refresh the access token */
  bool setAccessToken(std::string atoken, uint64_t timeout);                                      /* set the access token and the timeout (in seconds) */
  bool refreshAccessToken();                                                                      /* sends a new request to get a new access token when the current token times out */
  std::string getRefreshToken();                                                                  /* get the refresh token we got when exchanging the auth code; */
  std::string getAccessToken();                                                                   /* get the current access token from the underlying model */
  uint64_t getTokenTimeout();                                                                     /* get the timestamp (in seconds) when the current access token will timeout, when this happens we will automatically get a new fresh access token */
 private:                                                                                         
  bool is_setup;                                                                                  
  uint64_t upload_check_timeout;                                                                  /* we check if there are new uploads every X-seconds, this is the timeout */
  uint64_t upload_check_delay;                                                                    /* the delay between each upload, in seconds */
  std::string client_id;                                                                          /* the client id of the application you created at the youtube developers site */
  std::string client_secret;                                                                      /* the clent secret of the application you created at the youtube developers site */
 public:                                                                                          
  YouTubeModel model;                                                                             /* the model stores the auth token; resumable video uploads etc.. */
  youtube_upload_progress_callback cb_upload_progress;                                            /* the callback which gets called while uploading; -must return 0 if you want to continue uploading -, if you want to stop uploading let it return 1 */
  youtube_upload_ready_callback cb_upload_ready;                                                  /* gets called when the video has been uploaded, see the YouTubeUpload class for the func decl. */
  void* cb_upload_progress_user;                                                                  /* gets passed into the progress callback */
  void* cb_upload_ready_user;                                                                     /* gets passed into the cb_upload_read() callback */
};

inline std::string YouTube::getRefreshToken() {
  return model.getRefreshToken();
}

inline std::string YouTube::getAccessToken() {
  return model.getAccessToken();
}

inline uint64_t YouTube::getTokenTimeout() {
  return model.getTokenTimeout();
}

inline bool YouTube::setRefreshToken(std::string rtoken) {

  if(!rtoken.size()) {
    RX_ERROR("Refresh token is empty.");
    return false;
  }

  model.setRefreshToken(rtoken);

  return true;
}

inline bool YouTube::setAccessToken(std::string atoken, uint64_t timeout) {

  if(!atoken.size()) {
    RX_ERROR("Access token is empty");
    return false;
  }

  if(!timeout) {
    RX_ERROR("Invalid token timeout");
    return false;
  }

  model.setAccessToken(atoken, timeout);
  return true;
}



#endif
