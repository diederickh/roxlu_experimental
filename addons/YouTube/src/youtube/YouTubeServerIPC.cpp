#include <roxlu/io/Buffer.h>
#include <youtube/YouTube.h>
#include <youtube/YouTubeServerIPC.h>

void youtube_server_ipc_on_add_to_queue(std::string path, char* data, size_t nbytes, void* user) {
  if(!nbytes) {
    RX_ERROR("/add_to_queue called but we didn't receive any data (?)");
    return;
  }

  YouTubeServerIPC* ipc = static_cast<YouTubeServerIPC*>(user);
  Buffer buf(data, nbytes);
  YouTubeVideo video;
  video.unpack(buf);

  ipc->youtube.addVideoToUploadQueue(video);
}

void youtube_server_ipc_on_upload_ready(YouTubeVideo video, void* user) {
  YouTubeServerIPC* ipc = static_cast<YouTubeServerIPC*>(user);
  Buffer buf;
  video.pack(buf);
  ipc->server.call("/uploaded", buf.ptr(), buf.size());
}

YouTubeServerIPC::YouTubeServerIPC(YouTube& youtube, std::string sockfile, bool datapath) 
  :server(sockfile, datapath)
  ,youtube(youtube)
{
  server.addMethod("/add_to_queue", youtube_server_ipc_on_add_to_queue, this);
  youtube.setUploadReadyCallback(youtube_server_ipc_on_upload_ready, this);
}

YouTubeServerIPC::~YouTubeServerIPC() {
}

bool YouTubeServerIPC::start() {
  return server.start();
}

bool YouTubeServerIPC::stop() {
  return server.stop();
}

void YouTubeServerIPC::update() {
  server.update();
}

