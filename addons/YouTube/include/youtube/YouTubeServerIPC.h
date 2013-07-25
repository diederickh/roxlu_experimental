/*

 # YouTubeServerIPC

 This class implements a basic ipc protocol that can be used to run the youtube
 addon in another process. The `YouTubeServerIPC` class makes user of the libuv
 unix domain pipes feature which is -SUPER FAST-

 */

#ifndef ROXLU_YOUTUBE_SERVER_IPC_H
#define ROXLU_YOUTUBE_SERVER_IPC_H

#include <string>
#include <uv/ServerIPC.h>
#include <youtube/YouTubeTypes.h>

class YouTube;

class YouTubeServerIPC {
 public:
  YouTubeServerIPC(YouTube& youtube, std::string sockfile, bool datapath);
  ~YouTubeServerIPC();
  bool start();
  bool stop();
  void update();
 private:
  YouTube& youtube;
  ServerIPC server;
};

#endif
