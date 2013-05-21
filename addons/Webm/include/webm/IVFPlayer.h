#ifndef ROXLU_IVF_PLAYER_H
#define ROXLU_IVF_PLAYER_H

#include <webm/IVFReader.h>
#include <videocapture/VideoCaptureGLSurface.h>

void ivf_player_read(unsigned char* pixels, size_t nbytes, void* user);

class IVFPlayer {
 public:
  IVFPlayer();
  bool open(std::string filename, bool datapath = false);          /* open an ivf file */
  bool close();                                                    /* close the opened file */
  void play();                                                     /* start playing */
  void pause();                                                    /* pause playback */
  void stop();                                                     /* stop playback and go to the begin */
  void update();                                                   /* call this repeatedly, it will update the frames */
  void draw(int x, int y, int w = 0, int h = 0);                   /* draw ... */
  void setFPS(double fps);                                         /* set the FPS, see the info in IVF.h */
  int getWidth();                                                  /* returns the width of the video, as stored in the ivf file header */
  int getHeight();                                                 /* returns the height of the video */

 public:
  IVFReader ivf;
  VideoCaptureGLSurface gl_surface;
};

inline void IVFPlayer::setFPS(double fps) {
  ivf.setFPS(fps);
}

inline bool IVFPlayer::close() {
  return ivf.close();
}

inline void IVFPlayer::stop() {
  ivf.stop();
}

inline void IVFPlayer::pause() {
  ivf.pause();
}

inline void IVFPlayer::update() {
  ivf.update();
}

inline void IVFPlayer::play() {
  ivf.play();
}

inline int IVFPlayer::getWidth() {
  return ivf.width;
}

inline int IVFPlayer::getHeight() {
  return ivf.height;
}

#endif
