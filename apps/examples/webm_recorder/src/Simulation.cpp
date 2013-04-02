#include <Simulation.h>

Simulation::Simulation()
  :SimulationBase()
{
}

Simulation::~Simulation() {
  rec.stop();
}

void Simulation::setup() {

  setWindowTitle("Webm recorder");

  // open the file to which we write the webm video
  if(!efile.open(rx_to_exe_path("recording_test.webm"))) {
    RX_ERROR("cannot open ebml destination file");
    ::exit(0);
  }

  // setup the settings for the encoder.
  WebmScreenRecorderSettings c;
  c.use_video = true;

  c.cb_write = ebml_file_write;
  c.cb_close = ebml_file_close;
  c.cb_peek = ebml_file_peek;
  c.cb_read = ebml_file_read;
  c.cb_skip = ebml_file_skip;
  c.cb_user = &efile;

  c.vid_in_w = VIDEO_W;
  c.vid_in_h = VIDEO_H;
  c.vid_out_w = VIDEO_W;
  c.vid_out_h = VIDEO_H;
  c.vid_fps = VIDEO_FPS;

  if(!rec.setup(c)) {
    RX_ERROR("cannot setup screen recorder");
    ::exit(0);
  }
  
  // we use a capture device (only tested on mac)
  if(!cap.openDevice(0, 640, 480, VC_FMT_YUYV422)) {
    RX_ERROR("cannot open webcam");
    ::exit(0);
  }

  if(!cap.startCapture()) {
    RX_ERROR("cannot start capture");
    ::exit(0);
  }

  cap_gl.setup(cap.getWidth(), cap.getHeight()); // , cap.getFormat());
}

void Simulation::update() {
  cap.update();
}


void Simulation::draw() {
  glDisable(GL_DEPTH_TEST);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(cap.hasNewData()) {
    cap_gl.setPixels(cap.getNewDataPtr(), cap.getNumBytes());
  }

  cap_gl.draw(0,0,640,480);
  fps.draw();
  rec.grabFrame();
}

void Simulation::onMouseDown(int x, int y, int button) {
}

void Simulation::onMouseUp(int x, int y, int button) {
}

void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {
}

void Simulation::onMouseMove(int x, int y) {
}

void Simulation::onKeyDown(int key) {
  if(key == 'S') {
    RX_VERBOSE("start stream");
    rec.start();
  }
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
}

