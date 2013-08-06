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

#include <uv/IPC.h>
#include <youtube/YouTubeTypes.h>
#include <youtube/YouTubeUpload.h>

void youtube_client_ipc_on_uploaded(std::string path, char* data, size_t nbytes, void* user);  /* gets called by the youtube server ipc when a video has been uploaded */

class YouTubeClientIPC {
 public:
  YouTubeClientIPC(std::string sockfile, bool datapath);
  ~YouTubeClientIPC();
  bool connect();                                                                              /* call this once to kickoff, it will try to connect and if we can't connect we will automatically keep trying */
  void update();                                                                               /* call this often to flush the network buffers */
  void addVideoToUploadQueue(YouTubeVideo video);                                              /* adds the new video to the upload queue. see YouTube.h for more info */
  void setUploadReadyCallback(youtube_upload_ready_callback readyCB, void* user);              /* set the callback that gets called whenever a video has been completely uploaded to the youtube server */
 public:
  youtube_upload_ready_callback cb_upload_ready;
  void* cb_upload_ready_user;
 private:
  ClientIPC client;

};


#endif
