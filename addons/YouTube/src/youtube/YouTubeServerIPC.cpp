#include <youtube/YouTube.h>
#include <youtube/YouTubeServerIPC.h>

void youtube_server_ipc_on_read(ConnectionIPC* con, void* user) {
  // lets check if we have something usefull in the buffer.
  RX_VERBOSE("Received some info from a client...: %ld bytes in buffer", con->buffer.size());

  int cmd;
  uint32_t nbytes;
  size_t offset = sizeof(cmd) + sizeof(nbytes);

  while(con->buffer.size() > offset) {

    memcpy((char*)&cmd, &con->buffer[0], sizeof(cmd));
    memcpy((char*)&nbytes, &con->buffer[sizeof(cmd)], sizeof(nbytes)); 

    if(con->buffer.size() - offset >= nbytes) {
      msgpack::unpacked unp;
      msgpack::unpack(&unp, &con->buffer[offset], nbytes);

      if(cmd == YT_CMD_VIDEO) {
        YouTubeVideo video;
        msgpack::object obj = unp.get();
        try {
          obj.convert(&video);
          video.print();
          con->buffer.erase(con->buffer.begin(), con->buffer.begin() + offset + nbytes);
        }
        catch(std::exception& ex) {
          RX_ERROR("Error while trying to decode: %s", ex.what());
          con->buffer.clear(); // @todo - check if we do right by removing the buffer here... we shouldn't come to this point anyway
          break;
        }
      }
      else {
        RX_ERROR("Unhandled command: %ld, flushing the buffer!", cmd);
        con->buffer.clear();
      }
    }
    else {
      break;
    }
  }
}

YouTubeServerIPC::YouTubeServerIPC(YouTube& youtube, std::string sockfile, bool datapath) 
  :server(sockfile, datapath)
  ,youtube(youtube)
{
  server.setup(youtube_server_ipc_on_read, this);
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
