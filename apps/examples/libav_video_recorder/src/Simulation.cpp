#include <Simulation.h>
#include <image/Image.h>

void on_frame(AVFrame* in, size_t nbytesIn, AVFrame* out, size_t nbytesOut, void* user) {
  RX_VERBOSE("GOT: %d in, out: %d, %d, %d", nbytesIn, nbytesOut, out->width, out->height);
  Simulation* sim = static_cast<Simulation*>(user);

  memcpy(sim->pixels, out->data[0], nbytesOut);
  sim->has_new_frame = true;
  /*
  static int c = 0;
  char fname[512];
  sprintf(fname, "img_%04d.png", c);
  Image img;
  img.copyPixels(out->data[0], in->width, in->height, RX_FMT_RGB24);
  img.save(fname, true);
  ++c;
  */
}

// -----------------

Simulation::Simulation()
  :SimulationBase()
  ,pixels(NULL)
  ,has_new_frame(false)
#if !defined(USE_CANON)
   //  ,cap(VIDEOCAPTURE_WINDOWS_MEDIA_FOUNDATION)
   //,cap(VIDEOCAPTURE_AVFOUNDATION)
   ,cap(VIDEOCAPTURE_EDSDK)
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
  //cap.printCapabilities(0);
  cap.printSupportedPixelFormats();
  VideoCaptureSettings cfg;
  cfg.width = 640;
  cfg.height = 480;
  cfg.width = 928;
  cfg.height = 616;
  cfg.in_pixel_format = AV_PIX_FMT_YUV420P;
  cfg.in_pixel_format = AV_PIX_FMT_UYVY422;
  cfg.in_pixel_format = AV_PIX_FMT_RGB24;
  cfg.out_pixel_format = AV_PIX_FMT_RGB24;
  cfg.fps = 30.00;

  if(cap.openDevice(0, cfg, on_frame, this)) {
    cap.startCapture();
  }
  RX_VERBOSE("%d x %d", cap.getWidth(), cap.getHeight());
  surface.setup(cap.getWidth(), cap.getHeight(), GL_RGBA, GL_RGB, GL_UNSIGNED_BYTE);
  pixels = new unsigned char[cap.getWidth() * cap.getHeight() * 3];
  
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
   if(has_new_frame) {
     surface.setPixels(pixels, cap.getWidth() * cap.getHeight() * 3);
     has_new_frame = false;
   }
   // RX_VERBOSE(".");
}

void Simulation::draw() {
  float r = 0.5 + sin(rx_millis() * 0.001) * 0.5;
  //  printf("%f\n", r);
  glClearColor(r, 0, 1-r, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  surface.draw(0,0);
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
  else if(key == GLFW_KEY_P) {
    can.getDevice()->print();
  }
  else if(key == GLFW_KEY_Q) {
    can.startLiveView();
  }
  else if(key == GLFW_KEY_W) {
    can.endLiveView();
  }
  else if(key == GLFW_KEY_C) {
    RX_VERBOSE("CLOSE DEVICE  ++++++++++++++++++++++++++++++++");
    can.closeDevice();
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
#if defined(USE_CANON)
  can.closeDevice();
#endif
  ::exit(EXIT_SUCCESS);
}

