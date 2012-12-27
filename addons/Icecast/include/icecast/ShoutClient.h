#ifndef ROXLU_SHOUTCLIENT_H
#define ROXLU_SHOUTCLIENT_H

#include <string>
#include <shout/shout.h>
#include <lame/lame.h>

#define MP3_BUFFER_SIZE 8192

class ShoutClient {
 public:
  ShoutClient();
  ~ShoutClient();
  int setup(const std::string ip,
    const unsigned short port,
    const std::string username,
    const std::string password,
    const std::string mount
  );
  
  void addAudio(const short int* data, size_t nbytes);
 private:
  shout_t* shout;
  lame_global_flags* lame_flags;
  unsigned char mp3_buffer[MP3_BUFFER_SIZE];
};


#endif



