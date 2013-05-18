#ifndef ROXLU_IVF_WRITER_THREADED_H
#define ROXLU_IVF_WRITER_THREADED_H

extern "C" {
#  include <uv.h>  
}

#include <string>
#include <vector>
#include <webm/IVFWriter.h>

#define IVF_ERR_TWNO_FREE_FRAMES "No free frames that we can use to write raw images to... the encoding thread can't keep up. Try to increase num_frames in setup"

void ivf_writer_thread(void* user);

struct IVFData {                                              /* will delete the pixels it holds in the d'tor */
  IVFData();
  ~IVFData();

  unsigned char* pixels;
  size_t nbytes;
  int pts;
  bool is_free;
};

struct IVFDataSorter {
  bool operator()(IVFData* a, IVFData* b) {
    return a->pts < b->pts;
  }
};

class IVFWriterThreaded {
 public:
  IVFWriterThreaded();
  ~IVFWriterThreaded();
  bool setup(int inW, int inH,                              /* input size */
             int outW, int outH,                            /* output size; we will automatically scale if you want */
             int fps, int numFramesToAllocate = 250);       /* numFramesToAllocate is the number of raw frames we can hold in a buffer, the thread reads from these frames and passes the bytes to the encoder */
  bool start(std::string filename, bool datapath);          /* start encoding */
  bool stop();                                              /* stop encoding */
  void addFrame(unsigned char* data, size_t nbytes);        /* add a new frame to the encoder thread */
 public:
  IVFData* getFreeFrame();
 public:
  std::vector<IVFData*> frames;
  uint64_t time_started;
  uint64_t new_frame_timeout;
  uint64_t millis_per_frame;
  IVFWriter writer;

  bool must_stop;
  uv_mutex_t mutex;
  uv_thread_t thread;

};


#endif
