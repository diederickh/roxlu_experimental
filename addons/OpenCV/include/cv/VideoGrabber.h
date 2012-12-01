#ifndef ROXLU_VIDEOGRABBER_H
#define ROXLU_VIDEOGRABBER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace roxlu {
  class VideoGrabber {
  public:
    VideoGrabber();
    ~VideoGrabber();
    void listDevices();
    void setup(int w, int h, int device);
    bool grabFrame();
    const unsigned char* getPixels();
    int getWidth();
    int getHeight();
  private:
    int w;
    int h;
    unsigned char* pixels;
    cv::VideoCapture capture;
  };

  inline int VideoGrabber::getWidth() {
    return w;
  }

  inline int VideoGrabber::getHeight() {
    return h;
  }

  inline const unsigned char* VideoGrabber::getPixels() {
    return pixels;
  }
} // roxlu
#endif
