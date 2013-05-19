#ifndef ROXLU_LIBAV_AVENCODER_THREADED_H
#define ROXLU_LIBAV_AVENCODER_THREADED_H

extern "C" {
#  include <uv.h>
}

#include <string>
#include <vector>
#include <av/AVTypes.h>
#include <av/AVEncoder.h>

#define ERR_AVT_NOT_SETUP "Not setup so we cannot start!"
#define ERR_AVT_NOT_STARTED "Not started yet, so you cannot add frames"
#define ERR_AVT_WRONG_SIZE "Cannot calculate the size for the input pixel format"
#define ERR_AVT_WRONG_MILLIS_PER_FRAME "Wrong millis per frame .. did you set the time_base_num and time_base_den faulty?"
#define ERR_AVT_NO_FREE_FRAME "No free frame found. Allocate more frames (see setup()), or change the frame rate (see AVEncoderSettings time_base_*) "

void avencoder_thread(void* user);

struct AVEncoderFrame {                /* we create a somewhat similar object like a ringbuffer with this type. The AVEncoderFrame will take ownership of the pixels */
  AVEncoderFrame();
  ~AVEncoderFrame();

  unsigned char* pixels;
  int64_t pts;
  bool is_free;
  size_t nbytes;
};

struct AVEncoderFrameSorter {
  bool operator()(AVEncoderFrame* a, AVEncoderFrame* b) {
    return a->pts < b->pts;
  }
};

class AVEncoderThreaded {
 public:
  AVEncoderThreaded();
  ~AVEncoderThreaded();
  bool setup(AVEncoderSettings cfg, int numFramesToAllocate = 100);
  bool start(std::string filename, bool datapath = false);
  bool stop();
  bool addVideoFrame(unsigned char* data, size_t nbytes);
  uint64_t millis();                                         /* tiny helper to retrieve time in millis */
 private:
  AVEncoderFrame* getFreeFrame();

public:                                                     /* all members are actually private but we need to acces them in the thread function */
  bool is_setup;
  std::vector<AVEncoderFrame*> frames;

  /* timing when encoding */
  uint64_t time_started;
  uint64_t new_frame_timeout;
  uint64_t millis_per_frame;

  /* encoder */
  AVEncoder enc;
  
  /* threading */
  bool must_stop;
  uv_mutex_t mutex;
  uv_thread_t thread;
  
  
};

#endif

