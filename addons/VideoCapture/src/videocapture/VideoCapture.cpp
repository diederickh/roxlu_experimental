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
  c = rx_capture_init();
}

VideoCapture::~VideoCapture() {
}

int VideoCapture::listDevices() {
  return rx_capture_list_devices(c);
}

int VideoCapture::printVerboseInfo() {
  return rx_capture_print_verbose_info(c);
}

int VideoCapture::openDevice(int device) {
  rx_capture_set_frame_callback(c, video_capture_callback, this);
  return rx_capture_open_device(c, device);
}

int VideoCapture::startCapture() {
  return rx_capture_start(c);
}

