#include <videocapture/VideoCaptureDirectShow.h>
#include <videocapture/rx_capture.h>
#include <stdio.h>
#include <vector>

// --------------------------------------------
rx_capture_t* rx_capture_init() {
  rx_capture_t* c = new rx_capture_t();
  c->cap = new VideoCaptureDirectShow();
  return c;
}

int rx_capture_list_devices(rx_capture_t* c) {
  return c->cap->listDevices();
}

int rx_capture_open_device(rx_capture_t* c, int device, int w, int h, VideoCaptureFormat fmt) {
  return c->cap->openDevice(device, w, h, fmt);
}

int rx_capture_start(rx_capture_t* c) {
  return c->cap->startCapture();
}

int rx_capture_print_verbose_info(rx_capture_t* c) {
  return c->cap->printVerboseInfo();
}

int rx_capture_set_frame_callback(rx_capture_t* c, rx_capture_frame_cb cb, void* user) {
  return c->cap->setFrameCallback(cb, user);
}

int rx_capture_get_width(rx_capture_t* c) {
  return c->cap->getWidth();
}

int rx_capture_get_height(rx_capture_t* c) {
  return c->cap->getHeight();
}

int rx_capture_is_format_supported(rx_capture_t* c, VideoCaptureFormat fmt, int w, int h, int set) {
  return c->cap->isFormatSupported(fmt, w, h, set);
}

