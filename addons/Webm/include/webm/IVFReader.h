#ifndef ROXLU_WEBM_IVF_READER_H
#define ROXLU_WEBM_IVF_READER_H

#include <string>
#include <vector>
#include <deque>
#include <roxlu/io/RingBuffer.h>
#include <roxlu/core/Utils.h>
#include <webm/VPXEncoder.h> /* for the VPXSettings */
#include <webm/VPXDecoder.h>

#define IVF_ERR_FOPEN "Cannot open the file: %s"
#define IVF_ERR_READ_HEADER "Cannot read the ivf-header"
#define IVF_ERR_INVALID_FILE "Not a correct ivf-file"
#define IVF_ERR_INVALID_SIZE "The ivf-header contains an invalid width or height (%d, %d)"
#define IVF_ERR_FRAME_TOO_BIG "The current frame size is too big to handle"
#define IVF_ERR_READ_FRAME "Cannot read the frame"
#define IVF_ERR_FRAME_READ_HDR "Cannot read frame header.. at the end the file?"
#define IVF_ERR_FRAME_STATE "Cannot read a frame because the current state is not IVF_STATE_PLAY"
#define IVF_ERR_DECODER_SETUP "Cannot setup the decoder"
#define IVF_ERR_ALREADY_OPEN "We've already opened a ivf file"
#define IVF_ERR_CLOSE "Cannot close ivf as we haven't opened a file yet"
#define IVF_ERR_ALREADY_PLAYING "We're already playing. First call stop, then play again to restart"
#define IVF_ERR_HDR_FILE "Cannot read the IVF header because the file pointer is invalid"
#define IVF_ERR_HDR_SEEK "Cannot set the file pointer to the beginning of the file"
#define IVF_V_ALREADY_PAUSED "We're already paused... not pausing again :) "

#define IVF_FILE_HDR_SIZE 32
#define IVF_FRAME_HDR_SIZE 12

struct IVFFrame {
  IVFFrame();
  size_t offset;                                                    /* offset in the buffer */
  uint64_t pts;                                                     /* presentation timestamp from the ivf file */
  uint64_t millis;                                                  /* time in millis when this frame should be played back */
  size_t size;                                                      /* number of bytes */
};

#define IVF_STATE_NONE 0
#define IVF_STATE_PLAY 1
#define IVF_STATE_PAUSE 2

class IVFReader {
 public:
  IVFReader(vpx_read_cb readCB, void* readUser = NULL);
  ~IVFReader();
  bool open(std::string filename, bool datapath = false);            /* open a file for reading */
  bool close();                                                      /* close the current file and reset everythin */
  void print();                                                      /* print some verbose info */
  void play();                                                       /* start playing back */
  void stop();                                                       /* stop the current playback */
  void pause();                                                      /* pause the current playback */
  void update();                                                     /* call this as often as possible */
  void setFPS(double fps);                                           /* force this FPS, when you use this function we assume you're using `avconv` as described in the README.md because somehow the timebase with avconv is always 1/90000 and it's not setting the correct fps. */
 private:
  bool readHeader();                                                 /* read the file header, once the file has been opened, this can be called any time after you've opened a file.. it will reset the read pointer to the start of the file. see stop() of all other house keeping you need to do when you want to restart a stream */
  void reset();                                                      /* resets all members used while decoding (does not reset the state, width/height etc..) */
  bool shutdown();                                                   /* destructor; resets everything to the same state as when the object was created */
  bool readFrame();                                                  /* reads a raw encoded frame and adds a new IVFFrame to the "frames" buffer that is used when playing back */
  void updateBuffer();                                               /* updates the buffer with read (encoded) frames */ 
 public:
  /* playback */
  int state;                                                         /* current state ... */
  uint64_t time_started;                                             /* time the playback started, in millis */
  uint64_t time_paused;
  double time_base;                                                  /* used to convert pts (presentation time stamps) to current playback time */
  size_t frame_dx;                                                   /* current read frame is reset to 0 after decoding, in update() */
  ssize_t last_frame_dx;                                             /* last frame dx we showed */

  /* buffer */
  RingBuffer rbuffer;
  int num_frames_to_buffer;                                          /* number of frames we need to pre-buffer so we have a smooth playback */
  size_t num_read_frames;                                            /* how many raw frames have been read */
  std::deque<IVFFrame> frames;                                       /* contains IVFFrame information that is used to synchronize the IVF timestamps when playing back the file*/
    
  /* decoder */
  VPXDecoder* decoder;                                               /* the decoder wrapper */
  vpx_read_cb cb_read;                                               /* function that gets called by the VPXDecoder when it has decoded a frame */
  void* cb_user;                                                     /* internally used by the VPXDecoder */

  /* reading IVF */
  FILE* fp;                                                          /* the file pointer of the currenet ivf file, see IVF::open() */
  uint16_t width;                                                    /* width of the video */
  uint16_t height;                                                   /* height of the video */
  uint32_t rate;                                                     /* time base, rate: used in combination with the pts values (presentation time stamps) while playing back, a value like  90000 is used by the avconv util when converting a 29.97 fps video to ivf */
  uint32_t scale;                                                    /* time base, scale: probably something like 1  */
  uint32_t num_frames;                                               /* number of frames in the ivf file.. might be 0 when the encoder doesn't store this value.. it's not used for playback */
  unsigned char frame_hdr[IVF_FRAME_HDR_SIZE];                       /* each frame contains a small header */
  unsigned char frame_data[256 * 1024];                              /* buffer for one frame */
};

#endif
