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
    std::sort(work_data.begin(), work_data.end(), AVEncoderFrameSorter());
    for(std::vector<AVEncoderFrame*>::iterator it = work_data.begin(); it != work_data.end(); ++it) {

      AVEncoderFrame* f = *it;
      enc.enc.addVideoFrame(f->pixels, f->pts, f->nbytes);

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
  :pixels(NULL)
  ,pts(0)
  ,is_free(true)
  ,nbytes(0)
{
}

AVEncoderFrame::~AVEncoderFrame() {

  if(pixels) {
    delete[] pixels;
    pixels = NULL;
  }

  is_free = true;
  nbytes = 0;
  pts = 0;
}


// -------------------------------------------

AVEncoderThreaded::AVEncoderThreaded()
  :is_setup(false)
  ,thread(NULL)
  ,must_stop(true)
  ,num_frames_to_allocate(0)
  ,millis_per_frame(0)
  ,new_frame_timeout(0)
  ,time_started(0)
{
  uv_mutex_init(&mutex);
}

AVEncoderThreaded::~AVEncoderThreaded() {

  if(time_started) {
    stop();
  }

  uv_thread_join(&thread);

  time_started = 0;
  new_frame_timeout = 0;
  millis_per_frame = 0;

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

  millis_per_frame = (cfg.time_base_num / cfg.time_base_den) * 1000;
  if(!millis_per_frame) {
    RX_ERROR(ERR_AVT_WRONG_MILLIS_PER_FRAME);
    return false;
  }

  num_frames_to_allocate = numFramesToAllocate;
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

  for(int i = 0; i < num_frames_to_allocate; ++i) {
    AVEncoderFrame* f = new AVEncoderFrame();
    f->pixels = new unsigned char[nbytes_per_image];
    f->nbytes = nbytes_per_image;
    f->is_free = true;
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

  new_frame_timeout = millis() + millis_per_frame;

  uv_thread_create(&thread, avencoder_thread, this);

  return true;

}

bool AVEncoderThreaded::addVideoFrame(unsigned char* data, size_t nbytes) {

  if(!time_started) {
    RX_ERROR(ERR_AVT_NOT_STARTED);
    return false;
  }

#if 1
  uint64_t now = millis();
  if(now < new_frame_timeout) {
    return false;
  }
  new_frame_timeout = now + millis_per_frame;
#endif

  AVEncoderFrame* f = getFreeFrame();

  if(f) {

    int64_t pts = (millis() - time_started) / millis_per_frame;

    memcpy(f->pixels, data, nbytes);

    uv_mutex_lock(&mutex);
    {
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

AVEncoderFrame* AVEncoderThreaded::getFreeFrame() {
  AVEncoderFrame* f = NULL;

  uv_mutex_lock(&mutex);
  {
    for(std::vector<AVEncoderFrame*>::iterator it = frames.begin(); it != frames.end(); ++it) {
      AVEncoderFrame* frame = *it;
      if(frame->is_free) {
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

