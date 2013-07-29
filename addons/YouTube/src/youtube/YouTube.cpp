#include <youtube/YouTube.h>
#include <fstream>

YouTube::YouTube() 
  :is_setup(false)
  ,upload_check_timeout(0)
  ,upload_check_delay(4) 
{
}

YouTube::~YouTube() {
}

bool YouTube::setup(std::string clientID, std::string clientSecret) {
  client_id = clientID; // oauth consumer key
  client_secret = clientSecret; // oauth consumer secret
  is_setup = true;
  print();
  return true;
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
        RX_ERROR("Error while trying to get an upload url for video id: %id, filename: %s", video.id, video.filename.c_str());
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
          RX_VERBOSE("SET VIDEO STATE TO UPLOAD!");
          model.setVideoState(video.id, YT_VIDEO_STATE_UPLOAD);
        }
      }

    }
    else if((start_id = model.hasVideoInUploadQueue(YT_VIDEO_STATE_UPLOAD))) {
      YouTubeUpload upload;
      YouTubeVideo video = model.getVideo(start_id);
      RX_VERBOSE("Uploading a video with id: %ld, file: %s", video.id, video.filename.c_str());
      RX_VERBOSE("Start uploading a new video from the upload queue, with id: %ld.", video.id);

      if(!upload.upload(video)) {
        RX_ERROR("Something went wrong while trying to upload the video, with id: %d", video.id);
        // @todo -> change status so we will retry 
      }
      else {
        model.setVideoState(video.id, YT_VIDEO_STATE_READY);
      }
    }
  }
}

void YouTube::print() {
  int64_t ttl = model.getTokenTimeout() - (rx_time());
  RX_VERBOSE("-----------------------------------------------");
  RX_VERBOSE("refresh_token: %s", model.getRefreshToken().c_str());
  RX_VERBOSE("access_token: %s", model.getAccessToken().c_str());
  RX_VERBOSE("token_timeout: %ld, ttl: %ld", model.getTokenTimeout(), ttl);
  RX_VERBOSE("is_setup: %c", (is_setup) ? 'Y' : 'N');
  RX_VERBOSE("-----------------------------------------------");
}

