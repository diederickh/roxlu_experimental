#include <audio/AudioPlayer.h>

AudioPlayer::AudioPlayer()
  :total_read_frames(0)
  ,samplerate(0)
  ,state(AUDIO_PLAYER_STATE_NONE)
  ,cb_player(NULL)
  ,cb_user(NULL)
{
}

AudioPlayer::~AudioPlayer() {
  cb_player = NULL;
  cb_user = NULL;
  total_read_frames = 0;
  samplerate = 0;
}

bool AudioPlayer::load(std::string file, bool datapath, audio_player_callback cbPlayer, void* cbUser) {
  if(!audio_file.load(file, datapath)) {
    return false;
  }

  cb_player = cbPlayer;
  cb_user = cbUser;
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
  if(state == AUDIO_PLAYER_STATE_PLAYING) {
    RX_ERROR("Cannot start playing because we're already started");
    return false;
  }
 
  if(!audio.startOutputStream()) {
    RX_ERROR("Cannot start the audio output stream");
    return false;
  }
 
  state = AUDIO_PLAYER_STATE_PLAYING;
 
  if(cb_player) {
    cb_player(this, AUDIO_PLAYER_EVENT_STARTED, cb_user);
  }
  RX_VERBOSE("PLAYER STATE (IN PLAY()): %d", state);
  return true;
}

bool AudioPlayer::stop() {
  RX_VERBOSE("PLAYER STATE (IN STOP()): %d", state);
  if(state != AUDIO_PLAYER_STATE_PLAYING) {
    RX_ERROR("Cannot stop playing because we didn't start yet");
    return false;
  }
 
  total_read_frames = 0;
  audio_file.close();
  audio.closeOutputStream();
  RX_VERBOSE("----------------------------------------");
  /*
  if(!audio.stopOutputStream()) {
    RX_ERROR("Cannot stop the output stream (but closed the audio file)");
    return false;
  }
  */

  state = AUDIO_PLAYER_STATE_NONE;
 
  if(cb_player) {
    cb_player(this, AUDIO_PLAYER_EVENT_STOPPED, cb_user);
  }
 
  return true;

}

void audio_player_audio_out_callback(void* output, unsigned long nframes, void* user) {
  AudioPlayer* ap = static_cast<AudioPlayer*>(user);
  ap->total_read_frames += nframes;
  int nread = ap->audio_file.readFrames(output, nframes);
 
  if(nread == 0) {
    ap->stop();
  }
}


