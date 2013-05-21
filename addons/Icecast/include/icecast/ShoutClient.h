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
#define ICE_ERR_MOUNT "Icecast mount not set"
#define ICE_ERR_NOLOGIN "Icecast can't connect because the login fails"
#define ICE_ERR_NUM_CHANNELS "Incorrect number of channels set for lame encoder"
#define ICE_ERR_SAMPLERATE "No samplerate set for the lame encoder"
#define ICE_ERR_MODE "No lame mode set"
#define ICE_ERR_NEW "Could not allocate shout_t"
#define ICE_ERR_SET_HOST "Error setting hostname: %s"
#define ICE_ERR_SET_PROTO "Error setting protocol: %s" 
#define ICE_ERR_SET_PORT "Error setting port: %s"
#define ICE_ERR_SET_PASS "Error setting password: %s"
#define ICE_ERR_SET_MOUNT "Error setting mount: %s"
#define ICE_ERR_SET_USERNAME "Error setting user: %s"
#define ICE_ERR_SET_FORMAT "Error setting format: %s"
#define ICE_ERR_SET_NONBLOCK "Error setting non-blocking mode: %s"
#define ICE_ERR_ALREADY_STARTED "Error starting icecast stream: already started" 
#define ICE_ERR_NOT_STARTED "Error stopping stream: not started"
#define ICE_ERR_UNKNOWN_MODE "Unknown lame mode"
#define ICE_ERR_UNKNOWN_SAMPLERATE "Unknown samplerate"
#define ICE_ERR_CANNOT_CLOSE "Error while trying to close the shout connection: %s"
#define ICE_ERR_SHOUT_SEND "Error: cannot shout_send: %s"
#define ICE_ERR_LAME_INIT "Error: cannot initialize lame, by calling lame_init(). Out of mem?"

void shoutclient_lame_debugf(const char* format, va_list ap);

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
  std::string icecast_ip;                                                                   /* IPv4 address of your icecast server */
  unsigned short icecast_port;                                                              /* the port on which you icecast server is running */
  std::string icecast_username;                                                             /* the password for the mount point as set in your <mount><username>..</username></mount> field */
  std::string icecast_password;                                                             /* the password for the mount point as set in your <mount><password>..</password></mount> field */
  std::string icecast_mount;                                                                /* the name of the mount point as added to your icecast.xml file. For example `/roxlu.mp3`. */

  /* mp3 encoding */
  int lame_num_channels;                                                                    /* number of channels your source audio data will have */
  ShoutClientSampleRate lame_samplerate;                                                    
  ShoutClientMode lame_mode;
};

struct ShoutClientStreamInfo {
  std::string title;                                                                         /* IDv3 tag info: title, also used for icecast stream name */
  std::string artist;
  std::string album;
  std::string year;
  std::string comment;
  std::string description;                                                                   /* used for the icecast stream */
};


class ShoutClient {
 public:
  ShoutClient();
  ~ShoutClient();
  bool setup(ShoutClientParams& params);                                                     /* setup the shoutclient. settings are stored internally and used in startStreaming() */
  void setStreamInfo(ShoutClientStreamInfo& info);                                           /* set stream information, will be unset when you pass an empty ShoutClientStreamInfo */
  bool addAudio(const short int* data, size_t nbytes, int nsamples);                         /* nsamples is the number of samples of one channel */
  bool addAudioInterleaved(const short int* data, size_t nbytes, int nsamples);              /* call this when the audio data is interleaved */
  bool startStreaming();                                                                     /* start streaming to the server */ 
  bool stopStreaming();                                                                      /* stop streaming to the server */
  bool isStreaming();                                                                        /* check if we are currently streaming */
  int samplerateToInt(ShoutClientSampleRate rate);                                           /* convert a ShoutClientSampleRate to a numeric value, to be used by mp3 encoder */
 private:
  bool is_started;                                                                           /* iternally used to check if the stream is started */
  bool has_info;                                                                             /* is set to true when we have information for the current stream */
  ShoutClientParams params;                                                                  /* parameters we use to connect to the server and start the MP3 encoding */
  ShoutClientStreamInfo info;                                                                /* information that is used to add some extra info to the audio stream */
  shout_t* shout;                                                                            /* our shout context; is allocated in startStreaming and deallocated in stopStreaming */
  lame_global_flags* lame_flags;                                                             /* our mp3/lame encoder context. is allocated in startStreaming and deallocated in stopStreaming */
  unsigned char mp3_buffer[MP3_BUFFER_SIZE];                                                 /* buffers the MP3 data */
};


inline int ShoutClient::samplerateToInt(ShoutClientSampleRate rate) {
  if(rate == SC_SAMPLERATE_44100) {
    return 44100;
  }
  return 0;
}

inline bool ShoutClient::isStreaming() {
  return is_started;
}

inline void ShoutClient::setStreamInfo(ShoutClientStreamInfo& i) {
  has_info = true;
  info = i;
}

#endif



