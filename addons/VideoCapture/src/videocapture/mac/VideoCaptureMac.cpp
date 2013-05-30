#include <videocapture/mac/VideoCaptureMac.h>
#include <roxlu/core/Log.h>

VideoCaptureMac::VideoCaptureMac()
  :cap(NULL)
{
  cap = avf_alloc();
  if(!cap) {
    RX_ERROR(ERR_CAP_MAC_CANNOT_ALLOC);
  }
}

VideoCaptureMac::~VideoCaptureMac() {
  RX_ERROR("added a VideoCaptureAVFoundation::dealloc + make sure that all is dealloc'd");
  avf_dealloc(cap);
}

int VideoCaptureMac::listDevices() {
  return avf_list_devices(cap);
}

bool VideoCaptureMac::openDevice(int device, VideoCaptureSettings cfg) {
  if(!cfg.validate()) {
    return false;
  }
  return avf_open_device(cap, device, cfg);
}

bool VideoCaptureMac::closeDevice() {
  return avf_close_device(cap);
}

bool VideoCaptureMac::startCapture() {
  return avf_start_capture(cap);
}

bool VideoCaptureMac::stopCapture() {
  return avf_stop_capture(cap);
}

void VideoCaptureMac::update() {
}

void VideoCaptureMac::setFrameCallback(videocapture_frame_cb frameCB, void* user) {
  avf_set_frame_callback(cap, frameCB, user);
}

std::vector<AVCapability> VideoCaptureMac::getCapabilities(int device) {
  return avf_get_capabilities(cap, device);
}

/*
std::vector<AVRational> VideoCaptureMac::getSupportedFrameRates(int device, int width, int height, enum AVPixelFormat fmt) {

  std::vector<AVRational> result;
  if(!cap) {
    RX_ERROR(ERR_CAP_MAC_PIXFMT_NOT_ALLOC);
    return result;
  }
  
  result = avf_get_framerates(cap, device, width, height, fmt);
  return result;
}


std::vector<enum AVPixelFormat> VideoCaptureMac::getSupportedPixelFormats(int device, int width, int height) {

  std::vector<enum AVPixelFormat> result;
  if(!cap) {
    RX_ERROR(ERR_CAP_MAC_CAP_NOT_ALLOC);
    return result;
  }

  return avf_get_pixel_formats(cap, device, width, height);
}

std::vector<AVSize> VideoCaptureMac::getSupportedSizes(int device) {
  std::vector<AVSize> result;
  if(!cap) {
    RX_ERROR(ERR_CAP_MAC_CAP_NOT_ALLOC);
    return result;
  }
  return avf_get_sizes(cap, device);
}
*/


