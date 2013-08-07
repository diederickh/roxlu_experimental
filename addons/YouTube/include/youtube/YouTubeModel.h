#ifndef ROXLU_YOUTUBE_MODEL_H
#define ROXLU_YOUTUBE_MODEL_H

#include <sstream>
#if !defined(WIN32)
#  include <inttypes.h>
#endif
#include <sqlite/Database.h>
#include <youtube/YouTubeTypes.h>

#define YT_VIDEO_STATE_NONE 0
#define YT_VIDEO_STATE_UPLOAD 1
#define YT_VIDEO_STATE_READY 2
#define YT_VIDEO_STATE_FAILED 99   /* we set the state to FAILED when the number of retries for a video reaches YT_MAX_UPLOAD_RETRIES */

using namespace roxlu;

class YouTubeModel {
 public:
  YouTubeModel();
  ~YouTubeModel();
  bool addVideoToUploadQueue(YouTubeVideo video);                                 /* adds the given video to the upload queue, the state will be set to `0`, YT_VIDEO_STATE_NONE; make sure that these fields are set in the video struct: filename, datapath, title */
  int hasVideoInUploadQueue(int state);                                           /* returns the ID of the video that is in the queue with the given state; returns 0 when nothing is found */
  bool setVideoState(int id, int state);                                          /* changes the state of the video in the upload queue */
  bool setVideoUploadURL(int id, std::string url);                                /* set the video upload url for a resumable upload */
  bool setVideoBytesUploaded(int id, size_t nbytes);                              /* update the number of bytes that have been uploaded for the video */
  bool incrementVideoBytesUploaded(int id, size_t nbytes);                        /* increment the number of bytes which have been uploaded so far */
  bool incrementNumberOfRetries(int id);                                          /* increment the number of times we tried to upload the video by one */
  YouTubeVideo getVideo(int id);                                                  /* get a video from the upload queue */
  void setRefreshToken(std::string rtoken);                                       /* stores the given refresh token (must be reloadable, when application restarts) */
  void setAccessToken(std::string atoken, uint64_t timeout);                      /* stores the given access token + timout (must be reloadable, when application restarts) */
  std::string getRefreshToken();                                                  /* returns the last stored refresh token */
  std::string getAccessToken();                                                   /* returns the last stored access token */
  uint64_t getTokenTimeout();                                                     /* returns the last stored token timeout, which indicates when we need to refresh the tokenn */
  int getNumberOfRetries(int id);                                                 /* get the number of times we retried to upload the video for the given id, when the result is < 0 it mean we encountered a problem when querying. */
 private:
  Database db;                                                                    /* we use SQLite to store the state and video upload queue */
};

inline std::string YouTubeModel::getRefreshToken() {
  QueryResult qr(db);
  db.select("value").from("state").where("name = 'refresh_token'").execute(qr);

  qr.next();
    std::string rtoken = qr.getString(0);
  qr.finish();

  return rtoken;
}

inline std::string YouTubeModel::getAccessToken() {
  QueryResult qr(db);
  db.select("value").from("state").where("name = 'access_token'").execute(qr);

  qr.next();
    std::string atoken = qr.getString(0);
  qr.finish();

  return atoken;
}

inline uint64_t YouTubeModel::getTokenTimeout() {
  QueryResult qr(db);
  db.select("value").from("state").where("name = 'token_timeout'").execute(qr);

  qr.next();
   int64_t tt = qr.getInt(0);
  qr.finish();

  return tt;
}

#endif
