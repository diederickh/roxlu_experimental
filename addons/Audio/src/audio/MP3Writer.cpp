#include <audio/MP3Writer.h>
#include <roxlu/core/Utils.h>


MP3WriterConfig::MP3WriterConfig() {
  clear();
}

MP3WriterConfig::~MP3WriterConfig() {
  clear();
}

void MP3WriterConfig::clear() {
  cb_open = NULL;
  cb_data = NULL;
  cb_close = NULL;
  user = NULL;
  bitrate = 0;
  num_channels = 0;
  quality = 0;
  mode = MP3_WR_MODE_NOT_SET;
  samplerate = MP3_WR_SAMPLERATE_NOT_SET;
  id3_title.clear();
  id3_artist.clear();
  id3_album.clear();
  id3_year.clear();
  id3_comment.clear();
  id3_track.clear();
}

bool MP3WriterConfig::validate() {
  if(mode == MP3_WR_MODE_NOT_SET) {
    RX_ERROR(MP3_WRERR_MODE_NOT_SET);
    return false;
  }

  if(num_channels < 0 || num_channels > 2) {
    RX_ERROR(MP3_WRERR_WRONG_NCHANNELS);
    return false;
  }

  if(samplerate == MP3_WR_SAMPLERATE_NOT_SET) {
    RX_ERROR(MP3_WRERR_SAMPLERATE_NOT_SET);
    return false;
  }

  if(bitrate == 0) {
    RX_ERROR(MP3_WRERR_NO_BITRATE);
    return false;
  }

  if(cb_open == NULL) {
    RX_ERROR(MP3_WRERR_NO_OPEN_CB);
    return false;
  }

  if(cb_data == NULL) {
    RX_ERROR(MP3_WRERR_NO_DATA_CB);
    return false;
  }

  if(cb_close == NULL) {
    RX_ERROR(MP3_WRERR_NO_CLOSE_CB);
    return false;
  }

  if(quality == 0) {
    quality = 5;
  }

  return true;
}

bool MP3WriterConfig::hasID3() {
  return id3_title.size() || id3_artist.size() || id3_album.size()
    || id3_year.size() || id3_comment.size() || id3_track.size();
}

int MP3WriterConfig::samplerateToInt() {
  if(samplerate == MP3_WR_SAMPLERATE_44100) {
    return 44100;
  }
  else if(samplerate == MP3_WR_SAMPLERATE_16000) {
    return 16000;
  }

  return 0;
}

// -----------------------------------------

void mp3_writer_default_open(void* user) {
  MP3FileWriter* f = static_cast<MP3FileWriter*>(user);
  if(!f->ofs.is_open()) {
    return;
  }
}

void mp3_writer_default_close(void* user) {
  MP3FileWriter* f = static_cast<MP3FileWriter*>(user);
  if(!f->ofs.is_open()) {
    return;
  }

  f->ofs.close();
}

void mp3_writer_default_data(const char* data, size_t nbytes, void* user) {
  MP3FileWriter* f = static_cast<MP3FileWriter*>(user);
  if(!f->ofs.is_open()) {
    return;
  }

  f->ofs.write((char*)data, nbytes);
}

MP3FileWriter::MP3FileWriter(std::string filename, bool datapath) 
  :cb_open(NULL)
  ,cb_data(NULL)
  ,cb_close(NULL)
{
  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  ofs.open(filename.c_str(), std::ios::binary);

  if(!ofs.is_open()) {
    RX_ERROR(MP3_WRERR_FILE_OPEN, filename.c_str());
  }
  else {
    cb_open = mp3_writer_default_open;
    cb_close = mp3_writer_default_close;
    cb_data = mp3_writer_default_data;
  }
}

MP3FileWriter::~MP3FileWriter() {
  cb_open = NULL;
  cb_close = NULL;
  cb_data = NULL;
}

// -----------------------------------------

MP3Writer::MP3Writer() 
  :is_started(false)
  ,is_setup(false)
  ,lame_flags(NULL)
{

}

MP3Writer::~MP3Writer() {
  is_started = false;
  is_setup = false;

  if(lame_flags) {
    end();
  }
  
}

