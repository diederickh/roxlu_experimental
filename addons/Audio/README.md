# Audio

- Audio input (microphone)
- Audio output (player)
- Audio conversion to MP3 

## Basic example for audio input

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