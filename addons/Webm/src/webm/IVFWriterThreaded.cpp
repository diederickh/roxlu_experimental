#include <webm/IVFWriterThreaded.h>
#include <roxlu/core/Utils.h>

void ivf_writer_thread(void* user) {
  IVFWriterThreaded* ivf = static_cast<IVFWriterThreaded*>(user);
  uv_mutex_t& mutex = ivf->mutex;
  std::vector<IVFData*> work_data;
  bool must_stop = false;

  while(true) {
    // get frames that need to be encoded
    work_data.clear();
    uv_mutex_lock(&mutex);
    {
      for(std::vector<IVFData*>::iterator it = ivf->frames.begin(); it != ivf->frames.end(); ++it) {
        IVFData* f = *it;
        if(!f->is_free) {
          work_data.push_back(f);
        }
      }
    }
    must_stop = ivf->must_stop;
    uv_mutex_unlock(&mutex);

    // encode worker data
    std::sort(work_data.begin(), work_data.end(), IVFDataSorter());
    for(std::vector<IVFData*>::iterator it = work_data.begin(); it != work_data.end(); ++it) {
      IVFData* f = *it;
      ivf->writer.addFrameWithPTS(f->pixels, f->pts, f->nbytes);
      f->is_free = true;
    }

    if(must_stop) {
      break;
    }

  }

  ivf->writer.stop();
}

// --------------------------------------------------
IVFData::IVFData() 
  :pixels(NULL)
  ,nbytes(0)
  ,pts(0)
  ,is_free(true)
{
}

IVFData::~IVFData() {
  if(pixels) {
    delete[] pixels;
    pixels = NULL;
  }
  nbytes = 0;
  pts = 0;
  is_free = true;
}

// --------------------------------------------------

IVFWriterThreaded::IVFWriterThreaded() 
  :time_started(0)
  ,millis_per_frame(0)
  ,must_stop(false)
  ,new_frame_timeout(0)
{
  uv_mutex_init(&mutex);
}

IVFWriterThreaded::~IVFWriterThreaded() {
  if(time_started) {
    stop();
  }
  uv_thread_join(&thread);
  uv_mutex_destroy(&mutex);

  time_started = 0;
  millis_per_frame = 0;
  new_frame_timeout = 0;
  must_stop = false;

  for(std::vector<IVFData*>::iterator it = frames.begin(); it != frames.end(); ++it) {
    IVFData* f = *it;
    delete f;
  }

  frames.clear();
}

bool IVFWriterThreaded::setup(int inW, int inH, int outW, int outH, int fps, int numFramesToAllocate) {
  // preallocate some frames
  int num_bytes = inW * inH * 3;
  for(int i = 0; i < numFramesToAllocate; ++i) {
    IVFData* frame = new IVFData();
    frame->pixels = new unsigned char[num_bytes];
    frames.push_back(frame);
  }

  millis_per_frame = (1.0f/fps) * 1000;
  return writer.setup(inW, inH, outW, outH, fps);
}

bool IVFWriterThreaded::start(std::string filename, bool datapath) {
  time_started = rx_millis();

  if(!writer.start(filename, datapath)) {
    return false;
  }

  new_frame_timeout = time_started + millis_per_frame;

  uv_thread_create(&thread, ivf_writer_thread, this);

  return true;
}

bool IVFWriterThreaded::stop() {
  time_started = 0;
  uv_mutex_lock(&mutex);
  must_stop = true;
  uv_mutex_unlock(&mutex);
  return true;
}

void IVFWriterThreaded::addFrame(unsigned char* data, size_t nbytes) {
  if(!time_started) {
    return;
  }

  // only add frames at the interval of the fps
  uint64_t now = rx_millis();
  if(now < new_frame_timeout) {
    return;
  }
  new_frame_timeout = now + millis_per_frame;

  // find a new frame
  IVFData* f = getFreeFrame();
  if(f) {
    uint64_t pts = (rx_millis() - time_started) / millis_per_frame;

    memcpy(f->pixels, data, nbytes);

    uv_mutex_lock(&mutex);
    {
      f->is_free = false;
      f->pts = pts;
      f->nbytes = nbytes;
    }
    uv_mutex_unlock(&mutex);

  }
  else {
    RX_ERROR(IVF_ERR_TWNO_FREE_FRAMES);
  }
}

IVFData* IVFWriterThreaded::getFreeFrame() {
  IVFData* found = NULL;
  uv_mutex_lock(&mutex);
  {
    for(std::vector<IVFData*>::iterator it = frames.begin(); it != frames.end(); ++it) {
      IVFData* f = *it;
      if(f->is_free) {
        found = f;
        break;
      }
    }
  }
  uv_mutex_unlock(&mutex);

  return found;
}
