#ifndef ROXLU_YOUTUBE_TYPES_H
#define ROXLU_YOUTUBE_TYPES_H

#include <iostream>
#include <string>
#undef nil
#include <msgpack.hpp>

#define YT_CURL_ERR(code)  if(code != CURLE_OK) { RX_VERBOSE("Curl error: %s", curl_easy_strerror(code)); goto error; }  


enum YouTubeCommandsIPC  {                            /* command codes that are used by the YouTubeClientIPC and YouTubeServerIPC */
  YT_CMD_VIDEO = 1
};

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

  MSGPACK_DEFINE(filename, description, title, tags, datapath);
};


#endif
