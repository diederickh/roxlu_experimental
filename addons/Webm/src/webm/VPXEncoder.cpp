#include <webm/VPXEncoder.h>

VPXEncoder::VPXEncoder() 
  :sws(NULL)
  ,iter(NULL)
  ,pkt(NULL)
  ,pic_out(NULL)
  ,flags(0)
{
}

VPXEncoder::~VPXEncoder() {
  shutdown();
}

bool VPXEncoder::shutdown() {
  if(settings.in_w == 0 && settings.in_h == 0) {
    return false;
  }

  settings.fmt = AV_PIX_FMT_NONE;

  if(pic_out) {

    if(vpx_codec_destroy(&ctx)) {
      die("ERROR: failed to destroy codec.");
    }

    vpx_img_free(pic_out);
    pic_out = NULL;
  }

  if(sws) {
    sws_freeContext(sws);
    sws = NULL;
  }

  
  flags = 0;
  return true;
}

bool VPXEncoder::setup(VPXSettings cfg) {
  settings = cfg;
  return true;
}

bool VPXEncoder::initialize() {
  assert(settings.out_w != 0);
  assert(settings.out_h != 0);

  if(!configure()) {
    return false;
  }
  
  if(vpx_codec_enc_init(&ctx, interface, &cfg, 0)) {
    die("ERROR: Failed to initialize codec.");
    return false;
  }

  pic_out = vpx_img_alloc(NULL, VPX_IMG_FMT_I420, settings.out_w, settings.out_h, 0);
  if(!pic_out) {
    die("ERROR: cannot allocate picture.");
    return false;
  }

  if(!initializeSWS()) {
    return false;
  }

  return true;
}

bool VPXEncoder::configure() {
  assert(settings.in_w != 0);
  assert(settings.in_h != 0);

  vpx_codec_err_t res = vpx_codec_enc_config_default(interface, &cfg, 0);
  if(res) {
    RX_ERROR("failed to initialized config: %s\n", vpx_codec_err_to_string(res));
    return false;
  }

  cfg.rc_target_bitrate = settings.out_w * settings.out_h * cfg.rc_target_bitrate / cfg.g_w / cfg.g_h;

  cfg.g_w = settings.out_w;
  cfg.g_h = settings.out_h;
  
  return true;
}

void VPXEncoder::die(const char* s) {
  const char* detail = vpx_codec_error_detail(&ctx);
  RX_ERROR("%s : %s", s, vpx_codec_error(&ctx));
  if(detail) {
    RX_ERROR("%s\n", detail);
  }
}

bool VPXEncoder::initializeSWS() {
  if(sws != NULL) {
    RX_WARNING(("SWS already initialized."));
    return false;
  }
  
  sws = sws_getContext(settings.in_w, settings.in_h, settings.fmt,
                       settings.out_w, settings.out_h, avformat(VPX_IMG_FMT_I420),
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

void VPXEncoder::forceKeyFrame() {
  flags |= VPX_EFLAG_FORCE_KF;
  RX_WARNING(("-----------> KEYFRAME <-------------------"));
}

void VPXEncoder::encode(unsigned char* data, int64_t pts) {
  assert(settings.cb_user != NULL);
  assert(settings.cb_write != NULL);

  if(!rescale(data)) {
    return;
  }

  if(vpx_codec_encode(&ctx, pic_out, pts, 1, flags, VPX_DL_REALTIME)) {
    die("Failed to encode frame");
    return;
  }

  iter = NULL;
  while( (pkt = vpx_codec_get_cx_data(&ctx, &iter)) ) {
    if(pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
      settings.cb_write(pkt, pts, settings.cb_user);
    }
  }
  
  flags &= ~VPX_EFLAG_FORCE_KF;
}

bool VPXEncoder::rescale(unsigned char* data) {
  assert(settings.in_w != 0);
  assert(settings.in_h != 0);

  // vpx way to scale
#if 0 
  vpx_image_t* img_ptr = vpx_img_wrap(pic_in, settings.fmt, settings.in_w, settings.in_h, 0, data);
  if(!img_ptr) {
    RX_ERROR("cannot wrap input image.");
    return false;
  }
  int h = sws_scale(sws, 
                    pic_in->planes, pic_in->stride, 0, settings.in_h, 
                    pic_out->planes, pic_out->stride);
  
  
  if(!sws) {
    RX_ERROR(("sws not initialized"));
    return false;
  }
#endif

  int img_nbytes = avpicture_fill(&pic_in, data, settings.fmt, settings.in_w, settings.in_h);

  int h = sws_scale(sws, 
                    pic_in.data, pic_in.linesize, 0, settings.in_h, 
                    pic_out->planes, pic_out->stride);
  
  if(h != settings.out_h) {
    RX_ERROR("cannot convert input data with sws.");
    return false;
  }

  return true;
}
