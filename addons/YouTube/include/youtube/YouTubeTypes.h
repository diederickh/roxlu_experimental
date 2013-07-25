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

  /* database info */
  std::string filename;
  std::string video_resource_json;
  std::string upload_url;
  size_t bytes_uploaded;
  size_t bytes_total;
  int id;

  /* ipc */
  std::string title;                                /* the title for the video */
  bool datapath;                                    /* set this to true when the filename is stored in the datapath */

  MSGPACK_DEFINE(filename, title, datapath);
};


#endif
