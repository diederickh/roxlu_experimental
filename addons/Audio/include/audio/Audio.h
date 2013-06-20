#ifndef ROXLU_PORTAUDIO_H
#define ROXLU_PORTAUDIO_H

#include <string>
#include <portaudio.h>
#include <audio/PCMWriter.h>


/* 
   Created this class solely for microphone input... needs some design changes to be used
   for audio output
	
   Add the following frameworks on Mac:

   - CoreAudio
   - AudioToolbox
   - AudioUnit
   - CoreServices

   If you want to use this on Windows, you need to install the DirectX SDK 2010

   input callback:
   ---------------
   The callback for audio input. If you're asking for multiple channel input, the 
   input will probably be interleaved, so [chan0][chan1][chan0][chan1]etc..

*/

#define ERR_AUDIO_NOT_INITIALIZED "Cannot use audio input or output. Make sure to call `rx_init_audio()` before using any of the audio features is used; do this first thing in main.cpp ."
#define ERR_AUDIO_INIT "cannot initialize port audio: %s"
#define ERR_AUDIO_IN_STOP "cannot stop audio input stream: %s"
#define ERR_AUDIO_PORT "cannot terminate port audio: %s"
#define ERR_AUDIO_DEV_COUNT "Pa_CountDevices returned: %d"
#define ERR_AUDIO_UNKNOWN_DEV "unknown device id: %d"
#define ERR_AUDIO_IN_FORMAT_NOT_SUPPORTED "ERROR: input format is not supported for this device: %d"
#define ERR_AUDIO_PORTAUDIO_MSG "portaudio message: %s"
#define ERR_AUDIO_START_INPUT "cannot start input stream, no callback set."
#define ERR_AUDIO_IN_NOT_OPENED "cannot start input stream which hasnt been opened yet"
#define ERR_AUDIO_STREAM_NOT_CREATED "cannot stop a stream which isnt' created yet"
#define ERR_AUDIO_INVALID_DEV "Invalid device"
#define ERR_AUDIO_INVALID_CHANNEL_COUNT "Invalid channel count"
#define ERR_AUDIO_NO_OUTPUT_CB_SET "no output callback set"

#define VER_AUDIO_FORMAT_SUPPORTED_Y "+ [%d] supports channels: %d, format: %s, samplerate: %d"
#define VER_AUDIO_FORMAT_SUPPORTED_N "- [%d] supports channels: %d, format: %s, samplerate: %d"
#define VER_AUDIO_DEV_INFO "[%d] = %s, max in channels: %d, max out channels: %d, default samplerate: %f"
#define VER_AUDIO_IN_CLOSED "closed input audio stream"

typedef void(*cb_audio_in)(const void* input, unsigned long nframes, void* user);

int audio_in_callback(const void* input, void* output, unsigned long nframes,
                      const PaStreamCallbackTimeInfo* time, 
                      PaStreamCallbackFlags status, void* userData);


void audio_out_end_callback(void* user);

class Audio {
 public:
  Audio();
  ~Audio();
  int listDevices();

  /* input */
  int getDefaultInputDevice();
  bool isInputFormatSupported(int device, int numChannels, PaSampleFormat format, double samplerate);
  bool openInputStream(int device, int numChannels, PaSampleFormat format, double samplerate, unsigned long framesPerBuffer);
  bool startInputStream();
  bool stopInputStream();
  void setInputListener(cb_audio_in inCB, void* inData); 

  /* output */
  int getDefaultOutputDevice();

 public:
  std::string getSampleFormatText(PaSampleFormat f);  

  /* input */
  PaStream* input_stream;                              /* input stream handle (if setup) */
  cb_audio_in in_cb;                                   /* gets called when we get audio, make sure to not do heavy stuff in your callback! */
  void* in_user;                                       /* gets passed to your in callback */
};


// ------------------------------------------------------------------

extern bool rx_is_audio_initialized_flag;              /* make sure audio is only initialize once */

bool rx_is_audio_initialized();                        /* returns true when the audio backend has been initialize (rx_audio_init() has been called) */
bool rx_init_audio();                                  /* initializes the audio */
int rx_get_default_audio_output_device();              /* returns the index/id of the default audio output device */
int rx_get_default_audio_input_device();               /* returns the index/id of the default audio input device */

#endif
