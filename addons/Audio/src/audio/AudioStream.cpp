#include <audio/AudioStream.h>

AudioStream::AudioStream() {
}

AudioStream::~AudioStream() {
  RX_VERBOSE("~AudioStream()");
}

// -------------------------------------------------------------------------------------------

AudioStreamFile::AudioStreamFile()
  :AudioStream()
{
}

AudioStreamFile::~AudioStreamFile() {
}

bool AudioStreamFile::load(std::string filename, bool datapath) {
  if(!audio_file.load(filename, datapath)) {
    RX_ERROR("Cannot load file.");
    return false;
  }
  return true;
}

size_t AudioStreamFile::read(void** input, unsigned long nframes) {
  return audio_file.readFrames(*input, nframes);
}

void AudioStreamFile::gotoFrame(int frame) {
  audio_file.seek(frame, SEEK_SET);
}

// -------------------------------------------------------------------------------------------

AudioStreamMemory::AudioStreamMemory() 
  :index(0)
  ,num_channels(0)
  ,format(0)
  ,samplerate(0)
{
}

AudioStreamMemory::~AudioStreamMemory() {
  index = 0;
  num_channels = 0;
  format = 0;
  samplerate = 0;
}

bool AudioStreamMemory::load(std::string filename, bool datapath) {

  AudioFile af;
  if(!af.load(filename, datapath)) {
    RX_ERROR("Cannot load audio file: '%s'", filename.c_str());
    return false;
  }

  const int tmp_buffer_size = 1024 * 8;
  short tmp_buffer[tmp_buffer_size];
  sf_count_t count = 0;
  do {
    count = af.readItems((void*)tmp_buffer, tmp_buffer_size);
    if(count) {
      std::copy(tmp_buffer, tmp_buffer+count, std::back_inserter(buffer));
    }
  } while(count != 0);

  num_channels = af.getNumChannels();
  format = af.getFormat();
  samplerate = af.getSampleRate();

  return true;
}

size_t AudioStreamMemory::read(void** input, unsigned long nframes) {
  assert(num_channels);
  assert(format);

  if(index >= buffer.size()) {
    return 0;
  }

  size_t indices_needed = nframes * num_channels;
  size_t indices_left = buffer.size() - index;
  if(indices_needed > indices_left) {
    indices_needed = indices_left;
  }
  
  memcpy((char*)*input, &buffer[index], nframes * num_channels * sizeof(short));

  index += indices_needed;

  return indices_needed / num_channels;
}


