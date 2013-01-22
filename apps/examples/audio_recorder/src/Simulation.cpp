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
{
  int seconds_to_record = 5;
  int samplerate = 16000;
  int num_channels = 1;
  int max_frames = 320;

  // The PCM writer creates a ringbuffer into which is stored the data. If you record longer then 
  // seconds_to_record, previous recorded audio will be overwritten.
  pcm.open(File::toDataPath("input.pcm"), 
           seconds_to_record, 
           samplerate, 
           num_channels, 
           sizeof(short int));

  audio.setInputListener(audio_in_cb, this);

  audio.listDevices();

  audio.openInputStream(1,  // use device 1, check if this one is capable of using the given specs
                        num_channels,  // number of input channels
                        paInt16,  // the format (see Portaudio documentation)
                        samplerate, // samplerate of recording
                        max_frames); // max frames we get in our 'audio_in_cb'

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
