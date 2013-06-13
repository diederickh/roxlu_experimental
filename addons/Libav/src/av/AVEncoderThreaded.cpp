#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <av/AVEncoderThreaded.h>

void avencoder_thread(void* user) {

  AVEncoderThreaded& enc = *(static_cast<AVEncoderThreaded*>(user));
  
  if(!enc.initialize()) {
    return;
  }

  uv_mutex_t& mutex = enc.mutex;
  std::vector<AVEncoderFrame*> work_data;
  bool must_stop = false;
  
  while(true) {
    // get frames that need to be encoded
    work_data.clear();
    uv_mutex_lock(&mutex);
    {
      for(std::vector<AVEncoderFrame*>::iterator it = enc.frames.begin(); it != enc.frames.end(); ++it) {
        AVEncoderFrame* f = *it;
        if(!f->is_free) {
          work_data.push_back(f);
        }
      }
    }
    must_stop = enc.must_stop;
    uv_mutex_unlock(&mutex);

    // encode worker data
    //std::sort(work_data.begin(), work_data.end(), AVEncoderFrameSorter()); // @todo seems to work fine w/o
    for(std::vector<AVEncoderFrame*>::iterator it = work_data.begin(); it != work_data.end(); ++it) {
      AVEncoderFrame* f = *it;
      if(f->type == AV_TYPE_VIDEO) {
        enc.enc.addVideoFrame(f->data, f->pts, f->nbytes);
        enc.enc.update();
      }
      else if(f->type == AV_TYPE_AUDIO) {
        enc.enc.addAudioFrame(f->data, 1152, f->pts);
        enc.enc.update();
      }

      uv_mutex_lock(&mutex);
      f->is_free = true;
      uv_mutex_unlock(&mutex);
    }

    if(must_stop) {
      break;
    }
  }

  enc.shutdown();
}

// -------------------------------------------

AVEncoderFrame::AVEncoderFrame() 
  :data(NULL)
  ,pts(0)
  ,is_free(true)
  ,nbytes(0)
  ,type(AV_TYPE_NONE)
{
}

AVEncoderFrame::~AVEncoderFrame() {

  if(data) {
    delete[] data;
    data = NULL;
  }

  is_free = true;
  nbytes = 0;
  pts = 0;
  type = AV_TYPE_NONE;
}


// -------------------------------------------

AVEncoderThreaded::AVEncoderThreaded()
  :is_setup(false)
  ,must_stop(true)
  ,num_video_frames_to_allocate(0)
  ,millis_per_video_frame(0)
  ,new_video_frame_timeout(0)
  ,time_started(0)
  ,num_added_audio_samples(0)
{
  uv_mutex_init(&mutex);
}

AVEncoderThreaded::~AVEncoderThreaded() {

  if(time_started) {
    stop();
  }

  uv_thread_join(&thread);

  time_started = 0;
  new_video_frame_timeout = 0;
  millis_per_video_frame = 0;
  num_added_audio_samples = 0;

  for(std::vector<AVEncoderFrame*>::iterator it = frames.begin(); it != frames.end(); ++it) {
    AVEncoderFrame* f = *it;
    delete f;
    f = NULL;
  }
  frames.clear();
  
  uv_mutex_destroy(&mutex);
}

bool AVEncoderThreaded::setup(AVEncoderSettings cfg, int numFramesToAllocate) {

  if(!enc.setup(cfg)) {
    return false;
  }

  // setup video
  millis_per_video_frame = (cfg.time_base_num / cfg.time_base_den) * 1000;
  if(!millis_per_video_frame) {
    RX_ERROR(ERR_AVT_WRONG_MILLIS_PER_FRAME);
    return false;
  }

  num_video_frames_to_allocate = numFramesToAllocate;
  settings = cfg;
  is_setup = true;
  return true;
}

