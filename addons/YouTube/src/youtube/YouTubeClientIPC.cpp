#include <youtube/YouTubeClientIPC.h>
#include <roxlu/io/Buffer.h>

// -----------------------------------------------------------------

void youtube_client_ipc_on_uploaded(std::string path, char* data, size_t nbytes, void* user) {
  RX_VERBOSE("Client got a message that the video was uploaded.");

  YouTubeClientIPC* ipc = static_cast<YouTubeClientIPC*>(user);

  if(ipc->cb_upload_ready) {
    Buffer buf(data, nbytes);
    YouTubeVideo video;
    video.unpack(buf);
    ipc->cb_upload_ready(video, ipc->cb_upload_ready_user);
  }

}

// -----------------------------------------------------------------

YouTubeClientIPC::YouTubeClientIPC(std::string sockfile, bool datapath) 
  :client(sockfile, datapath)
  ,cb_upload_ready(NULL)
  ,cb_upload_ready_user(NULL)
{
  client.addMethod("/uploaded", youtube_client_ipc_on_uploaded, this);
}

YouTubeClientIPC::~YouTubeClientIPC() {
  cb_upload_ready = NULL;
  cb_upload_ready_user = NULL;
}

bool YouTubeClientIPC::connect() {
  return client.connect();
}

void YouTubeClientIPC::update() {
  client.update();
}

void YouTubeClientIPC::addVideoToUploadQueue(YouTubeVideo video) {
  Buffer buf;
  video.pack(buf);
  client.call("/add_to_queue", buf.ptr(), buf.size());
}

void YouTubeClientIPC::setUploadReadyCallback(youtube_upload_ready_callback readyCB, void* user) {
  cb_upload_ready = readyCB;
  cb_upload_ready_user = user;
}
