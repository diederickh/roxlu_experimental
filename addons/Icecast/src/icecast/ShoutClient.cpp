#include <icecast/ShoutClient.h>
#include <roxlu/core/Utils.h>

// -------------------------------------

void shoutclient_lame_debugf(const char* format, va_list ap) {
  (void)vfprintf(stdout, format, ap);
}

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
    RX_ERROR(ICE_ERR_PORT);
    return false;
  }

  if(!icecast_ip.size()) {
    RX_ERROR(ICE_ERR_IP);
    return false;
  }

  if(!icecast_username.size()) {
    RX_ERROR(ICE_ERR_USERNAME);
    return false;
  }

  if(!icecast_password.size()) {
    RX_ERROR(ICE_ERR_PASSWORD);
    return false;
  }

  if(!icecast_mount.size()) {
    RX_ERROR(ICE_ERR_MOUNT);
    return false;
  }
  
  if(!lame_num_channels) {
    RX_ERROR(ICE_ERR_NUM_CHANNELS);
    return false;
  }

  if(lame_samplerate == SC_SAMPLERATE_NOT_SET) {
    RX_ERROR(ICE_ERR_SAMPLERATE);
    return false;
  }

  if(lame_mode == SC_MODE_NOT_SET) {
    RX_ERROR(ICE_ERR_MODE);
    return false;
  }

  return true;
}

// -------------------------------------

ShoutClient::ShoutClient() 
  :is_started(false)
  ,has_info(false)
{
  shout_init();
}

ShoutClient::~ShoutClient() {
  if(is_started) {
    stopStreaming();
  }

  shout_shutdown();
  is_started = false;
  lame_flags = NULL;
  shout = NULL;
  has_info = false;
}

bool ShoutClient::setup(ShoutClientParams& p) {
  long ret;
  params = p;

  if(!params.validate()) {
    return false;
  }

  return true;
}

