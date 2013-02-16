#ifndef ROXLU_VPX_DECODER_H
#define ROXLU_VPX_DECODER_H

extern "C" {
#  define VPX_CODEC_DISABLE_COMPAT 1
#  include <stdlib.h>
#  include <vpx/vpx_decoder.h>
#  include <vpx/vp8dx.h>
#  include <libswscale/swscale.h>
#  include <libavutil/avutil.h>
#  define decode_interface (vpx_codec_vp8_dx())
}

#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <webm/VPXEncoder.h>                                             /* for VPXSettings */

#include <roxlu/Roxlu.h>

class VPXDecoder {
 public:
  VPXDecoder();
  ~VPXDecoder();
  bool setup(VPXSettings cfg);
  void decodeFrame(unsigned char* data, int nbytes);                    /* decodes a frame */
 private:
  void rescale(vpx_image_t* in, vpx_image_t* out);
  bool initializeDecoder();
  bool initializeSWS();
  void die();
 private:
  unsigned char* pixels;                                                 /* first version of stream encoder uses simply glReadPixels, an optimized version will use pbos */
  VPXSettings settings;    
  int num_bytes_per_frame;                                               /* when using glReadPixels this is the amount of bytes one frame contains */
  int64_t num_frames_decoded;
  SwsContext* sws;

  /* vpx */
  vpx_codec_ctx_t codec;
  vpx_image_t img_out;

};

#endif
