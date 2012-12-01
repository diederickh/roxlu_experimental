#include <cv/VideoGrabber.h>
namespace roxlu {
VideoGrabber::VideoGrabber()
  :w(0)
  ,h(0)
  ,pixels(NULL)
{
}

VideoGrabber::~VideoGrabber() {
  if(pixels != NULL) {
    delete[] pixels;
  }
}

void VideoGrabber::setup(int w, int h, int device) {
}

void VideoGrabber::listDevices() {
}

bool VideoGrabber::grabFrame() {
  return false;
}

}
