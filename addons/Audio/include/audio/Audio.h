#ifndef ROXLU_PORTAUDIO_H
#define ROXLU_PORTAUDIO_H

#include <string>
#include <portaudio.h>
#include <audio/PCMWriter.h>
//#include "AudioListener.h"

/* 
   Created this class solely for microphone input... needs some design changes to be used
   for audio output
	
   Add the following frameworks:

   - CoreAudio
   - AudioToolbox
   - AudioUnit
   - CoreServices

*/

typedef void(*cb_audio_in)(const void* input, unsigned long nframes, void* user);

class Audio {
 public:
  Audio();
  ~Audio();
  int listDevices();
  bool isInputFormatSupported(int device, int numChannels, PaSampleFormat format, double samplerate);
  bool openInputStream(int device, int numChannels, PaSampleFormat format, double samplerate, unsigned long framesPerBuffer);
  bool startInputStream();
  bool stopInputStream();
  void setInputListener(cb_audio_in inCB, void* inData); //AudioListener* listener);
  static int inputStreamCallback(
    const void* input
    ,void* output
    ,unsigned long numFrames
    ,const PaStreamCallbackTimeInfo* time
    ,PaStreamCallbackFlags status
    ,void* userData
  );
 private:
  std::string getSampleFormatText(PaSampleFormat f);
  PaStream* input_stream;
  cb_audio_in in_cb; /* gets called when we get audio, make sure to not do heavy stuff in your callback! */
  void* in_user; /* gets passed to your in callback */
  
  //AudioListener* input_listener;
  
};
#endif
