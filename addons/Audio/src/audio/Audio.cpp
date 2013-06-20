#include <audio/Audio.h>

bool rx_is_audio_initialized_flag = false;

// Initializes the audio backend (port audio)
bool rx_init_audio() {

  if(rx_is_audio_initialized_flag) {
    return true;
  }

  PaError err = Pa_Initialize();
  if(err != paNoError) {
    RX_ERROR(ERR_AUDIO_INIT,  Pa_GetErrorText(err));
    return false;
  }

  rx_is_audio_initialized_flag = true;

  return true;
}

bool rx_is_audio_initialized() {
  return rx_is_audio_initialized_flag;
}

int rx_get_default_audio_output_device() {
  return Pa_GetDefaultOutputDevice();
}

int rx_get_default_audio_input_device() {
  return Pa_GetDefaultInputDevice();
}
// ------------------------------------------------------------------


Audio::Audio() 
  :input_stream(NULL)
  ,in_cb(NULL)
  ,in_user(NULL)
{
  if(!rx_is_audio_initialized()) {
    RX_ERROR(ERR_AUDIO_NOT_INITIALIZED);
    ::exit(EXIT_FAILURE);
  }
}

Audio::~Audio() {

  if(input_stream) {
    PaError err = Pa_StopStream(input_stream);
    if(err != paNoError) {
      RX_ERROR(ERR_AUDIO_IN_STOP, Pa_GetErrorText(err));
    }
    else {
      RX_VERBOSE(VER_AUDIO_IN_CLOSED);
    }
    input_stream = NULL;
  }

  in_cb = NULL;
  in_user = NULL;
}

int Audio::listDevices() {
  int num = Pa_GetDeviceCount();
  if(num <= 0) {
    RX_ERROR(ERR_AUDIO_DEV_COUNT, num);
    return -1;
  };

  const PaDeviceInfo* dev_info;
  for(int i = 0; i < num; ++i) {
    dev_info = Pa_GetDeviceInfo(i);

    RX_VERBOSE(VER_AUDIO_DEV_INFO 
               ,i
               ,dev_info->name
               ,dev_info->maxInputChannels
               ,dev_info->maxOutputChannels
               ,dev_info->defaultSampleRate);
  }

  return -1;
}

int Audio::getDefaultInputDevice() {
  return Pa_GetDefaultInputDevice();
}

bool Audio::isInputFormatSupported(int device, int numChannels, PaSampleFormat format, double samplerate) {
  PaStreamParameters input;
  PaError err;

  input.channelCount = numChannels;
  input.device = device;
  input.hostApiSpecificStreamInfo = NULL;
  input.sampleFormat = format;
  input.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;

  err = Pa_IsFormatSupported(&input, NULL, samplerate);
  if(err == paFormatIsSupported) {
    RX_VERBOSE(VER_AUDIO_FORMAT_SUPPORTED_Y, device, numChannels, getSampleFormatText(format).c_str(), int(samplerate));
     return true;
  }
  else {
    RX_VERBOSE(VER_AUDIO_FORMAT_SUPPORTED_N, device, numChannels, getSampleFormatText(format).c_str(), int(samplerate));
    return false;
  }
}

std::string Audio::getSampleFormatText(PaSampleFormat f) {
  std::string r = "";

  switch(f) {
    // interleaved formats
    case paFloat32:      r = "F32"; break;
    case paInt32:        r = "I32"; break;
    case paInt24:        r = "I24"; break;
    case paInt16:        r = "I16"; break;
    case paInt8:         r = "I8"; break;
    case paUInt8:        r = "UI8"; break;
    case paCustomFormat: r = "CustomFormat"; break;

      // NON-interleaved formats, aka planar
    case (paFloat32|paNonInterleaved):      r = "F32P"; break;
    case (paInt32|paNonInterleaved):        r = "I32P"; break;
    case (paInt24|paNonInterleaved):        r = "I24P"; break;
    case (paInt16|paNonInterleaved):        r = "I16P"; break;
    default: r = "unknown"; break;
  };

  return r;
}


bool Audio::openInputStream(int device, int numChannels, PaSampleFormat format  
                            ,double samplerate, unsigned long framesPerBuffer)
{

  PaDeviceIndex total_devices = Pa_GetDeviceCount();
  if(!total_devices || device >= total_devices) {
    RX_ERROR(ERR_AUDIO_UNKNOWN_DEV, device);
    return false;
  }

  if(!isInputFormatSupported(device, numChannels, format, samplerate)) {
    RX_ERROR(ERR_AUDIO_IN_FORMAT_NOT_SUPPORTED, device);
    return false;
  }

  PaStreamParameters input;
  memset(&input, 0, sizeof(input));

  input.channelCount = numChannels;
  input.device = device;
  input.hostApiSpecificStreamInfo = NULL;
  input.sampleFormat = format;
  input.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;

  PaError err = Pa_OpenStream(&input_stream, &input, NULL
                              ,samplerate, framesPerBuffer, paClipOff
                              ,audio_in_callback, (void*) this);

  if(err != paNoError) {
    RX_ERROR(ERR_AUDIO_PORTAUDIO_MSG, Pa_GetErrorText(err));
    return false;
  }
									 
  return true;
}


bool Audio::startInputStream() {
  if(!in_cb) {
    RX_ERROR(ERR_AUDIO_START_INPUT);
    return false;
  }

  if(input_stream == NULL) {
    RX_ERROR(ERR_AUDIO_IN_NOT_OPENED);
    return false;
  }

  PaError err = Pa_StartStream(input_stream);
  if(err != paNoError) {
    RX_ERROR(ERR_AUDIO_PORTAUDIO_MSG, Pa_GetErrorText(err));
    return false;
  }

  return true;
}

bool Audio::stopInputStream() {
  if(input_stream == NULL) {
    RX_ERROR(ERR_AUDIO_STREAM_NOT_CREATED);
    return false;
  }

  PaError err = Pa_StopStream(input_stream);
  if(err != paNoError) {
    RX_ERROR(ERR_AUDIO_PORTAUDIO_MSG, Pa_GetErrorText(err));
    return false;
  }

  return true;
}

void Audio::setInputListener(cb_audio_in inCB, void* inUser) {
  in_cb = inCB;
  in_user = inUser;
}

int Audio::getDefaultOutputDevice() {
  return Pa_GetDefaultOutputDevice();
}

// -------------------------------------------------------------------------------------------
int audio_in_callback(const void* input, void* output, unsigned long nframes,
                      const PaStreamCallbackTimeInfo* time, 
                      PaStreamCallbackFlags status, void* userData)
{

  Audio* a = static_cast<Audio*>(userData);
  a->in_cb(input, nframes, a->in_user);
  return 0; // 0 = continue, 1 = stop
}