bool AVEncoderThreaded::initialize() {

  if(frames.size()) {
    RX_ERROR(ERR_AVT_INITIALIZE_FRAMES);
    return false;
  }

  // preallocate the frames
  int nbytes_per_image = avpicture_get_size(settings.in_pixel_format, settings.in_w, settings.in_h);
  if(!nbytes_per_image) {
    RX_ERROR(ERR_AVT_WRONG_SIZE);
    return false;
  }

  // preallocate video frames 
  for(int i = 0; i < num_video_frames_to_allocate; ++i) {
    AVEncoderFrame* f = new AVEncoderFrame();
    f->data = new unsigned char[nbytes_per_image + 4]; // @todo - we add 4 bytes because swscale seems to access (or writes into) a location which isn't allowed. My guess is that is uses SSE that is 4 bytes aligned, see this log with a backtracke: https://gist.github.com/roxlu/ffcc2305dd96d21f0a4f */
    f->nbytes = nbytes_per_image;
    f->is_free = true;
    f->type = AV_TYPE_VIDEO;
    frames.push_back(f);
  }

  // preallocate audio frames
  int linesize = 0;
  nbytes_per_audio_buffer = av_samples_get_buffer_size(&linesize, 
                                                              settings.num_channels,
                                                              enc.getAudioInputFrameSizePerChannel(settings) * settings.num_channels,
                                                              settings.sample_fmt,
                                                              0); // @todo what do we need for this last parameter to av_samples_get_buffer_size
  // @todo also pass an option to allocate X-audio frames
  for(int i = 0; i < num_video_frames_to_allocate; ++i) {
    AVEncoderFrame* f = new AVEncoderFrame();
    f->data = new unsigned char[nbytes_per_audio_buffer];
    f->nbytes = nbytes_per_audio_buffer;
    f->is_free = true;
    f->type = AV_TYPE_AUDIO;
    frames.push_back(f);
  }

  return true;
}

bool AVEncoderThreaded::shutdown() {

  for(std::vector<AVEncoderFrame*>::iterator it = frames.begin(); it != frames.end(); ++it) {
    delete *it;
  }

  frames.clear();
  enc.stop();
  return true;
}

bool AVEncoderThreaded::start(std::string filename, bool datapath) {

  if(!is_setup) {
    RX_ERROR(ERR_AVT_NOT_SETUP);
    return false;
  }

  uv_mutex_lock(&mutex);
  must_stop = false;
  uv_mutex_unlock(&mutex);
  
  enc.start(filename, datapath);

  time_started = millis();
  num_added_audio_samples = 0;
  new_video_frame_timeout = millis() + millis_per_video_frame;

  uv_thread_create(&thread, avencoder_thread, this);

  return true;

}

bool AVEncoderThreaded::addAudioFrame(uint8_t* data, size_t nsamples) {
  if(!time_started) {
    RX_ERROR(ERR_AVT_NOT_STARTED);
    return false;
  }

  AVEncoderFrame* f = getFreeAudioFrame();
  if(!f) {
    RX_ERROR(ERR_AVT_NO_FREE_FRAME);
    return false;
  }

  uint64_t pts = num_added_audio_samples;
  num_added_audio_samples += nsamples;

  uv_mutex_lock(&mutex);
  {
    memcpy(f->data, data, nbytes_per_audio_buffer);
    f->pts = pts;
    f->is_free = false;
  }
  uv_mutex_unlock(&mutex);

  return true;
}

bool AVEncoderThreaded::addVideoFrame(unsigned char* data, size_t nbytes) {

  if(!time_started) {
    RX_ERROR(ERR_AVT_NOT_STARTED);
    return false;
  }

#if 0
  uint64_t now = millis();
  if(now < new_video_frame_timeout) {
    return false;
  }
  new_video_frame_timeout = now + millis_per_video_frame;
#endif

  AVEncoderFrame* f = getFreeVideoFrame();

  if(f) {

    int64_t pts = (millis() - time_started) / millis_per_video_frame;
    uv_mutex_lock(&mutex);
    {
      memcpy(f->data, data, nbytes);
      f->is_free = false;
      f->pts = pts;
    }
    uv_mutex_unlock(&mutex);
  }
  else {
    RX_ERROR(ERR_AVT_NO_FREE_FRAME);
  }

  return true;
}

AVEncoderFrame* AVEncoderThreaded::getFreeVideoFrame() {
  return getFreeFrame(AV_TYPE_VIDEO);
}

AVEncoderFrame* AVEncoderThreaded::getFreeAudioFrame() {
  return getFreeFrame(AV_TYPE_AUDIO);
}

AVEncoderFrame* AVEncoderThreaded::getFreeFrame(int type) {
  AVEncoderFrame* f = NULL;

  uv_mutex_lock(&mutex);
  {
    for(std::vector<AVEncoderFrame*>::iterator it = frames.begin(); it != frames.end(); ++it) {
      AVEncoderFrame* frame = *it;
      if(frame->type == type && frame->is_free) {
        f = frame;
        break;
      }
    }
  }
  uv_mutex_unlock(&mutex);
  return f;
}

bool AVEncoderThreaded::stop() {

  if(!is_setup) {
    RX_ERROR(ERR_AVT_NOT_SETUP);
    return false;
  }

  time_started = 0;

  uv_mutex_lock(&mutex);
  must_stop = true;
  uv_mutex_unlock(&mutex);
  
  return true;
}

uint64_t AVEncoderThreaded::millis() {
  return (uv_hrtime()/1000000);
}

