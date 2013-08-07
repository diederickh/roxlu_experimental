#include <youtube/YouTube.h>
#include <fstream>

int youtube_on_upload_progress(double ultotal, double ulnow, void* user) {
  YouTubeUploadInfo* ui = static_cast<YouTubeUploadInfo*>(user);
  ui->yt->model.setVideoBytesUploaded(ui->video_id, ulnow);
  
  if(ui->yt->cb_upload_progress) {
    return ui->yt->cb_upload_progress(ultotal, ulnow, ui->yt->cb_upload_progress_user);
  }
  else {     
    RX_VERBOSE(">>> %f / %f (id = %d)", ulnow, ultotal, ui->video_id);
    return 0;
  }
}

void youtube_on_upload_ready(YouTubeVideo video, void* user) {
  RX_VERBOSE("On upload ready cb.");

  YouTubeUploadInfo* ui = static_cast<YouTubeUploadInfo*>(user);
  YouTube* yt = ui->yt; 

  if(yt->cb_upload_ready) {
    yt->cb_upload_ready(video, yt->cb_upload_ready_user);
  }

}

// ----------------------------------------------------------------------

YouTube::YouTube() 
  :is_setup(false)
  ,upload_check_timeout(0)
  ,upload_check_delay(4) 
  ,cb_upload_progress(NULL)
  ,cb_upload_progress_user(NULL)
  ,cb_upload_ready(NULL)
  ,cb_upload_ready_user(NULL)
{
}

YouTube::~YouTube() {
  cb_upload_progress = NULL;
  cb_upload_progress_user = NULL;
  cb_upload_ready = NULL;
  cb_upload_ready_user = NULL;
  is_setup = false;
  upload_check_timeout = 0;
  upload_check_delay = 0;
}

bool YouTube::setup(std::string clientID, std::string clientSecret) {
  client_id = clientID; // oauth consumer key
  client_secret = clientSecret; // oauth consumer secret
  is_setup = true;
  print();
  return true;
}

void YouTube::setUploadProgressCallback(youtube_upload_progress_callback progressCB, void* user) {
  cb_upload_progress = progressCB;
  cb_upload_progress_user = user;
}

void YouTube::setUploadReadyCallback(youtube_upload_ready_callback readyCB, void* user) {
  cb_upload_ready = readyCB;
  cb_upload_ready_user = user;
}

bool YouTube::hasAccessToken() {
  return model.getAccessToken().size();
}

void YouTube::update() {
  checkAccessTokenTimeout();
  checkUploadQueue();
}

bool YouTube::exchangeAuthCode(std::string code) {

  YouTubeExchangeKey ex_key;
  if(!ex_key.exchange(code, client_id, client_secret)) { 
    return false;
  }
  
  if(!setRefreshToken(ex_key.getRefreshToken())) {
    return false;
  }

  if(!setAccessToken(ex_key.getAccessToken(), ex_key.getTokenTimeOut())) {
    return false;
  }

  return true;
}

void YouTube::checkAccessTokenTimeout() {
  // @todo when we send the request to refresh the access token we need to set a flag
  // to make sure only one requests is done at a time
  uint64_t now = rx_time();
  uint64_t token_timeout = model.getTokenTimeout();

  if(now >= token_timeout) {
    RX_VERBOSE("token has timed out");
    if(!refreshAccessToken()) {
      RX_ERROR("Error while trying to refresh the access token");
    }
  }
}

bool YouTube::refreshAccessToken() {
  YouTubeRefreshToken rt;

  if(!rt.refresh(client_id, client_secret, model.getRefreshToken())) {
    return false;
  }
  
  if(!setAccessToken(rt.getAccessToken(), rt.getTokenTimeOut())) {
    return false;
  }

  return true;
}

bool YouTube::addVideoToUploadQueue(YouTubeVideo video) {

  if(!model.addVideoToUploadQueue(video)) {
    RX_ERROR("Cannot add the video to the upload queue. %s", video.filename.c_str());
    return false;
  }

  return true;
}

void YouTube::checkUploadQueue() {

  uint64_t now = rx_time();
  if(now > upload_check_timeout) {

    upload_check_timeout = now + upload_check_delay;

    int upload_id, start_id = 0;
    
    // Check if there is a new video
    if((upload_id = model.hasVideoInUploadQueue(YT_VIDEO_STATE_NONE))) {

      YouTubeVideo video = model.getVideo(upload_id);
      RX_VERBOSE("UPLOAD_ID: %d, VIDEO.id: %d", upload_id, video.id);
      YouTubeUploadStart start;
      if(!start.start(video, model.getAccessToken())) {
        RX_ERROR("Error while trying to get an upload url for video id: %d, filename: %s", video.id, video.filename.c_str());
        // @todo -> change status to FAILED ? 
      }
      else {
        if(!start.getLocation().size()) {
          RX_ERROR("The returned location is empty so we don't know whereto we need to upload the video");
          // @todo -> change status to e.g. INVALID_UPLOAD_URL
        }
        else if(!model.setVideoUploadURL(video.id, start.getLocation())) {
          RX_ERROR("Failed to set the upload URL in the model.");
          // @todo -> change the status to e.g. ERR_SET_UPLOAD_URL
        }
        else {
          model.setVideoState(video.id, YT_VIDEO_STATE_UPLOAD);
        }
      }

    }
    else if((start_id = model.hasVideoInUploadQueue(YT_VIDEO_STATE_UPLOAD))) {
      YouTubeUpload upload;
      YouTubeVideo video = model.getVideo(start_id);

      YouTubeUploadInfo* upload_info = new YouTubeUploadInfo;
      upload_info->video_id = video.id;
      upload_info->yt = this;

      RX_VERBOSE("Uploading a video with id: %ld, file: %s", video.id, video.filename.c_str());
      RX_VERBOSE("Start uploading a new video from the upload queue, with id: %ld.", video.id);

      bool r = upload.upload(video, model.getAccessToken(), 
                             youtube_on_upload_progress, 
                             youtube_on_upload_ready,
                             upload_info);

      // keep track of the number of times we tried to upload this video.
      model.incrementNumberOfRetries(video.id);
      
      if(!r) {

        // Check if we should fall back after we reached the max number of retries.
        int total_retries = model.getNumberOfRetries(video.id);
        if(total_retries < 0) {
          RX_ERROR("Error while trying to retrieve the total number of retries.");
        }
        else if(total_retries > YT_MAX_UPLOAD_RETRIES) {
          RX_ERROR("Number of retries reached the limit; we're not trying anymore.");
          model.setVideoState(video.id, YT_VIDEO_STATE_FAILED);
        }

        RX_ERROR("Something went wrong while trying to upload the video, with id: %d", video.id);

      }
      else {
        model.setVideoState(video.id, YT_VIDEO_STATE_READY);
      }

      delete upload_info;
      upload_info = NULL;
    }
  }
}

void YouTube::print() {
  int64_t ttl = model.getTokenTimeout() - (rx_time());
  RX_VERBOSE("-----------------------------------------------");
  RX_VERBOSE("refresh_token: %s", model.getRefreshToken().c_str());
  RX_VERBOSE("access_token: %s", model.getAccessToken().c_str());
  RX_VERBOSE("token_timeout: %lld, ttl: %lld", model.getTokenTimeout(), ttl);
  RX_VERBOSE("is_setup: %c", (is_setup) ? 'Y' : 'N');
  RX_VERBOSE("-----------------------------------------------");
}

