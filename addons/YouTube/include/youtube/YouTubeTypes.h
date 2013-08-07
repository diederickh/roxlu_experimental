#ifndef ROXLU_YOUTUBE_TYPES_H
#define ROXLU_YOUTUBE_TYPES_H

#include <roxlu/io/Buffer.h>
#include <iostream>
#include <string>

#define YT_CURL_ERR(code)  if(code != CURLE_OK) { RX_VERBOSE("Curl error: %s", curl_easy_strerror(code)); goto error; }  

#define YT_MAX_UPLOAD_RETRIES 4                      /* maximum number of times we will try to reupload a video after receiving an error; when all retries failed we change the status to FAILED */

class YouTube; 

#if 0
enum YouTubeCommandsIPC  {                           /* command codes that are used by the YouTubeClientIPC and YouTubeServerIPC */
  YT_CMD_VIDEO = 1
};
#endif

struct YouTubeVideo {
  YouTubeVideo();
  void print();

  std::string filename;                             /* ipc/db: the name of the video file or or the full path to the video file when datapath=true  */
  std::string video_resource_json;                  /* db:     when you retrieve a video this will contain the json that you need to sent in the request to youtube, it contains the video resource json as described by: https://developers.google.com/youtube/v3/docs/videos*/
  std::string upload_url;                           /* db:     we're using resumable uploads; this will be set when you retrieve an initialized video form the db; this means that the YouTubeUploadStart executed successfully for this video */
  std::string description;                          /* ipc/db: a description for the video; will be shown on youtube */
  std::string title;                                /* ipc/db: the title for the video */
  std::string tags;                                 /* ipc/db: comma separate list with tags for youtube */
  std::string privacy_status;                       /* ipc/db: public, private, unlisted; defaults to private */
  size_t bytes_uploaded;                            /* - not used atm - */
  size_t bytes_total;                               /* the total filesize of the video; is set when calling `YouTubeModel::addVideoToUploadQueue` */
  bool datapath;                                    /* ipc/db: set this to true when the filename is stored in the datapath */
  int id;                                           /* the auto generated id from the model (sqlite in this case) */
  int category;                                     /* the category id for youtube (defaults to 22) */

  /* from parsing the upload json response */
  std::string yt_id;                                /* the ID of the video on youtube; is set after uploading. eg. zHDDFFfSSpYs */
  std::string yt_etag;                              /* the etag of the uploaded video; is set after uploading. eg. dYdUuZbCApIwJ-onuS7iKMSekuo/kdbzaewg3exZonOlTBeNfEUz3Vo */
  std::string yt_published_at;                      /* the published at date; is set after uploading, eg. 2013-08-06T16:55:47.000Z */
  std::string yt_channel_id;                        /* the channel id to which the video has been uploaded; eg. UCXM9u0QQm6O-KztCJCIOde */

  BUFFERIZE(filename, description, title, tags, datapath);
};

struct YouTubeUploadInfo {                           /* when uploading an upload progress callback will be called that is used to update the model; this is passed to the callback */
  int video_id;
  YouTube* yt;
};

struct YouTubeError {                               /* the youtube API returns json error objects when some error occurs (not for each failure); this struct represents these errors. See YouTubeUpload where we parse the error. */
  YouTubeError();
  ~YouTubeError();
  void print();                                     /* outputs/logs the error info */

  std::string domain;                               /* e.g. global */
  std::string reason;                               /* e.g. authError */
  std::string message;                              /* e.g. Invalid Credentials */
  int code;                                         /* e.g. 400, 401, etc.. http status code */
};
#endif
