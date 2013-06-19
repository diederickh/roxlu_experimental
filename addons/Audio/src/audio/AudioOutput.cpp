#include <roxlu/core/Log.h>
#include <audio/AudioOutput.h>


int audio_output_callback(const void* input, void* output, unsigned long numFrames,
                          const PaStreamCallbackTimeInfo* time,
                          PaStreamCallbackFlags status, void* user) 
{
  AudioOutput* out = static_cast<AudioOutput*>(user);
  size_t nread = 0;
  std::vector<SoundInfo*> to_remove;

  for(std::vector<SoundInfo*>::iterator it = out->sounds_info.begin(); it != out->sounds_info.end(); ++it) {

    SoundInfo* si = *it;
    if(si->state != AS_STATE_PLAYING) {
      continue;
    }

    si->stream->gotoFrame(si->read_frames);
    nread = si->stream->read(&output, numFrames);
    RX_VERBOSE("READ: %ld", nread);

    if(nread == 0) {
      to_remove.push_back(si);
    }
    else {
      si->read_frames += nread;
    }
  }

  uv_mutex_lock(&out->mutex);
  for(std::vector<SoundInfo*>::iterator it = to_remove.begin(); it != to_remove.end(); ++it) {
    out->removeSoundInfo(*it);
  }
  uv_mutex_unlock(&out->mutex);

  return paContinue;
}

void audio_output_stream_end_callback(void* user) {
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
  :output_stream(NULL)
  ,state(AO_STATE_NONE)
{
  if(!rx_is_audio_initialized()) {
    RX_ERROR(ERR_AUDIO_NOT_INITIALIZED);
    ::exit(EXIT_FAILURE);
  }

  uv_mutex_init(&mutex);
}

AudioOutput::~AudioOutput() {
  RX_VERBOSE("CLOSE / FREE OUTPUT STREAM");
}

bool AudioOutput::openOutputStream(int device, int numChannels, PaSampleFormat format, 
                                   double samplerate, unsigned long framesPerBuffer) 
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

  params.channelCount = numChannels;
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

  if(state != AO_STATE_OPENED) {
    RX_ERROR("Before starting the output stream, open it");
    return false;
  }

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
  /*

  if(st->getState() == AS_STATE_PLAYING) {
    RX_ERROR("Already playing sound: %d", id);
    return false;
  }
  st->setState(AS_STATE_PLAYING);
  */
  return true;
}

bool AudioOutput::stopSound(int id) {

  AudioStream* st = getSound(id);
  if(!st) {
    RX_ERROR("Cannot find the given sound, with id: %d", id);
    return false;
  }

  if(st->getState() == AS_STATE_NONE) {
    RX_ERROR("Cannot stop because we're not playing this sound: %d", id);
    return false;
  }

  st->setState(AS_STATE_NONE);

  return true;
}

AudioStream* AudioOutput::getSound(int id) {
  std::map<int, AudioStream*>::iterator it = sounds.find(id);
  if(it == sounds.end()) {
    return NULL;
  }
  return it->second;
}

// may only be called from the audio out called + make sure to wrap it around a mutex
bool AudioOutput::removeSoundInfo(SoundInfo* si) {
  std::vector<SoundInfo*>::iterator it = std::find(sounds_info.begin(), sounds_info.end(), si);
  if(it == sounds_info.end()) {
    RX_ERROR("Cannot remove sound object because we didn't find it");
    return false;
  }

  sounds_info.erase(it);
  delete *it;
  return true;
}

