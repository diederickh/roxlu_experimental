#ifndef ROXLU_LIBAV_AVPLAYER_H
#define ROXLU_LIBAV_AVPLAYER_H

#include <string>
#include <deque>
#include <av/AVDecoder.h>
#include <av/Types.h>
#include <videocapture/VideoCaptureGLSurface.h>


extern "C" {
#  include <uv.h>  
}

#define AVP_STATE_NONE 0
#define AVP_STATE_PLAY 1
#define AVP_STATE_PAUSE 2


#define ERR_AVP_ALREADY_SWS "We already have an SWS context; not creating another one."
#define ERR_AVP_CREATE_SWS "An error occured when creating the SWS context "
#define ERR_AVP_ALREADY_PLAYING "Cannot start playing because we're already playing. Maybe call stop() first?"
#define ERR_AVP_THREAD_UNHANDLED_TYPE "The decoder thread received a AVDecoderFrame for which we don't handle the type yet."
#define ERR_AVP_SWSCALE_FAILED "The sws_scale call failed"
#define ERR_AVP_PREALLOC_PIC "Cannot preallocate the frame. (we'll cleanup all other preallocated frames)"
#define ERR_AVP_ALLOC_SWS "Cannot initialize sws decoder."
#define ERR_AVP_PREALLOC_NO_SWS "Trying to preallocate the AVPlayerFrames and we need an SWS context to convert between pixel formats, but the SWScontext isn't created yet.. or an error occured when creating the sws context. "
//#define V_AVP_THREAD_UNHANDLED_VIDEO_FRAME "The decoder thread got a AVDecoderFrame which has a AVFrame with wrong width/height/data; this can be some arbitrary frame that we don't need to handle "


void avplayer_thread(void *user);

struct AVPlayerFrame {                                        /* we decode in a separate thread, and when necessary we will convert the default pixel format of the video to the format specified in the AVPlayerSettings, this frame is used to store the data */
  AVPlayerFrame();
  ~AVPlayerFrame();

  AVPicture pic;                                              /* is used when we need to convert between pixel formats; this member is only used for video frames. the AVPlayerFrame.data will be set to pic.data when we convert image data; else it will be set to the AVDecoderFrame.frame.data */
  AVDecoderFrame* decoder_frame;                              /* reference to the decoder frame. after using the data of this frame, you must free this memory! */
  unsigned char* data;                                        /* the data of this frame. can be audio data, video data, etc.. */
  bool is_free;                                               /* we preallocate and reuse memory so we don't need to allocate new memory for each frame */
  size_t nbytes;                                              /* the number fo bytes in data */
};

class AVPlayer {
 public:
  AVPlayer();
  ~AVPlayer();

  bool open(std::string filename, bool datapath = false, AVPlayerSettings cfg = AVPlayerSettings(), int numPreAllocateFrames = 10);
  bool play();
  bool stop();
  void update();
  void draw(int x, int y, int w = 0, int h = 0);

  void lock();
  void unlock();

  int getWidth();
  int getHeight();
  AVPixelFormat getPixelFormat();                                /* returns the pixel format that is used by the decoder */
  uint64_t millis();                                             /* just a helper which returns the current time in millis */

 public:                                                         /* these functions should actually be private, but the thread needs them */
  bool needsToConvertPixelFormat();                              /* returns true if we need to convert from the pixel format that the decoder uses by default and the one we have set in the settings.out_pixel_format */
  bool initializeSWS();                                          /* creates a SwsContext if necessary */
  bool getFreeFrames(std::vector<AVPlayerFrame*>& result);       /* will copy all free AVPlayerFrames into the given vector. returns true when there are free frames */                            
  bool allocateFrames();                                         /* this will allocate the AVPlayerFrames that we use when decodding. */
  void deleteFrames();                                           /* this function will deallocate the allocated AVPlayerFrames */
  void freeFrames();                                             /* this will make all frames `free` again so the can be reused */

 public:
  AVPlayerSettings settings;

  /* playback */
  int state;
  VideoCaptureGLSurface gl_surface;
  uint64_t time_started;
  bool must_stop;

  /* decoding */
  AVDecoder dec;
  std::vector<AVPlayerFrame*> frames;                           /* we pre-allocate some frames so we have a small buffer that will hold decoded frames. we decode in a separate thread */
  std::vector<AVPlayerFrame*> decoded_frames;                   /* when a frame is decoded (and maybe the pixel format converted) it's copied to this member that we used to draw the video to the screen */
  int num_frames_to_allocate;                    
  int nbytes_video_frame;                                               /* number of bytes in the decoded video frames */
  SwsContext* sws;
 
  /* thread */
  uv_mutex_t mutex;
  uv_thread_t thread;
};

inline void AVPlayer::lock() {
  uv_mutex_lock(&mutex);
}
inline void AVPlayer::unlock() {
  uv_mutex_unlock(&mutex);
}

inline int AVPlayer::getWidth() {
  return dec.getWidth();
}

inline int AVPlayer::getHeight() {
  return dec.getHeight();
}

inline AVPixelFormat AVPlayer::getPixelFormat() {
  return dec.getPixelFormat();
}

inline uint64_t AVPlayer::millis() {
  return (uv_hrtime() / 1000000);
}
#endif
