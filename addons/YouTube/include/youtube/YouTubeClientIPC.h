/*
  # YouTubeClientIPC
  
  The `YouTubeClientIPC` class is the interface that you can use to remotely 
  control the youtube api; The client connects to a unix socket file that must
  be created by teh `YouTubeServerIPC`. The `YouTubeServerIPC` will handle all
  all commands from this client such as adding new videos to the upload queue
  and making sure that the videos get uploaded correctly.
  
  You use the `YouTubeClientIPC` when you want to run the `YouTube` api implementation
  in another process.

*/
#ifndef ROXLU_YOUTUBE_CLIENT_IPC_H
#define ROXLU_YOUTUBE_CLIENT_IPC_H

extern "C" {
#  include <uv.h> 
}

#include <string>
#include <uv/ClientIPC.h>
#include <youtube/YouTubeTypes.h>
#include <msgpack.hpp>

class YouTubeClientIPC {
 public:
  YouTubeClientIPC(std::string sockfile, bool datapath);
  ~YouTubeClientIPC();
  bool connect();
  void update();
  void addVideoToUploadQueue(YouTubeVideo video);
 private:
  void writeCommand(int command, char* data, uint32_t nbytes);
 private:
  ClientIPC client;
};


#endif
