#include <youtube/YouTubeClientIPC.h>

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
  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, video);
  writeCommand(YT_CMD_VIDEO, sbuf.data(), sbuf.size());
}

void YouTubeClientIPC::writeCommand(int command, char* data, size_t nbytes) {
  RX_VERBOSE("WRITING: %ld bytes", nbytes);
  msgpack::unpacked unp;
  msgpack::unpack(&unp, data, nbytes);
  msgpack::object obj = unp.get();
  YouTubeVideo v;
  obj.convert(&v);
  RX_VERBOSE("CONVERTED!: %s, %ld, %ld", v.filename.c_str(), sizeof(command), sizeof(nbytes));
  client.write((char*)&command, sizeof(command));
  client.write((char*)&nbytes, sizeof(nbytes));
  client.write(data, nbytes);
}
