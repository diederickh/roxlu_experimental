# Audio

This is a prelimenary wrapper around port audio. I'm not an experienced audio programmer
so most of this code is in the process of many changes and most of it is experimental. Though
audio input has been used for a prodction environment where we're running an input -> mp3
server for many months already. 

The output is a bit more tricky but basic playback is working fine, just make sure 
that you **Use a samplerate of 44100**.  Also call `rx_init_audio()`, before the 
Simulation object is created in main.cpp.

Before you can use this API, make sure that you initialized it directly in `main.cpp`:

````c++
main.cpp:

int main() {

  if(!rx_init_audio()) {
     RX_ERROR("Cannot setup audio");
     ::exit(EXIT_FAILURE);
  }   
};

````

**IMPORTANT** When you want to do audio output, make sure that you setup using 
floats as we're using this internally. In future release we will add the appropriate 
conversions (from int->float for example). 


````c++
bool r = !audio_output.openOutputStream(rx_get_default_audio_output_device(), 2, 
                                        paFloat32, 44100, 1024));
````

_Basic example for audio input_

````c++

// the function which will be called with audio data from microphone
void on_audio_in(const void* input, unsigned long nframes, void* user) {
     // input - audio data
     // nframes - number of frames received
     // user - pointer to data you passed to audio.setInputListener
}

Audio audio;

audio.listDevices();
audio.setInputListener(webcam_on_audio_in, this);
audio.openInputStream(1, 2, paInt16 | paNonInterleaved, 44100, 1152); // use paNonInterleaved if you want planar packed audio
audio.startInputStream();

````


_Basic example for audio output_
````c++

AudioStreamMemory audio_stream;
AudioOutput audio_output;

void setup() {  

  // load a sound file into memory
  if(!audio_stream.load("effect.wav", true)) {
    RX_ERROR("Cannot open audio stream");
    ::exit(EXIT_FAILURE);
  }

  // setup the output device. Make sure to use paFloat32!!
  bool r = audio_output.openOutputStream(rx_get_default_audio_output_device(), 2, 
                                          paFloat32, 44100, 1024));
  if(!r) {
     ::exit(EXIT_FAILURE);
  }

  // enable the output stream
  if(!audio_output.startOutputStream()) {
    ::exit(EXIT_FAILURE);
  }
  
  // add the audio to the output (1 = id of sound, used when starting the sound)
  audio_output.addSound(1, &audio_stream);
}

// play the sound on space
void onKeyDown(int key, int scancode, int mods) {
  if(key == GLFW_KEY_SPACE) {
     audio_output.playSound(1);
  }
}

````

# TODO

- The Audio.h/Audio.cpp should be used as a class for general features like listing 
  devices. Now it's basically an AudioInput class. We should create a new class
  AudioInput and move input functionality to that class. 

- Add some more format checking in the AudioOutput class and maybe samplerate 
  conversion using "Secret Rabbit Code" http://www.mega-nerd.com/SRC/api_simple.html 