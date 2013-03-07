#include <audio/AudioFile.h>

AudioFile::AudioFile() 
  :handle(NULL)
  ,buffer(NULL)
  ,is_loaded(false)
  ,buffer_size(1024)
  ,num_frames(1024)
{
}

AudioFile::~AudioFile() {
  if(buffer) {
    delete[] buffer;
    buffer = NULL;
  }

  if(handle) {
    sf_close(handle);
    handle = NULL;
  }

  is_loaded = false;
}

bool AudioFile::load(std::string filename, bool datapath) {
  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  info.format = 0;
  handle = sf_open(filename.c_str(), SFM_READ, &info);

  if(!handle) {
    RX_ERROR(("Error loading: %s - %s", filename.c_str(), sf_strerror(NULL)));
    return false;
  }

  buffer_size = num_frames * info.channels * sizeof(short);
  buffer = new short[buffer_size];

  if(!buffer) {
    buffer = NULL;
    RX_ERROR(("Cannot allocate audio buffer"));
    return false;
  }
   
  is_loaded = true;
  return true;
}

sf_count_t AudioFile::readFrames(void* output, unsigned long nframes) {
  if(!is_loaded) {
    return 0;
  }

  sf_count_t num_read = sf_readf_short(handle, (short*)output, nframes);

  return num_read;
}

void AudioFile::print() {
  if(!is_loaded) {
    return;
  }

  RX_VERBOSE(("Samplerate: %d", info.samplerate));
  RX_VERBOSE(("Channels: %d", info.channels));
  RX_VERBOSE(("Frames: %lld", info.frames));
  RX_VERBOSE(("Format: %d", info.format));
  RX_VERBOSE(("Sections: %d", info.sections));
}
