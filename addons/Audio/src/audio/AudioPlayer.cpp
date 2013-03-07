#include <audio/AudioPlayer.h>

AudioPlayer::AudioPlayer() {
}

AudioPlayer::~AudioPlayer() {
}

bool AudioPlayer::load(std::string file, bool datapath) {
  if(!audio_file.load(file, datapath)) {
    return false;
  }

  bool r = audio.openOutputStream(audio.getDefaultOutputDevice(),
                         audio_file.getNumChannels(),
                         libSndfileFormatToPortAudioFormat(audio_file.getFormat()),
                         audio_file.getSampleRate(),
                         audio_file.getNumFramesPerChunk()
                         );
  if(!r) {
    return false;
  }

  audio.setOutputCallback(audio_player_audio_out_callback, this);

  return r;
}

bool AudioPlayer::play() {
  return audio.startOutputStream();
}

void audio_player_audio_out_callback(void* output, unsigned long nframes, void* user) {
  AudioPlayer* ap = static_cast<AudioPlayer*>(user);
  ap->audio_file.readFrames(output, nframes);
}


