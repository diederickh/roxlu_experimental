#include <videocapture/Webcam.h>

void webcam_frame_callback(AVFrame* in, size_t nbytesin, AVFrame* out, size_t nbytesout, void* user) {
  Webcam* cam = static_cast<Webcam*>(user);

  if(cam->out_pixels) {
    memcpy(cam->out_pixels, out->data[0], nbytesout);
    cam->has_new_pixels = true;
  }

  if(cam->cb_frame) {
    cam->cb_frame(in, nbytesin, out, nbytesout, cam->cb_user);
  }
}

Webcam::~Webcam() {
  closeDevice();
  num_out_bytes = 0;
  has_new_pixels = false;
}

bool Webcam::openDevice(int device, VideoCaptureSettings cfg, videocapture_frame_callback frameCB, void* user) {

  if(out_pixels) {
    RX_ERROR("Already opened the device. Make sure to first close it");
    return false;
  }

#if 0
  if(cfg.out_pixel_format != AV_PIX_FMT_RGB24) {
    RX_ERROR("For now we only support an out pixel format of AV_PIX_FMT_RGB24; in future releases we will support other formats");
    return false;
  }

  cfg.out_pixel_format = AV_PIX_FMT_RGB24;
#endif

  if(cap.openDevice(device, cfg, webcam_frame_callback, this)) {
    cb_frame = frameCB;
    cb_user = user;

    // get number of bytes in output image.
    AVPicture tmp_pic;
    num_out_bytes = avpicture_fill(&tmp_pic, NULL, cfg.out_pixel_format, cfg.width, cfg.height);
    out_pixels = new char[num_out_bytes];

    // setup the GL-surface
#if 0
    surface.setup(cfg.width, cfg.height, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
#endif

    return true;
  }

  return false;
}

void Webcam::update() {
  if(has_new_pixels) {
#if 0
    surface.setPixels((unsigned char*)out_pixels, num_out_bytes);
#endif
    has_new_pixels = false;
    for(std::vector<WebcamListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it) {
      (*it)->onWebcamPixelsUpdated(out_pixels, num_out_bytes);
    }
  }
  cap.update();
}

void Webcam::draw(int x, int y, int w, int h) {
#if 0
  surface.draw(x, y, w, h);
#endif
}

