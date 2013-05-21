#ifndef ROXLU_LIBAV_AVPLAYER_H
#define ROXLU_LIBAV_AVPLAYER_H

#include <string>
#include <deque>
#include <av/AVDecoder.h>
#include <av/AVTypes.h>
#include <av/AVUtils.h>
#include <videocapture/VideoCaptureGLSurface.h>


extern "C" {
#  include <uv.h>  
}

#define AVP_STATE_NONE 0
#define AVP_STATE_PLAY 1
#define AVP_STATE_PAUSE 2


#define ERR_AVP_ALREADY_SWS "We already have an SWS context; not creating another one."
#define ERR_AVP_ALREADY_OPEN "We already opened the decoder"
#define ERR_AVP_ALREADY_PAUSED "We are already paused."
#define ERR_AVP_CREATE_SWS "An error occured when creating the SWS context "
#define ERR_AVP_ALREADY_PLAYING "Cannot start playing because we're already playing. Maybe call stop() first?"
#define ERR_AVP_THREAD_UNHANDLED_TYPE "The decoder thread received a AVDecoderFrame for which we don't handle the type yet."
#define ERR_AVP_SWSCALE_FAILED "The sws_scale call failed"
#define ERR_AVP_PREALLOC_PIC "Cannot preallocate the frame. (we'll cleanup all other preallocated frames)"
#define ERR_AVP_ALLOC_SWS "Cannot initialize sws decoder."
#define ERR_AVP_PREALLOC_NO_SWS "Trying to preallocate the AVPlayerFrames and we need an SWS context to convert between pixel formats, but the SWScontext isn't created yet.. or an error occured when creating the sws context. "
#define ERR_AVP_INIT_FRAMES_EXIST "Cannot initialize the AVPlayer because we have some pre-allocated frames already. These should be removed first by calling `shutdown()` "
//#define V_AVP_THREAD_UNHANDLED_VIDEO_FRAME "The decoder thread got a AVDecoderFrame which has a AVFrame with wrong width/height/data; this can be some arbitrary frame that we don't need to handle "

void avplayer_thread(void *user);

struct AVPlayerFrame {                                        /* we decode in a separate thread, and when necessary we will convert the default pixel format of the video to the format specified in the AVPlayerSettings, this frame is used to store the data */
  AVPlayerFrame();
  ~AVPlayerFrame();

  void reset();                                               /* deletes the decoder_frame if it's set; */

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

  bool setup(std::string filename, bool datapath = false, AVPlayerSettings cfg = AVPlayerSettings(), int numPreAllocateFrames = 100);
  bool play();
  bool stop();
  bool pause();
  void draw(int x, int y, int w = 0, int h = 0);

  void lock();
  void unlock();

  int getWidth();
  int getHeight();
  AVPixelFormat getPixelFormat();                                /* returns the pixel format that is used by the decoder */
  uint64_t millis();                                             /* just a helper which returns the current time in millis */

 public:                                                         /* these functions should actually be private, but the thread needs them */     
           
  /* init/shutdown and re-initialize */
  bool open();
  bool isOpen();
  bool initialize();
  bool shutdown();

  /* faking a ring buffer + contexts used when decoding */
  bool needsToConvertPixelFormat();                              /* returns true if we need to convert from the pixel format that the decoder uses by default and the one we have set in the settings.out_pixel_format */
  bool initializeSWS();                                          /* creates a SwsContext if necessary */
  bool getFreeFrames(std::vector<AVPlayerFrame*>& result);       /* will copy all free AVPlayerFrames into the given vector. returns true when there are free frames */                            
  bool allocateFrames();                                         /* this will allocate the AVPlayerFrames that we use when decodding. */
  void deleteFrames();                                           /* this function will deallocate the allocated AVPlayerFrames */
  void freeFrames();                                             /* this will make all frames `free` again so the can be reused */

 public:
  AVPlayerSettings settings;                                   /* see AVTypes; can be used to convert the decoders' default pixel format to some other supported format. On Mac it's recommended to use the AV_PIX_FMT_UYVY422 because that can be used by the `GL_YCBCR_422_APPLE` texture format */

  /* playback */
  std::string filename;                                        /* we make a copy of the filename you pass to `setup()` because we need it to initialize()/shutdown() the player. When you call stop(), we stop the thread and once the thread loop has stopped we first clean all memory before exiting the thread */
  bool datapath;                                               /* is set to true when the filename is relative to the data path */
  int state;                                                   /* used to control playback */
  uint64_t time_started;                                       /* used to calculate what frame we need to show/process */
  uint64_t time_paused;                                        /* we need to keep track of the moment we paused so we can correctly reset/adjust the time started when you call `play()` again. */
  bool must_stop;                                              /* used to stop the thread and shutdown */
  VideoCaptureGLSurface gl_surface;                            /* we use the VideoCapture openGL surface class, because it has an optimized solution for drawing video data; on mac it uses a special texture format */

  /* decoding */
  AVDecoder dec;                                                /* the decoder that we use; this interfaces with libav */
  std::vector<AVPlayerFrame*> frames;                           /* we pre-allocate some frames so we have a small buffer that will hold decoded frames. we decode in a separate thread */
  std::vector<AVPlayerFrame*> decoded_frames;                   /* when a frame is decoded (and maybe the pixel format converted) it's copied to this member that we used to draw the video to the screen */
  int num_frames_to_allocate;                                   /* we pre-allocate and pre-decode a couple of (video) frames so we can playback the frames smoothly */                   
  int nbytes_video_frame;                                       /* number of bytes in the decoded video frames */
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

inline bool AVPlayer::isOpen() {
  return dec.isOpen();
}

inline void AVPlayerFrame::reset() {
  if(decoder_frame) {
    delete decoder_frame;
    decoder_frame = NULL;
  }
}
#endif
