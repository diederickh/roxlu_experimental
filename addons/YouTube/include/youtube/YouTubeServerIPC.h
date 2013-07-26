/*

 # YouTubeServerIPC

 This class implements a basic ipc protocol that can be used to run the youtube
 addon in another process. The `YouTubeServerIPC` class makes user of the libuv
 unix domain pipes feature which is -SUPER FAST-

 To use the `YouTubeServerIPC`, use something like:

 ````c++

 // init youtube
 YouTube yt;
 yt.setup(...);
 
 YouTubeServerIPC server(yt, "/tmp/youtube.sock", false);
 if(!server.start()) {
   printf("Cannot start youtube ipc server");
   ::exit(EXIT_FAILURE);
 } 

 while(true) {
   server.update();
 }

 ````
 

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
  bool start();                                                                /* start server; must be running before clients can connect */
  bool stop();                                                                 /* stop server; disconnects all clients and shutsdown the server */
  void update();                                                               /* this should be called as often as possible; processed the uv loop */
 public:                                                                       /* only used internally; must be public for access in callbacks */
  YouTube& youtube;
  ServerIPC server;
};

#endif
