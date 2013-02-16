#ifndef ROXLU_VPX_ENCODER_H
#define ROXLU_VPX_ENCODER_H

extern "C" {
#   define VPX_CODEC_DISABLE_COMPAT 1
#   include <vpx/vpx_encoder.h>
#   include <vpx/vp8cx.h>
#   define interface (vpx_codec_vp8_cx())
#   include <libswscale/swscale.h>
#   include <libavutil/avutil.h>
#   include <libavcodec/avcodec.h>
}

#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <roxlu/core/Log.h>

typedef void(*vpx_write_cb)(const vpx_codec_cx_pkt_t* pkt, int64_t pts, void* user); /* gets called by VPXEncoder when we have encode data */
typedef void(*vpx_read_cb)(unsigned char* pixels, size_t nbytes, void* user);  /* gets called by the VPXDecoder when we have a decoded frame */

struct VPXSettings {
  int in_w;                                          /* video input width */
  int in_h;                                          /* video input height */
  int out_w;                                         /* video output width */
  int out_h;                                         /* video output height */
  int fps;                                           /* video frame rate */
  AVPixelFormat fmt;                                 /* video input format - we convert it to a VPX capable format (I420) */
  vpx_write_cb cb_write;                             /* pointer to the write callback, gets encoded data, from VPXEncoder */
  vpx_read_cb cb_read;                               /* pointer to the read callback, gets decoded data, from VPXDecoder */
  void* cb_user;                                     /* user data for the write callback */
};

class VPXEncoder {
 public:
  VPXEncoder();
  ~VPXEncoder();

  bool setup(VPXSettings cfg);                        /* setup the encoder */
  bool initialize();                                  /* one time initialization (for multiple recordings) */
  bool shutdown();                                    /* one time deinitialization (for freeing all used memory) */

  void encode(unsigned char* data, int64_t pts);      /* call this to encode an input frame */
  void forceKeyFrame();
 private:
  void die(const char* s);                            /* gets called on failure, shuts down the encoder */
  bool configure();                                   /* configures the codec */
  bool initializeSWS();                               /* initialize the SwsContext for pixel conversion */
  AVPixelFormat avformat(vpx_img_fmt f);              /* returns the AVPixelFormat for the given vpx_img_fmt */
  bool rescale(unsigned char* data);                  /* rescales the input data into I420 and the output w/h */  

 private:
  vpx_codec_enc_cfg_t cfg;                            /* vpx encoder configuration */ 
  vpx_codec_ctx_t ctx;                                /* vpx encoder context */
  const vpx_codec_cx_pkt_t* pkt;                      /* encoded packet, returned by vpx_codec_encode */
  vpx_codec_iter_t iter;                              /* used to iterate over encoded frames */

  VPXSettings settings;                               /* video encoder settings */

  SwsContext* sws;                                    /* video input conversion context */
  AVPicture pic_in;
  vpx_image_t* pic_out;                               /* converted pixels, in VPX_IMG_FMT_I420 */
  int flags;                                          /* flags used while encoding e.g. to force a keyframe */
};

#endif
