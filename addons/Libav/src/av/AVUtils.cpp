#include <av/AVTypes.h>

bool rx_did_register_all = false;


// @todo - add a rx_shutdown_libav() to deinitialize e.g. the network (see libav/tools/aviocat.c)
void rx_init_libav() {
  if(!rx_did_register_all) {

    av_log_set_level(AV_LOG_DEBUG);

    av_register_all();
    avcodec_register_all();
    avformat_network_init();
    avfilter_register_all();

    rx_did_register_all = true;
  }
}
