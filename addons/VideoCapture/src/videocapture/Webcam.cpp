#include <videocapture/Webcam.h>

void webcam_frame_callback(AVFrame* in, size_t nbytesin, AVFrame* out, size_t nbytesout, void* user) {
  Webcam* cam = static_cast<Webcam*>(user);

  if(cam->pixels) {
    memcpy(cam->pixels, out->data[0], nbytesout);
    cam->has_new_pixels = true;
  }

  if(cam->cb_frame) {
    cam->cb_frame(in, nbytesin, out, nbytesout, cam->cb_user);
  }
}

Webcam::~Webcam() {
  closeDevice();
  num_bytes = 0;
  has_new_pixels = false;
}

bool Webcam::openDevice(int device, VideoCaptureSettings cfg, videocapture_frame_callback frameCB, void* user) {

  if(pixels) {
    RX_ERROR("Already opened the device. Make sure to first close it");
    return false;
  }

  if(cfg.out_pixel_format != AV_PIX_FMT_RGB24) {
    RX_ERROR("For now we only support an out pixel format of AV_PIX_FMT_RGB24; in future releases we will support other formats");
    return false;
  }

  cfg.out_pixel_format = AV_PIX_FMT_RGB24;

  if(cap.openDevice(device, cfg, webcam_frame_callback, this)) {
    cb_frame = frameCB;
    cb_user = user;

    // get number of bytes in output image.
    AVPicture tmp_pic;
    num_bytes = avpicture_fill(&tmp_pic, NULL, cfg.out_pixel_format, cfg.width, cfg.height);
    pixels = new char[num_bytes];

    // setup the GL-surface
    surface.setup(cfg.width, cfg.height, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);

    return true;
  }

  return false;
}

void Webcam::update() {
  if(has_new_pixels) {
    surface.setPixels((unsigned char*)pixels, num_bytes);
    has_new_pixels = false;
  }
  cap.update();
}

void Webcam::draw(int x, int y, int w, int h) {
  surface.draw(x, y, w, h);
}

