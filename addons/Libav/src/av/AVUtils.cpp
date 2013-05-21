#include <av/AVTypes.h>

bool rx_did_register_all = false;

void rx_init_libav() {
  if(!rx_did_register_all) {

    av_register_all();
    avcodec_register_all();
    avformat_network_init();

    rx_did_register_all = true;
  }
}
