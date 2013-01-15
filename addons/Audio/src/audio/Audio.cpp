#include <audio/Audio.h>

Audio::Audio() 
  :input_stream(NULL)
  ,in_cb(NULL)
  ,in_user(NULL)
{
  PaError err = Pa_Initialize();
  if(err != paNoError) {
    printf("ERROR: cannot initialize port audio.\n");
    printf("ERROR: portaudio message: %s\n", Pa_GetErrorText(err));
    ::exit(0);
  }
}

Audio::~Audio() {
  PaError err = Pa_Terminate();
  if(err != paNoError) {
    printf("ERROR: cannot terminate port audio.\n");
    printf("ERROR: portaudio message: %s\n", Pa_GetErrorText(err));
  }

}

int Audio::listDevices() {
  int num = Pa_GetDeviceCount();
  if(num <= 0) {
    printf("ERROR: Pa_CountDevices returned: %d\n", num);
    return -1;
  };

  const PaDeviceInfo* dev_info;
  for(int i = 0; i < num; ++i) {
    dev_info = Pa_GetDeviceInfo(i);
    printf("[%d] = %s, max in channels: %d, max out channels: %d, default samplerate: %f\n"
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
    printf("+ [%d] supports channels: %d, format: %s, samplerate: %d\n", device, numChannels, getSampleFormatText(format).c_str(), int(samplerate));
    return true;
  }
  else {
    printf("- [%d] supports channels: %d, format: %s, samplerate: %d\n", device, numChannels, getSampleFormatText(format).c_str(), int(samplerate));
    return false;
  }
}

std::string Audio::getSampleFormatText(PaSampleFormat f) {
  std::string r = "";
  switch(f) {
    case paFloat32: r = "F32"; break;
    case paInt32: r = "I32"; break;
    case paInt24: r = "I24"; break;
    case paInt16: r = "I16"; break;
    case paInt8: r = "I8"; break;
    case paUInt8: r = "UI8"; break;
    case paCustomFormat: "CustomFormat"; break;
    default: r = "unknown"; break;
  };
  return r;
}



// --------------------------------------------------
/**
 * Open an input stream.
 * @param PaSampleFormat:
 *        - paInt16 
 *        - paFloat32: in a range of -1.0 / +1.0
 */
bool Audio::openInputStream(
                            int device
                            ,int numChannels
                            ,PaSampleFormat format
                            ,double samplerate
                            ,unsigned long framesPerBuffer
                            )
{
  PaDeviceIndex total_devices = Pa_GetDeviceCount();
  if(!total_devices || device >= total_devices) {
    printf("ERROR: unknown device id: %d\n", device);
    return false;
  }

  if(!isInputFormatSupported(device, numChannels, format, samplerate)) {
    printf("ERROR: input format is not supported for this device: %d\n", device);
    return false;
  }

  PaStreamParameters input;
  memset(&input, 0, sizeof(input));

  input.channelCount = numChannels;
  input.device = device;
  input.hostApiSpecificStreamInfo = NULL;
  input.sampleFormat = format;
  input.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;

  PaError err = Pa_OpenStream(
                              &input_stream
                              ,&input
                              ,NULL
                              ,samplerate
                              ,framesPerBuffer
                              //                              ,paNoFlag
                              ,paClipOff
                              ,&Audio::inputStreamCallback
                              ,(void*) this
                              );
  if(err != paNoError) {
    printf("ERROR: portaudio message: %s\n", Pa_GetErrorText(err));
    return false;
  }
									 
  return true;
}

int Audio::inputStreamCallback(
                               const void* input
                               ,void* output
                               ,unsigned long numFrames
                               ,const PaStreamCallbackTimeInfo* time
                               ,PaStreamCallbackFlags status
                               ,void* userData)
{
  Audio* a = static_cast<Audio*>(userData);
  if(a->in_cb) {
    a->in_cb(input, numFrames, a->in_user);
  }
  return 0; // 0 = continue, 1 = stop
}


bool Audio::startInputStream() {
  if(input_stream == NULL) {
    printf("ERROR: cannot start input stream which hasnt been opened yet.\n");
    return false;
  }
  PaError err = Pa_StartStream(input_stream);
  if(err != paNoError) {
    printf("ERROR: portaudio message: %s\n", Pa_GetErrorText(err));
    return false;
  }
  return true;

}

bool Audio::stopInputStream() {
  if(input_stream == NULL) {
    printf("ERROR: cannot stop a stream which isnt' created yet.\n");
    return false;
  }
  PaError err = Pa_StopStream(input_stream);
  if(err != paNoError) {
    printf("ERROR: portaudio message: %s\n", Pa_GetErrorText(err));
    return false;
  }
  return true;
}
/*
void Audio::setInputListener(AudioListener* listener) {
  input_listener = listener;
}
*/
void Audio::setInputListener(cb_audio_in inCB, void* inUser) {
  in_cb = inCB;
  in_user = inUser;
}
