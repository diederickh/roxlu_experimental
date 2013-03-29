#include <Simulation.h>

void audio_in_cb(const void* input, unsigned long nframes, void* user) {
  printf("Audio in: %d\n", nframes);
  Simulation* s = static_cast<Simulation*>(user);
  if(s) {
    s->pcm.onAudioIn(input, nframes);

    size_t num_bytes = nframes * sizeof(short int) * s->mp3.config.num_channels;
    s->mp3.addAudioMono((const short int*)input, num_bytes, nframes);
  }
}

Simulation::Simulation()
  :SimulationBase()
  ,mp3_file_writer(rx_strftime("%Y_%m_%d_%H_%M_%S.mp3"), true)
{
  int seconds_to_record = 5;
  int samplerate = 16000;
  int num_channels = 1;
  int max_frames = 320;

  // PCM
  // ---------------------

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



  // MP3
  // ---------------------

  // Create the configuration object, to enable all features for the mp3
  MP3WriterConfig mp3_config;
  mp3_config.bitrate = 192;
  mp3_config.quality = 5;
  mp3_config.num_channels = num_channels;
  mp3_config.samplerate = MP3_WR_SAMPLERATE_16000;
  mp3_config.mode = MP3_WR_MODE_MONO;

  // add some id3 tag info
  mp3_config.id3_artist = "roxlu";
  mp3_config.id3_title = "roxlu example recording";
  mp3_config.id3_comment = "created with roxlu lib";
  mp3_config.id3_album = "roxlu";
  mp3_config.id3_year = rx_strftime("%Y");
  mp3_config.id3_track = "1";

  // set the encoder callbacks (THESE MUST BE SET!)
  mp3_config.user = &mp3_file_writer;
  mp3_config.cb_open = mp3_file_writer.cb_open;
  mp3_config.cb_close = mp3_file_writer.cb_close;
  mp3_config.cb_data = mp3_file_writer.cb_data;

  if(!mp3.setup(mp3_config)) {
    printf("ERROR: cannot setup mp3\n");
    ::exit(EXIT_FAILURE);
  }
  
  if(!mp3.begin()) {
    printf("ERROR: cannot open and initialize MP3\n");
    ::exit(EXIT_FAILURE);
  }


  // START
  // ---------------------
  audio.startInputStream();
}

Simulation::~Simulation() {
  audio.stopInputStream();
  mp3.end(); 
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
