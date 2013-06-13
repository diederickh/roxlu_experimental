#include <Simulation.h>

// if you need access to the raw (`in`) or converted (`out`), pixels you can pass this callback to setup
static void webcam_frame(AVFrame* in, size_t insize, AVFrame* out, size_t outsize, void* user) {
}

Simulation::Simulation()
  :SimulationBase()
{
}

void Simulation::setup() {
  setWindowTitle("Simple video capture example");

  // Show a list of devices
  int num_devices = cam.listDevices();
  if(!num_devices) {
    RX_ERROR("No capture devices found.");
    ::exit(EXIT_FAILURE);
  }

  // Define exactly how we want our data:
  VideoCaptureSettings cfg;
  cfg.width = 640;
  cfg.height = 480;
  cfg.in_pixel_format = AV_PIX_FMT_YUYV422;

  // Open the device
  if(!cam.openDevice(0, cfg, webcam_frame, this)) {
    RX_VERBOSE("Cannot open the webcam");
    ::exit(EXIT_FAILURE);
  }

  if(!cam.startCapture()) {
    RX_ERROR("Cannot start the capture");
    ::exit(EXIT_FAILURE);
  }
}

void Simulation::update() {
  cam.update();
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  cam.draw(0,0,640,480);
  cam.draw(640,0,320,240);
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
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
  ::exit(EXIT_SUCCESS);
}

