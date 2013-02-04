#include <webm/WebmScreenRecorder.h>
#include <roxlu/Roxlu.h>

WebmScreenRecorder::WebmScreenRecorder()
  :webm(&ebml),
   nbytes_per_video_frame(0)
{
}

WebmScreenRecorder::~WebmScreenRecorder() {
  stop();

  RX_VERBOSE(("~WebmScreenRecorder()"));
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
    RX_ERROR(("cb_user not set in the configs"));
    return false;
  }

  ebml.setCallbacks(settings.cb_write,
                    settings.cb_close,
                    settings.cb_peek,
                    settings.cb_read,
                    settings.cb_skip,
                    settings.cb_user);

  if(settings.vid_fmt == VPX_IMG_FMT_NONE) {
    settings.vid_fmt = VPX_IMG_FMT_RGB24;
  }

  if(settings.use_video) {
    bool r = webm.setupVideo(settings.vid_in_w, settings.vid_in_h, 
                             settings.vid_out_w, settings.vid_out_h, 
                             settings.vid_fps, settings.vid_fmt);

    if(!r) {
      RX_ERROR(("cannot setup webm video"));
      return false;
    }
  }

  if(!webm.initialize()) {
    RX_ERROR(("cannot initialize webm."));
    return false;
  }

  if(settings.vid_fmt == VPX_IMG_FMT_RGB24) { 
    nbytes_per_video_frame = settings.vid_in_w * settings.vid_in_h * 3;
  }
  else {
    RX_ERROR(("we only support VPX_IMG_FMT_RGB24 at the moment"));
    return false;
  }

#if !defined(WEBM_SCREENREC_USE_PBOS)
  pixels = new unsigned char[nbytes_per_video_frame];
  if(!pixels) {
    RX_ERROR(("cannot allocate memory for pixels"));
    return false;
  }
  memset(pixels, 0xFF, nbytes_per_video_frame);
#endif

  return true;
}

bool WebmScreenRecorder::start() {
  if(!webm.startThread()) {
    RX_ERROR(("cannot start encoder/screen-recorder thread"));
    return false;
  }
  return true;
}

bool WebmScreenRecorder::stop() {
  if(!webm.stopThread()) {
    RX_ERROR(("cannot stop encoder/screen-recorder thread (?)"));
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


