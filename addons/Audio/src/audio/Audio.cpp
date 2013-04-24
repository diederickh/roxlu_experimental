#include <audio/Audio.h>

Audio::Audio() 
  :input_stream(NULL)
  ,in_cb(NULL)
  ,in_user(NULL)
  ,output_stream(NULL)
  ,out_cb(NULL)
  ,out_user(NULL)
{
  PaError err = Pa_Initialize();
  if(err != paNoError) {
    RX_ERROR(ERR_AUDIO_INIT,  Pa_GetErrorText(err));
    ::exit(0);
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

  PaError err = Pa_Terminate();
  if(err != paNoError) {
    RX_ERROR(ERR_AUDIO_PORT, Pa_GetErrorText(err));
  }

  if(output_stream) {
    PaError err = Pa_StopStream(output_stream);
    if(err != paNoError) {
      RX_ERROR(ERR_AUDIO_OUT_STOP, Pa_GetErrorText(err));
    }
    else {
      RX_VERBOSE(VER_AUDIO_OUT_CLOSED);
    }
    output_stream = NULL;
  }

  in_cb = NULL;
  in_user = NULL;
  out_cb = NULL;
  out_user = NULL;
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
    case paFloat32:      r = "F32"; break;
    case paInt32:        r = "I32"; break;
    case paInt24:        r = "I24"; break;
    case paInt16:        r = "I16"; break;
    case paInt8:         r = "I8"; break;
    case paUInt8:        r = "UI8"; break;
    case paCustomFormat: r = "CustomFormat"; break;
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

bool Audio::openOutputStream(int device, int numChannels, PaSampleFormat format, 
                             double samplerate, unsigned long framesPerBuffer)
{
  PaStreamParameters params;
  PaError err;
  
  params.device = device;

  if(params.device == paNoDevice) {
    RX_ERROR(ERR_AUDIO_INVALID_DEV);
    return false;
  }

  if(!numChannels) {
    RX_ERROR(ERR_AUDIO_INVALID_CHANNEL_COUNT);
    return false;
  }

  params.channelCount = numChannels;
  params.sampleFormat = format;
  params.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowOutputLatency;
  params.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(&output_stream, NULL, &params, 
                      samplerate, framesPerBuffer, paClipOff, 
                      audio_out_callback, this);

  if(err != paNoError) {
    RX_ERROR(ERR_AUDIO_PORTAUDIO_MSG, Pa_GetErrorText(err));
    return false;
  }

  err = Pa_SetStreamFinishedCallback(output_stream, &audio_out_end_callback);
  if(err != paNoError) {
    RX_ERROR(ERR_AUDIO_PORTAUDIO_MSG, Pa_GetErrorText(err));
    return false;
  }


  return true;
}

bool Audio::startOutputStream() {
  if(!out_cb) {
    RX_ERROR(ERR_AUDIO_NO_OUTPUT_CB_SET);
    return false;
  }

  if(!output_stream) {
    RX_ERROR(ERR_AUDIO_OUT_NOT_OPENED);
    return false;
  }

  PaError err;
  err = Pa_StartStream(output_stream);
  if(err != paNoError) {
    RX_ERROR(ERR_AUDIO_PORTAUDIO_MSG, Pa_GetErrorText(err));
    return false;
  }

  return true;
}

bool Audio::stopOutputStream() {
  if(!output_stream) {
    RX_ERROR(ERR_AUDIO_OUT_NOT_OPENED);
    return false;
  }

  PaError err;
  err = Pa_StopStream(output_stream);
  if(err != paNoError) {
    RX_ERROR(ERR_AUDIO_PORTAUDIO_MSG, Pa_GetErrorText(err));
    return false;
  }

  return true;
}

void Audio::setOutputCallback(cb_audio_out outCB, void* outUser) {
  out_cb = outCB;
  out_user = outUser;
}

// -------------------------------------------------------------------------------------------

int audio_out_callback(const void* input, void* output, unsigned long nframes,
                       const PaStreamCallbackTimeInfo* time,
                       PaStreamCallbackFlags status, void* user)
{
  Audio* a = static_cast<Audio*>(user);
  a->out_cb(output, nframes, a->out_user);
  return paContinue;
}

void audio_out_end_callback(void* user) {

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

