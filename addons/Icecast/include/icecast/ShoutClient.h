#ifndef ROXLU_SHOUTCLIENT_H
#define ROXLU_SHOUTCLIENT_H

#include <string>
#include <shout/shout.h>
#include <lame/lame.h> /* see API in liblame source for info on encoding */

#define MP3_BUFFER_SIZE 8192

#define ICE_ERR_PORT "Icecast server port not set"
#define ICE_ERR_IP "Icecast server ip not set"
#define ICE_ERR_USERNAME "Icecast username not set"
#define ICE_ERR_PASSWORD "Icecast password not set"
#define ICE_ERR_MOUNT "Icecaset mount not set"
#define ICE_ERR_NUM_CHANNELS "Incorrect number of channels set for lame encoder"
#define ICE_ERR_SAMPLERATE "No samplerate set for the lame encoder"
#define ICE_ERR_MODE "No lame mode set"

enum ShoutClientSampleRate {
  SC_SAMPLERATE_44100,
  SC_SAMPLERATE_NOT_SET
};

enum ShoutClientMode {
  SC_MODE_MONO,
  SC_MODE_STEREO,
  SC_MODE_NOT_SET
};

struct ShoutClientParams {
  ShoutClientParams();
  ~ShoutClientParams();
  bool validate();

  /* icecast */
  std::string icecast_ip;
  unsigned short icecast_port;
  std::string icecast_username;
  std::string icecast_password;
  std::string icecast_mount;

  /* mp3 encoding */
  int lame_num_channels;
  ShoutClientSampleRate lame_samplerate;
  ShoutClientMode lame_mode;
};

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



