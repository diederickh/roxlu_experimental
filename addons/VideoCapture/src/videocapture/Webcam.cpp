#include <videocapture/Webcam.h>
#include <uv.h>

// ---------------------------------------------------------

void webcam_frame_callback(AVFrame* in, size_t nbytesin, AVFrame* out, size_t nbytesout, void* user) {
  Webcam* cam = static_cast<Webcam*>(user);
  
  if(cam->out_pixels) {

    cam->has_new_pixels = true;
    memcpy(cam->out_pixels, out->data[0], nbytesout);

    if(cam->settings.in_pixel_format != cam->settings.out_pixel_format) {
      memcpy(cam->in_pixels, in->data[0], nbytesin);
    }

  }

  if(cam->cb_frame) {
    cam->cb_frame(in, nbytesin, out, nbytesout, cam->cb_user);
  }
}

// ---------------------------------------------------------

Webcam::Webcam(VideoCaptureImplementation imp)
  :cap(imp)
  ,cb_user(NULL)
  ,cb_frame(NULL)
  ,out_pixels(NULL)
  ,in_pixels(NULL)
  ,gpu_tex(NULL)
  ,has_new_pixels(false)
  ,num_out_bytes(0)
  ,num_in_bytes(0)
{
}

Webcam::~Webcam() {

  // clears all memory
  closeDevice();

  num_in_bytes = 0;
  num_out_bytes = 0;
  has_new_pixels = false;
  gpu_tex = NULL;
  out_pixels = NULL;
  in_pixels = NULL;
  cb_user = NULL;
  cb_frame = NULL;
}

bool Webcam::openDevice(int device, VideoCaptureSettings cfg, videocapture_frame_callback frameCB, void* user) {

  if(out_pixels) {
    RX_ERROR("Already opened the device. Make sure to first close it");
    return false;
  }

  if(!gpu_drawer.setup()) {
    RX_ERROR("Cannot setup the GPUDrawer");
    return false;
  }

  if(!gpu_image.setup(cfg.out_pixel_format)) {
    RX_ERROR("Cannot setup the GPUImage");
    return false;
  }

  settings = cfg;

  gpu_tex = new GLuint[gpu_image.getNumTextures()];
  gpu_image.genTextures(gpu_tex, cfg.width, cfg.height);

  if(cap.openDevice(device, cfg, webcam_frame_callback, this)) {
    cb_frame = frameCB;
    cb_user = user;

    // get number of bytes in output image.
    AVPicture tmp_pic_out;
    num_out_bytes = avpicture_fill(&tmp_pic_out, NULL, cfg.out_pixel_format, cfg.width, cfg.height);
    out_pixels = new char[num_out_bytes];

    // if necessary create a separate buffer for the in pixels
    if(cfg.in_pixel_format != cfg.out_pixel_format) {
      AVPicture tmp_pic_in;
      num_in_bytes = avpicture_fill(&tmp_pic_in, NULL, cfg.in_pixel_format, cfg.width, cfg.height);
      in_pixels = new char[num_in_bytes];
    }
    else {
      in_pixels = out_pixels;
      num_in_bytes = num_out_bytes;
    }

    return true;
  }

  return false;
}

void Webcam::update() {
  if(has_new_pixels) {
    has_new_pixels = false;
    for(std::vector<WebcamListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it) {
      (*it)->onWebcamPixelsUpdated(in_pixels, num_in_bytes, out_pixels, num_out_bytes);
    }

    gpu_image.setPixels(gpu_tex, out_pixels, cap.getWidth(), cap.getHeight());
  }
  cap.update();
}

void Webcam::draw() { 
  gpu_drawer.draw(gpu_image, gpu_tex); 
}

