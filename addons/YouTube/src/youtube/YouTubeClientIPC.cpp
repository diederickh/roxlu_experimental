#include <youtube/YouTubeClientIPC.h>
#include <roxlu/io/Buffer.h>

YouTubeClientIPC::YouTubeClientIPC(std::string sockfile, bool datapath) 
  :client(sockfile, datapath)
{
}

YouTubeClientIPC::~YouTubeClientIPC() {
}

bool YouTubeClientIPC::connect() {
  return client.connect();
}

void YouTubeClientIPC::update() {
  client.update();
}

void YouTubeClientIPC::addVideoToUploadQueue(YouTubeVideo video) {
  Buffer buf;
  buf << video.filename << video.description << video.title << video.tags << video.datapath;
  writeCommand(YT_CMD_VIDEO, buf.ptr(), buf.size());
}

void YouTubeClientIPC::writeCommand(int command, char* data, uint32_t nbytes) {
  client.write((char*)&command, sizeof(command));
  client.write((char*)&nbytes, sizeof(nbytes));
  client.write(data, nbytes);
}
