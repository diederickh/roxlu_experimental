#include <roxlu/core/Log.h>
#include <audio/AudioOutput.h>


int audio_output_callback(const void* input, void* output, unsigned long numFrames,
                          const PaStreamCallbackTimeInfo* time,
                          PaStreamCallbackFlags status, void* user) 
{
  
  AudioOutput* out = static_cast<AudioOutput*>(user);
  assert(out->num_channels);

  size_t nread = 0;
  std::vector<SoundInfo*> to_remove;
  size_t nbytes = out->num_channels * numFrames * sizeof(float);
  memset((char*)output, 0x00, nbytes);

  if(!out->sounds_info.size()) {
    return paContinue;
  }

  std::vector<SoundInfo*> sounds;
  uv_mutex_lock(&out->mutex);
  {
    std::copy(out->sounds_info.begin(), out->sounds_info.end(), std::back_inserter(sounds));
  }
  uv_mutex_unlock(&out->mutex);


  for(std::vector<SoundInfo*>::iterator it = sounds.begin(); it != sounds.end(); ++it) {

    SoundInfo* si = *it;
    if(si->state != AS_STATE_PLAYING) {
      continue;
    }

    si->stream->gotoFrame(si->read_frames);
    nread = si->stream->accumulate(output, numFrames);

    if(nread == 0) {
      si->state = AS_STATE_NONE;
      delete si;
      to_remove.push_back(si);
    }
    else {
      si->read_frames += nread;
    }
  }

  uv_mutex_lock(&out->mutex);
  {
    for(std::vector<SoundInfo*>::iterator it = to_remove.begin(); it != to_remove.end(); ++it) {
      out->removeSoundInfo(*it);
    }
  }
  uv_mutex_unlock(&out->mutex);

  return paContinue;
}

void audio_output_stream_end_callback(void* user) {
  AudioOutput* output = static_cast<AudioOutput*>(user);
  output->shutdown();
}

// ------------------------------------------------------------------

SoundInfo::SoundInfo()
  :stream(NULL)
  ,state(AS_STATE_NONE)
  ,read_frames(0)
{
}

SoundInfo::~SoundInfo() {
  read_frames = 0;
  state = AS_STATE_NONE;
  stream = NULL;
 
}

// ------------------------------------------------------------------

AudioOutput::AudioOutput() 
  :num_channels(0)
  ,format(0)
  ,samplerate(0)
  ,frames_per_buffer(0)
  ,state(AO_STATE_NONE)
  ,output_stream(NULL)
{
  if(!rx_is_audio_initialized()) {
    RX_ERROR(ERR_AUDIO_NOT_INITIALIZED);
    ::exit(EXIT_FAILURE);
  }

  if(uv_mutex_init(&mutex) < 0) {
    RX_ERROR("Cannot initialize the mutex in the audio output");
    ::exit(EXIT_FAILURE);
  }
}

AudioOutput::~AudioOutput() {
  uv_mutex_lock(&mutex);

  for(std::vector<SoundInfo*>::iterator it = sounds_info.begin(); it != sounds_info.end(); ++it) {
    (*it)->state = AS_STATE_NONE;
  }
  uv_mutex_unlock(&mutex);

  if(output_stream) {
    if(!stopOutputStream() && !closeOutputStream()) {
      shutdown();
      output_stream = NULL;
    }
  }
  uv_mutex_destroy(&mutex);
}

// Shutdown; will be either called though the end stream callback or from destructor
void AudioOutput::shutdown() {
  for(std::vector<SoundInfo*>::iterator it = sounds_info.begin(); it != sounds_info.end(); ++it) {
    delete *it;
  }

  num_channels = 0;
  format = 0;
  samplerate = 0;
  frames_per_buffer = 0;
  state = AO_STATE_NONE;

  sounds_info.clear();
  sounds.clear();
}

