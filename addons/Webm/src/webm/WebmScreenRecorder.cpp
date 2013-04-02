#ifdef ROXLU_GL_WRAPPER

#include <webm/WebmScreenRecorder.h>
#include <roxlu/Roxlu.h>

WebmScreenRecorder::WebmScreenRecorder()
  :webm(&ebml),
   nbytes_per_video_frame(0)
{
  RX_WARNING(("@todo - implement the pbos"));
}

WebmScreenRecorder::~WebmScreenRecorder() {
  stop();

  RX_VERBOSE("~WebmScreenRecorder()");
  nbytes_per_video_frame = 0;

#if !defined(WEBM_SCREENREC_USE_PBOS)
  if(pixels) {
    delete[] pixels;
    pixels = NULL;
  }
#endif
}

bool WebmScreenRecorder::setup(WebmScreenRecorderSettings cfg) {
  settings = cfg;

  if(cfg.cb_user == NULL) {
    RX_ERROR("cb_user not set in the configs");
    return false;
  }

  ebml.setCallbacks(settings.cb_write,
                    settings.cb_close,
                    settings.cb_peek,
                    settings.cb_read,
                    settings.cb_skip,
                    NULL,
                    NULL,
                    settings.cb_user);


  if(settings.vid_fmt == AV_PIX_FMT_NONE) {
    settings.vid_fmt = AV_PIX_FMT_RGB24;
  }


  if(settings.use_video) {
    VPXSettings cfg;
    cfg.in_w = settings.vid_in_w;
    cfg.in_h = settings.vid_in_h;
    cfg.out_w = settings.vid_out_w;
    cfg.out_h = settings.vid_out_h;
    cfg.fps = settings.vid_fps;
    cfg.fmt = settings.vid_fmt;

    bool r = webm.setup(cfg);
    if(!r) {
      RX_ERROR("cannot setup webm video");
      return false;
    }
  }

  if(!webm.initialize()) {
    RX_ERROR("cannot initialize webm.");
    return false;
  }

  if(settings.vid_fmt == AV_PIX_FMT_RGB24) { 
    nbytes_per_video_frame = settings.vid_in_w * settings.vid_in_h * 3;
  }
  else {
    RX_ERROR("we only support VPX_IMG_FMT_RGB24 at the moment");
    return false;
  }

#if !defined(WEBM_SCREENREC_USE_PBOS)
  pixels = new unsigned char[nbytes_per_video_frame];
  if(!pixels) {
    RX_ERROR("cannot allocate memory for pixels");
    return false;
  }
  memset(pixels, 0xFF, nbytes_per_video_frame);
#endif

  return true;
}

bool WebmScreenRecorder::start() {
  if(!webm.startThread()) {
    RX_ERROR("cannot start encoder/screen-recorder thread");
    return false;
  }
  return true;
}

bool WebmScreenRecorder::stop() {
  if(!webm.stopThread()) {
    RX_ERROR("cannot stop encoder/screen-recorder thread (?)");
    return false;
  }
  return true;
}

void WebmScreenRecorder::grabFrame() {
#if !defined(WEBM_SCREENREC_USE_PBOS)
  if(webm.wantsNewVideoFrame()) {
    glReadPixels(0,0,settings.vid_in_w, settings.vid_in_h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    webm.addVideoFrame(pixels, nbytes_per_video_frame);
  }
#endif
}


#endif // ROXLU_GL_WRAPPER
