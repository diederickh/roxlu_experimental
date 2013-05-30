#include <Simulation.h>
#include <image/Image.h>

void on_frame(AVFrame* in, size_t nbytesIn, AVFrame* out, size_t nbytesOut, void* user) {
  RX_VERBOSE("GOT: %d in, out: %d, %d, %d", nbytesIn, nbytesOut, out->width, out->height);
  return;
  static int c = 0;


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
#if !defined(USE_CANON)
   //  ,cap(VIDEOCAPTURE_WINDOWS_MEDIA_FOUNDATION)
  ,cap(VIDEOCAPTURE_AVFOUNDATION)
#endif
{
}

void Simulation::setup() {
  setWindowTitle("Video recorder - Press 1 to start recording. Press 2 to stop the recording.");

#if defined(USE_CANON)
  can.listDevices();

  VideoCaptureSettings cfg;
  cfg.width = 320;
  cfg.height = 240;
  cfg.in_pixel_format = AV_PIX_FMT_YUV420P;
  cfg.in_pixel_format = AV_PIX_FMT_UYVY422;
  cfg.out_pixel_format = AV_PIX_FMT_RGB24;
  cfg.fps = 30.00;

  can.openDevice(0, cfg);

#endif

#if !defined(USE_CANON)
  cap.listDevices();
  VideoCaptureSettings cfg;
  cfg.width = 640;
  cfg.height = 480;
  cfg.in_pixel_format = AV_PIX_FMT_YUV420P;
  cfg.in_pixel_format = AV_PIX_FMT_UYVY422;
  cfg.out_pixel_format = AV_PIX_FMT_RGB24;
  cfg.fps = 30.00;

  if(cap.openDevice(0, cfg, on_frame, this)) {
    cap.startCapture();
  }
  /*
  cap.printCapabilities(0);
  cap.printSupportedPixelFormats(0);
  cap.printSupportedPixelFormats(0, 640, 480);
  cap.printSupportedSizes(0);
  */
#endif
}

void Simulation::update() {
#if !defined(USE_CANON)
   cap.update();
#endif
   RX_VERBOSE(".");
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

#if defined(USE_CANON) 
  if(key == GLFW_KEY_1) {
    for(int i = 0; i < 200; ++i) {
      can.listDevices();
    }
  }
  else if(key == GLFW_KEY_2) {
    VideoCaptureSettings cfg;
    cfg.width = 320;
    cfg.height = 240;
    cfg.in_pixel_format = AV_PIX_FMT_YUV420P;
    cfg.in_pixel_format = AV_PIX_FMT_UYVY422;
    cfg.out_pixel_format = AV_PIX_FMT_RGB24;
    cfg.fps = 30.00;
    can.openDevice(0, cfg);
  }
  else if(key == GLFW_KEY_3) {
    can.closeDevice();
  }
  else if(key == GLFW_KEY_SPACE) {
    printf(">>> take picture\n");
    can.takePicture();
    printf("<<< take picture\n");
  }

#endif

#if !defined(USE_CANON)
  if(key == GLFW_KEY_1) {
    cap.startCapture();
  }
  else if(key == GLFW_KEY_2) {
    cap.stopCapture();
  }
  else if(key == GLFW_KEY_3) {
    VideoCaptureSettings cfg;
    cfg.width = 320;
    cfg.height = 240;
    cfg.in_pixel_format = AV_PIX_FMT_YUV420P;
    cfg.in_pixel_format = AV_PIX_FMT_UYVY422;
    cfg.out_pixel_format = AV_PIX_FMT_RGB24;
    cfg.fps = 30.00;
    cap.openDevice(0, cfg, on_frame, this);
  }
  else if(key == GLFW_KEY_4) {
    RX_VERBOSE("CLOSE!");
    cap.closeDevice();
  }
#endif

}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
  ::exit(EXIT_SUCCESS);
}

