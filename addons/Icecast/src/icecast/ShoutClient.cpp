#include <icecast/ShoutClient.h>

// -------------------------------------

ShoutClientParams::ShoutClientParams() 
  :icecast_port(0)
  ,lame_num_channels(0)
  ,lame_samplerate(SC_SAMPLERATE_NOT_SET)
  ,lame_mode(SC_MODE_NOT_SET)
{
}

ShoutClientParams::~ShoutClientParams() {
  icecast_port = 0;
  icecast_ip = "";
  icecast_username = "";
  icecast_password = "";
  icecast_mount = "";
  lame_num_channels = 0;
  lame_samplerate = SC_SAMPLERATE_NOT_SET;
  lame_mode = SC_MODE_NOT_SET;
}

bool ShoutClientParams::validate() {
  if(!icecast_port) {
    RX_ERROR((ICE_ERR_PORT));
    return false;
  }

  if(!icecast_ip.size()) {
    RX_ERROR((ICE_ERR_IP));
    return false;
  }

  if(!icecast_username.size()) {
    RX_ERROR((ICE_ERR_USERNAME));
    return false;
  }

  if(!icecast_password.size()) {
    RX_ERROR((ICE_ERR_PASSWORD));
    return false;
  }

  if(!icecast_mount.size()) {
    RX_ERROR((ICE_ERR_MOUNT));
    return false;
  }
  
  if(!lame_num_channels) {
    RX_ERROR((ICE_ERR_NUM_CHANNELS));
    return false;
  }

  if(lame_samplerate == SC_SAMPLERATE_NOT_SET) {
    RX_ERROR((ICE_ERR_SAMPLERATE));
    return false;
  }

  if(lame_mode == SC_MODE_NOT_SET) {
    RX_ERROR((ICE_ERR_MODE));
    return false;
  }

  return true;
}

// -------------------------------------

ShoutClient::ShoutClient() {
}

ShoutClient::~ShoutClient() {
}

int ShoutClient::setup(
                       const std::string ip, 
                       const unsigned short port, /* 8000 */
                       const std::string username, /* shout */
                       const std::string password, /* hackme */
                       const std::string mount /* /example (no .mp3! ) */
)
{
  long ret;

  shout_init();

  if(!(shout = shout_new())) {
    printf("Could not allocate shout_t\n");
    return 1;
  }

  if(shout_set_host(shout, ip.c_str()) != SHOUTERR_SUCCESS) {
    printf("Error setting hostname: %s\n", shout_get_error(shout));
    return 1;
  }

  if(shout_set_protocol(shout, SHOUT_PROTOCOL_HTTP) != SHOUTERR_SUCCESS) {
    printf("Error setting protocol: %s\n", shout_get_error(shout));
    return 1;
  }

  if(shout_set_port(shout, port) != SHOUTERR_SUCCESS) {
    printf("Error setting port: %s\n", shout_get_error(shout));
    return 1;
  }

  if(shout_set_password(shout, password.c_str()) != SHOUTERR_SUCCESS) {
    printf("Error setting password: %s\n", shout_get_error(shout));
    return 1;
  }
  if(shout_set_mount(shout, mount.c_str()) != SHOUTERR_SUCCESS) {
    printf("Error setting mount: %s\n", shout_get_error(shout));
    return 1;
  }

  if(shout_set_user(shout, username.c_str()) != SHOUTERR_SUCCESS) {
    printf("Error setting user: %s\n", shout_get_error(shout));
    return 1;
  }

  if(shout_set_format(shout, SHOUT_FORMAT_MP3) != SHOUTERR_SUCCESS) {
    printf("Error setting user: %s\n", shout_get_error(shout));
    return 1;
  }

  if(shout_set_nonblocking(shout, 1) != SHOUTERR_SUCCESS) {
    printf("Error setting non-blocking mode: %s\n", shout_get_error(shout));
    return 1;
  }

  ret = shout_open(shout);
  if (ret == SHOUTERR_SUCCESS) {
    ret = SHOUTERR_CONNECTED;
  }

  while (ret == SHOUTERR_BUSY) {
    usleep(10000);
    ret = shout_get_connected(shout);
    printf("Connecting.. :%ld\n", ret);
  }

  lame_flags = lame_init();
  lame_set_num_channels(lame_flags, 1);
  lame_set_in_samplerate(lame_flags, 44100);
  lame_set_brate(lame_flags, 20);
  lame_set_mode(lame_flags, MONO);

  ret = lame_init_params(lame_flags);
  return 0;
}

// 
void ShoutClient::addAudio(const short int* data, size_t nbytes) {
  //int written = lame_encode_buffer_interleaved(lame_flags, (short int*)data, nbytes, mp3_buffer, MP3_BUFFER_SIZE);
  int written = lame_encode_buffer(lame_flags, data, data, 320, mp3_buffer, MP3_BUFFER_SIZE);
  if(written > 0) {
    int ret = shout_send(shout, mp3_buffer, written);
    if(ret != SHOUTERR_SUCCESS) {
      printf("ERROR: cannot shout_send: %s\n", shout_get_error(shout));
    }
    else {
      shout_sync(shout);
    }
  }
}
