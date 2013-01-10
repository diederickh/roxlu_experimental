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

*/


// The callback for audio input. If you're asking for multiple channel input, the 
// input will probably be interleaved, so [chan0][chan1][chan0][chan1]etc..
typedef void(*cb_audio_in)(const void* input, unsigned long nframes, void* user);

class Audio {
 public:
  Audio();
  ~Audio();
  int listDevices();
  int getDefaultInputDevice();
  bool isInputFormatSupported(int device, int numChannels, PaSampleFormat format, double samplerate);
  bool openInputStream(int device, int numChannels, PaSampleFormat format, double samplerate, unsigned long framesPerBuffer);
  bool startInputStream();
  bool stopInputStream();
  void setInputListener(cb_audio_in inCB, void* inData); 
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
};
#endif
