#include <Simulation.h>

void audio_in_cb(const void* input, unsigned long nframes, void* user) {
  printf("Audio in: %d\n", nframes);
  Simulation* s = static_cast<Simulation*>(user);
  if(s) {
    s->pcm.onAudioIn(input, nframes);
  }
}

Simulation::Simulation()
  :SimulationBase()
  ,vid(1024,768,640,480,25, true, false)
{
  pcm.open(File::toDataPath("input.pcm"));
  audio.setInputListener(audio_in_cb, this);
  audio.openInputStream(1, 2, paInt16, 16000, 320);
  audio.startInputStream();
}

void Simulation::setup() {  setWindowTitle("Audio recorder example"); }
void Simulation::update() {}
void Simulation::draw() {}
void Simulation::onMouseDown(int x, int y, int button) {}
void Simulation::onMouseUp(int x, int y, int button) {}
void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {}
void Simulation::onMouseMove(int x, int y) {}
void Simulation::onKeyDown(char key) {}
void Simulation::onKeyUp(char key) {}
