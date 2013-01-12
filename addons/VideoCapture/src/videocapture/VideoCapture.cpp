#include <videocapture/VideoCapture.h>

void video_capture_callback(void* pixels, size_t nbytes, void* user) {
  VideoCapture* c = static_cast<VideoCapture*>(user);
  if(c->allocated_bytes < nbytes) {
    while(c->allocated_bytes < nbytes) {
      c->allocated_bytes = std::max<size_t>(c->allocated_bytes * 2, 4096);
    }
    
    if(c->bytes != NULL) {
      delete[] c->bytes;
    }
    c->bytes = new unsigned char[c->allocated_bytes];
  }
  memcpy(c->bytes, pixels, nbytes);
  c->has_new_data = true;
  c->nbytes = nbytes;
}

VideoCapture::VideoCapture()
  :allocated_bytes(0)
  ,has_new_data(false)
  ,nbytes(0)
  ,bytes(NULL)
{
#if defined(__APPLE__)
  capture = rx_capture_avfoundation;
#elif defined(_WIN32)
  capture = rx_capture_directshow;
#else 
 #error WE HAVE NOT YET CREATED A VIDEO CAPTURE WRAPPER FOR THIS PLATFORM
#endif
  capture.initialize(&capture);
}

VideoCapture::~VideoCapture() {
}

