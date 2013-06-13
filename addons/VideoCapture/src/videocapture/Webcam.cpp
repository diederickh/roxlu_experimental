#include <videocapture/Webcam.h>

void webcam_frame_callback(AVFrame* in, size_t nbytesin, AVFrame* out, size_t nbytesout, void* user) {
  Webcam* cam = static_cast<Webcam*>(user);
  cam->surface.setPixels(out->data[0], nbytesout);

  if(cam->cb_frame) {
    cam->cb_frame(in, nbytesin, out, nbytesout, cam->cb_user);
  }
}

Webcam::~Webcam() {
}

bool Webcam::openDevice(int device, VideoCaptureSettings cfg, videocapture_frame_callback frameCB, void* user) {

  cfg.out_pixel_format = AV_PIX_FMT_RGB24;

  if(cap.openDevice(device, cfg, webcam_frame_callback, this)) {
    cb_frame = frameCB;
    cb_user = user;

    surface.setup(cfg.width, cfg.height, GL_RGBA8, GL_RGB, GL_UNSIGNED_BYTE);

    return true;
  }


  return false;
}

void Webcam::update() {
  cap.update();
}

void Webcam::draw(int x, int y, int w, int h) {
  surface.draw(x, y, w, h);
}

