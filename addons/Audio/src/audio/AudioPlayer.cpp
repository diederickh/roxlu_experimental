#include <audio/AudioPlayer.h>

AudioPlayer::AudioPlayer()
  :total_read_frames(0)
  ,samplerate(0)
{
}

AudioPlayer::~AudioPlayer() {
}

bool AudioPlayer::load(std::string file, bool datapath) {
  if(!audio_file.load(file, datapath)) {
    return false;
  }

  samplerate = audio_file.getSampleRate();

  RX_VERBOSE(">>>>>>>>>>>>> %d, channels: %d", audio_file.getSampleRate(), audio_file.getNumChannels());

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

bool AudioPlayer::stop() {
  total_read_frames = 0;
  return audio.stopOutputStream();
}

void audio_player_audio_out_callback(void* output, unsigned long nframes, void* user) {
  AudioPlayer* ap = static_cast<AudioPlayer*>(user);
  ap->total_read_frames += nframes;
  ap->audio_file.readFrames(output, nframes);
}


