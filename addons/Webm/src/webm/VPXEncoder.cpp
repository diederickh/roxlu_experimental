#include <webm/VPXEncoder.h>

VPXEncoder::VPXEncoder() 
  :in_w(0)
  ,in_h(0)
  ,out_w(0)
  ,out_h(0)
  ,fps(0)
  ,fmt(VPX_IMG_FMT_NONE)
  ,sws(NULL)
  ,iter(NULL)
  ,pkt(NULL)
  ,pic_in(NULL)
  ,pic_out(NULL)
  ,cb_write(NULL)
  ,cb_user(NULL)
{
}

VPXEncoder::~VPXEncoder() {
  shutdown();
}

bool VPXEncoder::shutdown() {
  if(in_w == 0 && in_h == 0) {
    return false;
  }

  in_w = 0;
  in_h = 0;
  out_w = 0;
  out_h = 0;
  fmt = VPX_IMG_FMT_NONE;

  if(pic_in) {
    vpx_img_free(pic_in);
    pic_in = NULL;
  }

  if(pic_out) {
    vpx_img_free(pic_out);
    pic_out = NULL;
  }

  if(sws) {
    sws_freeContext(sws);
    sws = NULL;
  }

  if(vpx_codec_destroy(&ctx)) {
    die("ERROR: failed to destroy codec.");
  }
  
  cb_user = NULL;
  cb_write = NULL;

  return true;
}

bool VPXEncoder::setup(int inW, 
                int inH, 
                int outW, 
                int outH, 
                int frate, 
                vpx_img_fmt format,
                vpx_write_cb writeCB,
                void* userCB
)
{
  in_w = inW;
  in_h = inH;
  out_w = outW;
  out_h = outH;
  fps = frate;
  fmt = format;
  cb_user = userCB;
  cb_write = writeCB;
  return true;
}

bool VPXEncoder::initialize() {
  if(!configure()) {
    return false;
  }
  
  if(vpx_codec_enc_init(&ctx, interface, &cfg, 0)) {
    die("ERROR: Failed to initialize codec.");
    return false;
  }

  pic_out = vpx_img_alloc(NULL, VPX_IMG_FMT_I420, out_w, out_h, 0);
  if(!pic_out) {
    die("ERROR: cannot allocate picture.");
    return false;
  }

  pic_in = vpx_img_alloc(NULL, fmt, in_w, in_h, 0);
  if(!pic_in) {
    die("ERROR: cannot allocate picture.");
    return false;
  }

  if(!initializeSWS()) {
    return false;
  }

  return true;
}

bool VPXEncoder::configure() {
  assert(in_w != 0);
  assert(in_h != 0);

  vpx_codec_err_t res = vpx_codec_enc_config_default(interface, &cfg, 0);
  if(res) {
    RX_ERROR(("failed to initialized config: %s\n", vpx_codec_err_to_string(res)));
    return false;
  }

  cfg.rc_target_bitrate = out_w * out_h * cfg.rc_target_bitrate / cfg.g_w / cfg.g_h;

  cfg.g_w = out_w;
  cfg.g_h = out_h;
  
  return true;
}

void VPXEncoder::die(const char* s) {
  const char* detail = vpx_codec_error_detail(&ctx);
  RX_ERROR(("%s : %s", s, vpx_codec_error(&ctx)));
  if(detail) {
    RX_ERROR(("%s\n", detail));
  }
}


bool VPXEncoder::initializeSWS() {
  if(sws != NULL) {
    RX_WARNING(("SWS already initialized."));
    return false;
  }
  
  sws = sws_getContext(in_w, in_h, avformat(fmt),
                       out_w, out_h, avformat(VPX_IMG_FMT_I420),
                       SWS_FAST_BILINEAR, NULL, NULL, NULL);

  if(!sws) {
    RX_ERROR(("cannot initialize sws context."));
    return false;
  }

  return true;
}

AVPixelFormat VPXEncoder::avformat(vpx_img_fmt f) {
  switch(f) {
    case VPX_IMG_FMT_RGB24: return AV_PIX_FMT_RGB24; 
    case VPX_IMG_FMT_ARGB: return AV_PIX_FMT_ARGB; 
    case VPX_IMG_FMT_I420: return AV_PIX_FMT_YUV420P;
    default: { return AV_PIX_FMT_NONE; }
  }
}

void VPXEncoder::encode(unsigned char* data, int64_t pts) {
  if(!rescale(data)) {
    return;
  }

  if(vpx_codec_encode(&ctx, pic_out, pts, 1, 0, VPX_DL_REALTIME)) {
    die("Failed to encode frame");
    return;
  }

  iter = NULL;
  while( (pkt = vpx_codec_get_cx_data(&ctx, &iter)) ) {
    if(pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
      cb_write(pkt, pts, cb_user);
    }
  }
}

bool VPXEncoder::rescale(unsigned char* data) {

  vpx_image_t* img_ptr = vpx_img_wrap(pic_in, fmt, in_w, in_h, 0, data);
  if(!img_ptr) {
    RX_ERROR(("cannot wrap input image."));
    return false;
  }
  
  int h = sws_scale(sws, 
                    pic_in->planes, pic_in->stride, 0, in_h, 
                    pic_out->planes, pic_out->stride);

  if(h != out_h) {
    RX_ERROR(("cannot convert input data with sws."));
    return false;
  }

  return true;
}
