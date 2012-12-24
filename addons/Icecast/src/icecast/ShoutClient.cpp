#include "ShoutClient.h"

ShoutClient::ShoutClient() {
}

ShoutClient::~ShoutClient() {
}

int ShoutClient::setup(const std::string ip) {
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

  if(shout_set_port(shout, 8000) != SHOUTERR_SUCCESS) {
    printf("Error setting port: %s\n", shout_get_error(shout));
    return 1;
  }

  if(shout_set_password(shout, "hackme") != SHOUTERR_SUCCESS) {
    printf("Error setting password: %s\n", shout_get_error(shout));
    return 1;
  }
  if(shout_set_mount(shout, "/example") != SHOUTERR_SUCCESS) {
    printf("Error setting mount: %s\n", shout_get_error(shout));
    return 1;
  }

  if(shout_set_user(shout, "source") != SHOUTERR_SUCCESS) {
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
    printf("Connecting.. :%d\n", ret);
  }

  lame_flags = lame_init();
  lame_set_num_channels(lame_flags, 1);
  lame_set_in_samplerate(lame_flags, 44100);
  lame_set_brate(lame_flags, 20);
  lame_set_mode(lame_flags, MONO);

  ret = lame_init_params(lame_flags);
  printf("%ld\n", ret);

  return 0;
}

void ShoutClient::addAudio(const short int* data, size_t nbytes) {
  //int written = lame_encode_buffer_interleaved(lame_flags, (short int*)data, nbytes, mp3_buffer, MP3_BUFFER_SIZE);
  int written = lame_encode_buffer(lame_flags, data, data, 320, mp3_buffer, MP3_BUFFER_SIZE);
  if(written > 0) {
    int ret = shout_send(shout, mp3_buffer, written);
    printf("ENCODED: %d\n", written);
    if(ret != SHOUTERR_SUCCESS) {
      printf("ERROR: cannot shout_send: %s\n", shout_get_error(shout));
    }
    else {
      shout_sync(shout);
    }
  }
}