bool MP3Writer::setup(MP3WriterConfig cfg) {
  if(!cfg.validate()) {
    return false;
  }

  if(is_setup) {
    return false;
  }

  if(is_started) {
    return false;
  }

  config = cfg;
  is_setup = true;
  return true;
}

bool MP3Writer::begin() {
  if(!is_setup) {
    RX_ERROR(MP3_WRERR_NOT_SETUP);
    return false;
  }

  if(is_started) {
    return false;
  }

  int samplerate = 0;
  MPEG_mode mode = MONO;
  
  if(config.mode == MP3_WR_MODE_STEREO) {
    mode = STEREO;
  }
  else {
    mode = MONO;
  }

  samplerate = config.samplerateToInt();

  lame_flags = lame_init();
  if(!lame_flags) {
    RX_ERROR(MP3_WRERR_LAME_INIT);
    return false;
  }

  lame_set_num_channels(lame_flags, config.num_channels);
  lame_set_in_samplerate(lame_flags, samplerate);
  lame_set_brate(lame_flags, config.bitrate);
  lame_set_mode(lame_flags, mode);
  lame_set_quality(lame_flags, config.quality); 

  if(config.hasID3()) {
    id3tag_init(lame_flags);
  }

  if(config.id3_title.size()) {
    id3tag_set_title(lame_flags, config.id3_title.c_str());
  }

  if(config.id3_artist.size()) {
    id3tag_set_artist(lame_flags, config.id3_artist.c_str());
  }
  
  if(config.id3_album.size()) {
    id3tag_set_album(lame_flags, config.id3_album.c_str());
  }

  if(config.id3_year.size()) {
    id3tag_set_year(lame_flags, config.id3_year.c_str());
  }

  if(config.id3_comment.size()) {
    id3tag_set_comment(lame_flags, config.id3_comment.c_str());
  }

  if(config.id3_track.size()) {
    int r = id3tag_set_track(lame_flags, config.id3_track.c_str());
    if(r == -1) {
      RX_ERROR(MP3_WRERR_ID3_TRACK_OOR);
    }
    else if(r == -2) {
      RX_ERROR(MP3_WRERR_ID3_TRACK_INVALID);
    }
  }

  if(lame_init_params(lame_flags) < 0) {
    RX_ERROR(MP3_WRERR_LAME_PARAMS);
    return false;
  }

  is_started = true;

  config.cb_open(config.user);
  return true;
}

bool MP3Writer::end() {
  if(!is_setup) {
    RX_ERROR(MP3_WRERR_NOT_SETUP);
    return false;
  }

  if(!is_started) {
    RX_ERROR(MP3_WRERR_NOT_STARTED);
    return false;
  }
  
  int written = lame_encode_flush(lame_flags, (unsigned char*)mp3_buffer, MP3_WRITER_BUFFER_SIZE);
  if(written < 0) {
    RX_ERROR(MP3_WRERR_CANNOT_FLUSH);
  }
  else if(config.cb_data) {
    config.cb_data((const char*)mp3_buffer, written, config.user);
  }

  lame_close(lame_flags);
  
  config.cb_close(config.user);
  
  lame_flags = NULL;
  is_started = false;

  return true;
}


void MP3Writer::addAudioInterleaved(const short int* data, size_t nbytes, int nsamples) {

  if(!is_setup) {
    return;
  }

  if(!is_started) {
    return;
  }

  int written = lame_encode_buffer_interleaved(lame_flags, (short int*)data, nsamples, mp3_buffer, MP3_WRITER_BUFFER_SIZE);
  if(written > 0 && config.cb_data) {
    config.cb_data((const char*)mp3_buffer, written, config.user);
  }

}

void MP3Writer::addAudioMono(const short int* data, size_t nbytes, int nsamples) {

 if(!is_setup) {
    return;
  }

  if(!is_started) {
    return;
  }

  int written = lame_encode_buffer(lame_flags, data, NULL, nsamples, mp3_buffer, MP3_WRITER_BUFFER_SIZE);
  if(written > 0 && config.cb_data) {
    config.cb_data((const char*)mp3_buffer, written, config.user);
  }
 
}
