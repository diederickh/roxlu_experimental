#include <Simulation.h>

void audio_in_cb(const void* input, unsigned long nframes, void* user) {
  Simulation* s = static_cast<Simulation*>(user);
  s->recorder.addAudio((void*)input, nframes);
}

Simulation::Simulation()
  :SimulationBase()
{
}

Simulation::~Simulation() {
  recorder.stop();
}

void Simulation::setup() {
  setWindowTitle("FLV recorder");

  setupCapture();
  setupRecorder();
  setupAudio();

  recorder.open(rx_to_data_path(rx_strftime("%Y_%m_%d_%H_%M_%S") +".flv"));
  recorder.start();

}

void Simulation::update() {
  cap.update();
}


void Simulation::draw() {
  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(cap.hasNewData()) {
    cap_surf.setPixels(cap.getNewDataPtr(), cap.getNumBytes());
  }

  cap_surf.draw(0,0,VIDEO_W, VIDEO_H);

  fps.draw();

  recorder.grabFrame();
}

void Simulation::setupAudio() {
  audio.setInputListener(audio_in_cb, this);
  audio.listDevices();
  audio.openInputStream(1, AUDIO_NUM_CHANNELS, AUDIO_FORMAT, AUDIO_SAMPLERATE, AUDIO_MAX_SAMPLES);
  audio.startInputStream();
}

void Simulation::setupCapture() {
  cap.listDevices();

  bool r = cap.openDevice(0, VIDEO_W, VIDEO_H, VC_FMT_YUYV422); //  VC_FMT_YUYV422); // format might differ on windows (VC_FMT_UYVY422)

  if(!r) {
    RX_ERROR(("ERROR: cannot open video capture device."));
    ::exit(EXIT_FAILURE);
  }

  r = cap.startCapture();
  if(!r) {
    RX_ERROR(("ERROR: cannot start video capture.n"));
    ::exit(EXIT_FAILURE);
  }
  cap_surf.setup(VIDEO_W, VIDEO_H);
  cap_surf.flip(false, true);
}

void Simulation::setupRecorder() {

  FLVScreenRecorderSettings settings;
  settings.vid_in_w = VIDEO_W;
  settings.vid_in_h = VIDEO_H;
  settings.vid_out_w = settings.vid_in_w;
  settings.vid_out_h = settings.vid_in_h;
  settings.vid_fps = VIDEO_FPS;
  settings.audio_num_channels = AUDIO_NUM_CHANNELS;
  settings.audio_samplerate = AUDIO_SAMPLERATE;
  settings.audio_max_samples = AUDIO_MAX_SAMPLES;
  settings.audio_format = AV_FMT_INT16;

  if(!recorder.setup(settings)) {
    RX_ERROR(("ERROR: cannot setup recorder"));
    ::exit(EXIT_FAILURE);
  }
}

void Simulation::onMouseDown(int x, int y, int button) {}
void Simulation::onMouseUp(int x, int y, int button) { }
void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {}
void Simulation::onMouseMove(int x, int y) {}
void Simulation::onKeyDown(int key) {}
void Simulation::onKeyUp(int key) {}
void Simulation::onWindowClose() {}