bool AudioOutput::openOutputStream(int device, int numChannels, PaSampleFormat audioFormat, 
                                   double audioSamplerate, unsigned long framesPerBuffer) 
{


  if(output_stream) {
    RX_ERROR("We already opened an output stream. You can only open one output stream for now");
    return false;
  }

  PaStreamParameters params;
  PaError err;
  
  params.device = device;

  if(params.device == paNoDevice) {
    RX_ERROR("Invalid audio devive");
    return false;
  }

  if(!numChannels) {
    RX_ERROR("Invalid number of audio channels: %d", numChannels);
    return false;
  }

  num_channels = numChannels;
  format = audioFormat;
  samplerate = audioSamplerate;
  frames_per_buffer = framesPerBuffer;

  params.channelCount = num_channels;
  params.sampleFormat = format;
  params.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowOutputLatency;
  params.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(&output_stream, NULL, &params, 
                      samplerate, framesPerBuffer, paClipOff, 
                      audio_output_callback, this);

  if(err != paNoError) {
    RX_ERROR("Error while trying to open the output stream: %s", Pa_GetErrorText(err));
    return false;
  }

  err = Pa_SetStreamFinishedCallback(output_stream, audio_output_stream_end_callback);
  if(err != paNoError) {
    RX_ERROR("Error while trying to set the output end stream callback", Pa_GetErrorText(err));
    return false;
  }

  state = AO_STATE_OPENED;

  return true;
}

bool AudioOutput::startOutputStream() {

  if(!output_stream) {
    RX_ERROR("Cannot start the output stream because we havent been setup");
    return false;
  }

  PaError err;
  err = Pa_StartStream(output_stream);
  if(err != paNoError) {
    RX_ERROR("Error while trying to open the output stream: %s", Pa_GetErrorText(err));
    return false;
  }
  
  state = AO_STATE_STARTED;
  
  return true;
}

bool AudioOutput::stopOutputStream() {

  if(state != AO_STATE_STARTED) {
    RX_ERROR("Cannot stop a stream which havent been started yet");
    return false;
  }

  if(!output_stream) {
    RX_ERROR("Cannot stop the output stream because we havent been setup");
    return false;
  }

  PaError err;
  err = Pa_StopStream(output_stream);
  if(err != paNoError) {
    RX_ERROR("Cannot stop the output stream: %s", Pa_GetErrorText(err));
    return false;
  }

  state = AO_STATE_OPENED; // we're still open, just stopped

  return true;
}

bool AudioOutput::closeOutputStream() {
  if(state != AO_STATE_OPENED) {
    RX_ERROR("Cannot close the stream. Did you open the stream? Make sure to open() and stop() before calling close()");
    return false;
  }

  PaError err = Pa_CloseStream(output_stream);
  if(err != paNoError) {
    RX_ERROR("Cannot close the stream: %s (%p)", Pa_GetErrorText(err), output_stream);
    return false;
  }

  output_stream = NULL;

  state = AO_STATE_NONE;
  return true;
}


bool AudioOutput::addSound(int id, AudioStream* io) {
  sounds.insert(std::pair<int, AudioStream*>(id, io));
  return true;
}

bool AudioOutput::playSound(int id) {

  AudioStream* st = getSound(id);
  if(!st) {
    RX_ERROR("Cannot find the given sound, with id: %d", id);
    return false;
  }
  
  SoundInfo* si = new SoundInfo();
  si->stream = st;
  si->state = AS_STATE_PLAYING;

  uv_mutex_lock(&mutex);
  sounds_info.push_back(si);
  uv_mutex_unlock(&mutex);
  return true;
}

bool AudioOutput::stopSound(int id) {
  RX_ERROR("stopSound not yet implemented!");
  /*
  AudioStream* st = getSound(id);
  if(!st) {
    RX_ERROR("Cannot find the given sound, with id: %d", id);
    return false;
  }
  */
  return false;
}

AudioStream* AudioOutput::getSound(int id) {
  AudioStream* found_stream = NULL;

  uv_mutex_lock(&mutex);
  {
    std::map<int, AudioStream*>::iterator it = sounds.find(id);
    if(it == sounds.end()) {
      return NULL;
    }
    found_stream = it->second;
  }
  uv_mutex_unlock(&mutex);

  return found_stream;
}

// we only remove the item from the vector here; this is called from the audio output callback 
// we deallocate memory in the output callback because calling delete here, gives unexpected
// behavior.
bool AudioOutput::removeSoundInfo(SoundInfo* si) {
  std::vector<SoundInfo*>::iterator it = std::find(sounds_info.begin(), sounds_info.end(), si);
  if(it == sounds_info.end()) {
    RX_ERROR("Cannot remove sound object because we didn't find it");
    return false;
  }
  sounds_info.erase(it);
  return true;
}

