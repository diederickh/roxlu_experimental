#include <audio/AudioStream.h>

AudioStream::AudioStream() 
  :state(AST_STATE_NONE)
{
}

AudioStream::~AudioStream() {
  state = AST_STATE_NONE;
}

// -------------------------------------------------------------------------------------------

AudioStreamFile::AudioStreamFile()
  :AudioStream()
{
}

AudioStreamFile::~AudioStreamFile() {
  state = AST_STATE_NONE;
}

bool AudioStreamFile::load(std::string filename, bool datapath) {
  if(!audio_file.load(filename, datapath)) {
    RX_ERROR("Cannot load file.");
    return false;
  }
  state = AST_STATE_OPEN;
  return true;
}

size_t AudioStreamFile::read(void* input, unsigned long nframes) {
  if(state != AST_STATE_OPEN) {
    RX_ERROR("Not yet opened");
    return 0;
  }
  return audio_file.readFrames(input, nframes);
}

size_t AudioStreamFile::accumulate(void* input, unsigned long nframes) {
  if(state != AST_STATE_OPEN) {
    RX_ERROR("Not yet opened");
    return 0;
  }
  RX_ERROR("ACCUMULATE NOT YET IMPLEMENTED FOR FILE STREAMS");
  return 0;
}

void AudioStreamFile::gotoFrame(int frame) {
  audio_file.seek(frame, SEEK_SET);
}

// -------------------------------------------------------------------------------------------

AudioStreamMemory::AudioStreamMemory() 
  :AudioStream()
  ,index(0)
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
  state = AST_STATE_NONE;
}

bool AudioStreamMemory::load(std::string filename, bool datapath) {

  AudioFile af;
  if(!af.load(filename, datapath)) {
    RX_ERROR("Cannot load audio file: '%s'", filename.c_str());
    return false;
  }

  const int tmp_buffer_size = 1024 * 8;
  float tmp_buffer[tmp_buffer_size];
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
  state = AST_STATE_OPEN;
  return true;
}

size_t AudioStreamMemory::read(void* output, unsigned long nframes) {

  if(state != AST_STATE_OPEN) {
    return 0;
  }

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
  
  memcpy((char*)output, &buffer[index], nframes * num_channels * sizeof(float));

  index += indices_needed;

  return indices_needed / num_channels;
}

size_t AudioStreamMemory::accumulate(void* output, unsigned long nframes) {

  if(state != AST_STATE_OPEN) {
    return 0;
  }

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
  
  float* output_buffer = (float*)output;
  for(size_t i = 0; i < indices_needed; ++i) {
    output_buffer[i] += buffer[index + i];
  }

  index += indices_needed;
  return indices_needed / num_channels;
}



