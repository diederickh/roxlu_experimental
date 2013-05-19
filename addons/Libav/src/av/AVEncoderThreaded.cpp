#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <av/AVEncoderThreaded.h>

void avencoder_thread(void* user) {

  AVEncoderThreaded& enc = *(static_cast<AVEncoderThreaded*>(user));
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
      // RX_VERBOSE("ADDING: pixels: %p, AVEncoderFrame: %p", f->pixels, f);
      //enc.enc.addVideoFrame(f->pixels, f->pts, f->nbytes);
      //uv_mutex_lock(&mutex);
      //f->is_free = true;
      //uv_mutex_unlock(&mutex);
    }

    if(must_stop) {
      break;
    }
  }

  enc.enc.stop();
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
  RX_VERBOSE("--------------------------------");
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

  // preallocate the frames
  int size = avpicture_get_size(cfg.in_pixel_format, cfg.in_w, cfg.in_h);
  if(!size) {
    RX_ERROR(ERR_AVT_WRONG_SIZE);
    return false;
  }
  
  for(int i = 0; i < numFramesToAllocate; ++i) {
    AVEncoderFrame* f = new AVEncoderFrame();
    f->pixels = new unsigned char[size];
    RX_VERBOSE("PIXELS: %p", f->pixels);
    frames.push_back(f);
  }

  millis_per_frame = (cfg.time_base_num / cfg.time_base_den) * 1000;
  if(!millis_per_frame) {
    RX_ERROR(ERR_AVT_WRONG_MILLIS_PER_FRAME);
    return false;
  }

  is_setup = true;
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

  //uv_thread_create(&thread, avencoder_thread, this);

  return true;

}

bool AVEncoderThreaded::addVideoFrame(unsigned char* data, size_t nbytes) {
  static int added = 0;
  if(!time_started) {
    RX_ERROR(ERR_AVT_NOT_STARTED);
    return false;
  }

  uint64_t now = millis();
  if(now < new_frame_timeout) {
    return false;
  }
  new_frame_timeout = now + millis_per_frame;

  AVEncoderFrame* f = getFreeFrame();


  if(f) {
    int64_t pts = (millis() - time_started) / millis_per_frame;
    
    uv_mutex_lock(&mutex);
    {
      f->is_free = false;
      f->pts = pts;
      f->nbytes = nbytes;
      RX_VERBOSE("COPYING INTO, pixels: %p, frame: %p, ADDED: %d", f->pixels, f, added);
      ++added;
      
      memcpy(f->pixels, data, nbytes);
    }
    uv_mutex_unlock(&mutex);

  }
  else {
    RX_ERROR(ERR_AVT_NO_FREE_FRAME);
  }
  RX_VERBOSE("++++++++++++++++++++++++");
  return true;
}

AVEncoderFrame* AVEncoderThreaded::getFreeFrame() {
  AVEncoderFrame* f = NULL;
  static int found_frames =  0;

  uv_mutex_lock(&mutex);
  {
    for(std::vector<AVEncoderFrame*>::iterator it = frames.begin(); it != frames.end(); ++it) {
      AVEncoderFrame* frame = *it;
      if(frame->is_free) {
        f = frame;
        found_frames++;
        RX_VERBOSE("----------------->: %p", f);
        break;
      }
    }
  }
  uv_mutex_unlock(&mutex);
  //f = NULL;
  RX_VERBOSE("--- GOT FREE FRAME: %p, found: %d", f, found_frames);  
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
