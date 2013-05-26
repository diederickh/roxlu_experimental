#include <Simulation.h>
#include <image/Image.h>

void on_frame(AVFrame* in, size_t nbytesIn, AVFrame* out, size_t nbytesOut, void* user) {
  RX_VERBOSE("GOT: %d in, out: %d, %d, %d", nbytesIn, nbytesOut, out->width, out->height);
  static int c = 0;

  return;
  char fname[512];
  sprintf(fname, "img_%04d.png", c);
  
  Image img;
  img.copyPixels(out->data[0], in->width, in->height, RX_FMT_RGB24);
  img.save(fname, true);

  ++c;
}

// -----------------

Simulation::Simulation()
  :SimulationBase()
{
  mac.listDevices();
  
  mac.printSupportedPixelFormats(0);
  // mac.printCapabilities(0);

  if(mac.isPixelFormatSupported(0, AV_PIX_FMT_YUYV422)) {
    VideoCaptureSettings cfg;
    //cfg.in_pixel_format = AV_PIX_FMT_UYVY422;
    cfg.in_pixel_format = AV_PIX_FMT_YUYV422;
    //cfg.in_pixel_format = AV_PIX_FMT_RGB24;
    //cfg.out_pixel_format = AV_PIX_FMT_RGB24;
    cfg.out_pixel_format  = AV_PIX_FMT_UYVY422;
    cfg.width = 640;
    cfg.height = 480;
    cfg.fps = 5.00;
    if(!mac.openDevice(0, cfg, on_frame, this)) {
      RX_ERROR("Cannot open devvice");
    }
    else {
      RX_VERBOSE("Opened");
      mac.startCapture();
    }
  }
  if(mac.isPixelFormatSupported(0, AV_PIX_FMT_YUYV422)) {
    /*
    mac.isSizeSupported(0, 640, 480);
    mac.isFrameRateSupported(0, 30.0);
    mac.printSupportedFrameRates(0, 640, 480, AV_PIX_FMT_YUYV422);
    mac.printSupportedPixelFormats(0, 640, 480);
    mac.printSupportedSizes(0);
    mac.printCapabilities(0);
    mac.printSupportedPixelFormats(0);
    */
  }


}

void Simulation::setup() {
  setWindowTitle("Video recorder - Press 1 to start recording. Press 2 to stop the recording.");

#if 0
  if(!cam.setup()) {
    RX_ERROR("Cannot setup the webcam and/or encoder; check if we're using the correct device and correct encoder settings.");
    ::exit(EXIT_FAILURE);
  }

  cam.startCapture();
#endif

}

void Simulation::update() {
#if 0
  cam.update();
#endif
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#if 0
  cam.draw();
#endif

  fps.draw();
}

void Simulation::onMouseUp(int x, int y, int button) {
}

void Simulation::onMouseDown(int x, int y, int button) {
}

void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {
}

void Simulation::onMouseMove(int x, int y) {
}

void Simulation::onChar(int ch) {
}

void Simulation::onKeyDown(int key) {
#if 0
  if(key == GLFW_KEY_1) {
    cam.startRecording();
  }
  else if(key == GLFW_KEY_2) {
    cam.stopRecording();
  }
#endif
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
#if 0
  cam.stopCapture();
  cam.stopRecording();
#endif
  ::exit(EXIT_SUCCESS);
}

