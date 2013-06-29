/*

  AUDIO OUTPUT
  ------------

  Experimental code which takes care of audio output. When you want to do 
  audio output, it works a little bit like webcam input. You have a specific
  device that you select and want to use for output. When selecting this device
  you need to specific the output settings, like number of channels, format (float/int)
  samplerate (44100, ..) etc..  In theory you should be able to select multiple 
  output streams, but it turns out that PortAudio goes nana and we got wierd 
  behavior. Therefore we advise to use just one object + one output stream.
  
 */ 
#ifndef ROXLU_AUDIO_OUTPUT_H
#define ROXLU_AUDIO_OUTPUT_H

#include <assert.h>
#include <map>
#include <string>
#include <portaudio.h>
#include <audio/Audio.h>
#include <audio/AudioStream.h>
#include <uv.h>

#define AO_STATE_NONE 0
#define AO_STATE_OPENED 1
#define AO_STATE_STARTED 2

#define AS_STATE_NONE 0                  /* SoundInfo state: default */
#define AS_STATE_PLAYING 1               /* SoundInfo state: playing ; we're reading frames in the audio output callback */


// ------------------------------------------------------------------

int audio_output_callback(const void* input, void* output, unsigned long numFrames,
                          const PaStreamCallbackTimeInfo* time,
                          PaStreamCallbackFlags status, void* user);


void audio_output_stream_end_callback(void* user); 


// ------------------------------------------------------------------

struct SoundInfo {                                    /* tiny struct which is used when playing multiple audio streams */
  SoundInfo(); 
  ~SoundInfo();  
  AudioStream* stream;                                /* the stream it is playing (multiple streams can be played back at the same time, using the same resource! */
  int state;                                          /* the state */
  size_t read_frames;                                 /* the number of read frames */
};

// ------------------------------------------------------------------

class AudioOutput {
 public:
  AudioOutput();
  ~AudioOutput();
  bool openOutputStream(int device, int numChannels, PaSampleFormat fmt, double samplerate, unsigned long framesPerBuffer);
  bool closeOutputStream();                           /* use: open/start/stop/close, or open/cose */
  bool startOutputStream();                           /* start the output stream, must be called if  you want to start listening */
  bool stopOutputStream();                            /* stop the output stream */
  
  bool addSound(int id, AudioStream* io);             /* add a new audio stream, the caller is responsible for removing it from this object (use: `removeSound()`) and freeing any related memory */
  bool removeSound(int id);                           /* remove the sound for the given id; caller is responsible for freeing memory */
  bool playSound(int id);                             /* play the given audio id */
  bool stopSound(int id);                             /* stop playing the given audio id */
  AudioStream* getSound(int id);                      /* return the sound stream for the given id, returns NULL when not found */
  bool removeSoundInfo(SoundInfo* si);
  void shutdown();                                    /* shutdown this object; free all allocated mem and go back to the state when we were allocated */
 public:
  int num_channels;                                   /* number of channels in the opened audio stream */
  PaSampleFormat format;                              /* the format of the opened audio stream */
  double samplerate;                                  /* samplerate (44100, etc..) of the opened audio stream */
  unsigned long frames_per_buffer;                    /* the number of frames per buffer we should fetch every time the callback gest called */
  int state;                                          /* state of this object, used to make sure correct functions are called at the correct time */
  PaStream* output_stream;                            /* the output stream */
  std::map<int, AudioStream*> sounds;                 /* streams are used to read samples; these can be file or memory streams */
  std::vector<SoundInfo*> sounds_info;
  uv_mutex_t mutex;          
};

#endif