bool ShoutClient::startStreaming() {
  if(is_started) {
    RX_ERROR(ICE_ERR_ALREADY_STARTED);
    return false;
  }

  if(!(shout = shout_new())) {
    RX_ERROR(ICE_ERR_NEW);
    return false;
  }

  if(shout_set_host(shout, params.icecast_ip.c_str()) != SHOUTERR_SUCCESS) {
    RX_ERROR(ICE_ERR_SET_HOST, shout_get_error(shout));
    shout_free(shout);
    return false;
  }

  if(shout_set_protocol(shout, SHOUT_PROTOCOL_HTTP) != SHOUTERR_SUCCESS) {
    RX_ERROR(ICE_ERR_SET_PROTO, shout_get_error(shout));
    shout_free(shout);
    return false;
  }

  if(shout_set_port(shout, params.icecast_port) != SHOUTERR_SUCCESS) {
    RX_ERROR(ICE_ERR_SET_PORT, shout_get_error(shout));
    shout_free(shout);
    return false;
  }

  if(shout_set_password(shout, params.icecast_password.c_str()) != SHOUTERR_SUCCESS) {
    RX_ERROR(ICE_ERR_SET_PASS, shout_get_error(shout));
    shout_free(shout);
    return false;
  }

  if(shout_set_mount(shout, params.icecast_mount.c_str()) != SHOUTERR_SUCCESS) {
    RX_ERROR(ICE_ERR_SET_MOUNT, shout_get_error(shout));
    shout_free(shout);
    return false;
  }

  if(shout_set_user(shout, params.icecast_username.c_str()) != SHOUTERR_SUCCESS) {
    RX_ERROR(ICE_ERR_SET_USERNAME, shout_get_error(shout));
    shout_free(shout);
    return false;
  }

  if(shout_set_format(shout, SHOUT_FORMAT_MP3) != SHOUTERR_SUCCESS) {
    RX_ERROR(ICE_ERR_SET_FORMAT, shout_get_error(shout));
    shout_free(shout);
    return false;
  }

  if(shout_set_nonblocking(shout, 1) != SHOUTERR_SUCCESS) {
    RX_ERROR(ICE_ERR_SET_NONBLOCK, shout_get_error(shout));
    shout_free(shout);
    return false;
  }

  if(has_info) {
    if(info.description.size()) {
      shout_set_description(shout, info.description.c_str());
    }
    if(info.artist.size()) {
      shout_set_name(shout, info.artist.c_str());
    }
  }
  
  long ret = shout_open(shout);
  if (ret == SHOUTERR_SUCCESS) {
    ret = SHOUTERR_CONNECTED;
  }

  while (ret == SHOUTERR_BUSY) {
    usleep(10000);
    ret = shout_get_connected(shout);
  }
  if(ret == SHOUTERR_NOLOGIN) {
    RX_ERROR(ICE_ERR_NOLOGIN);
    shout_free(shout);
    return false;
  }

  int l_samplerate = 0;
  MPEG_mode l_mode = MONO;

  if(params.lame_samplerate == SC_SAMPLERATE_44100) {
    l_samplerate = 44100;
  }
  else {
    RX_ERROR(ICE_ERR_UNKNOWN_SAMPLERATE);
    shout_free(shout);
    return false;
  }

  if(params.lame_mode == SC_MODE_STEREO) {
    l_mode = STEREO;
  }
  else if(params.lame_mode == SC_MODE_MONO) {
    l_mode = MONO;
  }
  else {
    RX_ERROR(ICE_ERR_UNKNOWN_MODE);
    shout_free(shout);
    return false;
  }

  lame_flags = lame_init();
  if(!lame_flags) {
    RX_ERROR(ICE_ERR_LAME_INIT);
    shout_free(shout);
    return false;
  }

  lame_set_debugf(lame_flags, shoutclient_lame_debugf);
  lame_set_msgf(lame_flags, shoutclient_lame_debugf);
  lame_set_errorf(lame_flags, shoutclient_lame_debugf);
  
  lame_set_num_channels(lame_flags, params.lame_num_channels);
  lame_set_in_samplerate(lame_flags, l_samplerate);
  lame_set_brate(lame_flags, 96); // in kilobits, 96 = 96 kbps
  lame_set_mode(lame_flags, l_mode);
  lame_set_quality(lame_flags, 5); // 2: near best, not too slow, 5: good, fast, 7: ok, really fast 

  if(has_info) {
    if(info.title.size()) {
      id3tag_set_title(lame_flags, info.title.c_str());
    }
    if(info.artist.size()) {
      id3tag_set_artist(lame_flags, info.artist.c_str());
    }
    if(info.year.size()) {
      id3tag_set_year(lame_flags, info.year.c_str());
    }
    if(info.album.size()) {
      id3tag_set_album(lame_flags, info.album.c_str());
    }
    if(info.comment.size()) {
      id3tag_set_comment(lame_flags, info.comment.c_str());
    }
  }
  
  ret = lame_init_params(lame_flags);

  is_started = true;

  return true;
}

bool ShoutClient::stopStreaming() {
  if(!is_started) {
    RX_ERROR(ICE_ERR_NOT_STARTED);
    return false;
  }

  int ret = shout_close(shout);
  if(ret != SHOUTERR_SUCCESS) {
    RX_ERROR(ICE_ERR_CANNOT_CLOSE, shout_get_error(shout));
    return false;
  }

  shout_free(shout);

  lame_close(lame_flags);

  lame_flags = NULL;
  shout = NULL;
  is_started = false;
  return true;
}


bool ShoutClient::addAudio(const short int* data, size_t nbytes, int nsamples) {
  if(!is_started || !shout || !lame_flags) {
    return false;
  }

  int written = lame_encode_buffer(lame_flags, data, data, nsamples, mp3_buffer, MP3_BUFFER_SIZE);
  if(written > 0) {
    int ret = shout_send(shout, mp3_buffer, written);
    if(ret != SHOUTERR_SUCCESS) {
      RX_ERROR(ICE_ERR_SHOUT_SEND, shout_get_error(shout));
      return false;
    }
    else {
      shout_sync(shout);
    }
  }
  return true;
}

bool ShoutClient::addAudioInterleaved(const short int* data, size_t nbytes, int nsamples) {
  if(!is_started || !shout || !lame_flags) {
    return false;
  }

  int written = lame_encode_buffer_interleaved(lame_flags, (short int*)data, nsamples, mp3_buffer, MP3_BUFFER_SIZE);
  if(written > 0) {
    int ret = shout_send(shout, mp3_buffer, written);
    if(ret != SHOUTERR_SUCCESS) {
      RX_ERROR(ICE_ERR_SHOUT_SEND, shout_get_error(shout));
      return false;
    }
    else {
      shout_sync(shout);
    }
  }
  return true;
}
