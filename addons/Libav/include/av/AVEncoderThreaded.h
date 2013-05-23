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
#define ERR_AVT_INITIALIZE_FRAMES "Cannot initialize because there are already allocated frames. Did you call start twice maybe?"

void avencoder_thread(void* user);

struct AVEncoderFrame {                /* we create a somewhat similar object like a ringbuffer with this type. The AVEncoderFrame will take ownership of the pixels */
  AVEncoderFrame();
  ~AVEncoderFrame();

  unsigned char* data;
  int64_t pts;
  bool is_free;
  size_t nbytes;
  int type; 
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
  bool isStarted();
  bool addVideoFrame(unsigned char* data, size_t nbytes);
  bool addAudioFrame(uint8_t* data, size_t nsamples);
  uint64_t millis();                                         /* tiny helper to retrieve time in millis */

 public:
  bool initialize();                                         /* accessed from the thread; initializes just before the thread loop starts */
  bool shutdown();                                           /* cleans the AVEncoderThreaded::frames member when the thread stops (e.g. when stop() has been called) */

 private:
  AVEncoderFrame* getFreeVideoFrame();                       /* get a free (preallocated) video frame */
  AVEncoderFrame* getFreeAudioFrame();                       /* get a free (preallocated) audio frame) */
  AVEncoderFrame* getFreeFrame(int type);                    /* get a free frame for the given type */

 public:                                                     /* all members are actually private but we need to acces them in the thread function */
  AVEncoderSettings settings;
  bool is_setup;
  std::vector<AVEncoderFrame*> frames;

  /* audio */
  uint64_t num_added_audio_samples; /* @todo init clear */
  size_t nbytes_per_audio_buffer; /* @todo init/clear */

  /* timing when encoding */
  uint64_t time_started;
  uint64_t new_video_frame_timeout;
  int num_video_frames_to_allocate;
  uint64_t millis_per_video_frame;

  /* encoder */
  AVEncoder enc;
  
  /* threading */
  bool must_stop;
  uv_mutex_t mutex;
  uv_thread_t thread;
};

inline bool AVEncoderThreaded::isStarted() {
  return enc.isStarted(); // @todo this might be thread unsafe
}

#endif

