#ifndef ROXLU_LIBAV_AVUTILS_H
#define ROXLU_LIBAV_AVUTILS_H

#include <av/AVTypes.h>

extern bool rx_did_register_all;              /* is set to true once we have register all the libav codecs */

void rx_init_libav();                         /* this will make sure that we only register the libav codecs, network, etc.. once */

#endif
