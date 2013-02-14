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
//#include <roxlu/opengl/OpenGLInit.h>
//#include <webm/VPX.h>

#include <roxlu/Roxlu.h>

//void vpx_write_callback(const vpx_codec_cx_pkt_t* pkt, int64_t pts, void *user);

struct VPXDecoderSettings {
  int in_w; 
  int in_h;
  int out_w;
  int out_h;
  int fps;
  vpx_img_fmt fmt;
};

class VPXDecoder {
 public:
  VPXDecoder();
  ~VPXDecoder();
  bool setup(VPXDecoderSettings cfg);
  //  bool grabFrame();                                   /* grabs + encodes a video frame based on the given fps, returns true when grabbed */
  void decodeFrame(unsigned char* data, int nbytes);  /* decodes a frame */
 private:
  void rescale(vpx_image_t* in, vpx_image_t* out);
  bool initializeDecoder();
  bool initializeSWS();
  void die();
  AVPixelFormat avformat(vpx_img_fmt f);              /* converts a VPX image format to AV image format. */
 private:
  unsigned char* pixels;                              /* first version of stream encodr uses simply glReadPixels, an optimized version will use pbos */
  //  VPX vpx;
  VPXDecoderSettings settings;
  int millis_per_frame;
  int num_bytes_per_frame;                             /* when using glReadPixels this is the amount of bytes one frame contains */
  int frame_timeout;
  int64_t num_frames_encoded;                          /* number of encoded frames */
  int64_t num_frames_decoded;
  SwsContext* sws;

  /* vpx */
  vpx_codec_ctx_t codec;
  vpx_image_t img_out;

};

#endif
