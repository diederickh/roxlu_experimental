#include <videocapture/VideoCaptureV4L2.h>
#include <videocapture/rx_capture.h>
#include <stdio.h>
#include <signal.h>
#include <vector>

// We need to close each device when the application crashes; On ubuntu 12.10, the
// device isn't shutdown properly.
// 
// @todo figure out how to handle this in a nice/clean way
// --------------------------------------------
static std::vector<VideoCaptureV4L2*> rx_capture_instances;
static bool rx_capture_signal_handler_set = false;
static void rx_capture_signal_handler(int sig) {
  printf("________________________SIG HANDLER _________________________\n");
  for(std::vector<VideoCaptureV4L2*>::iterator it = rx_capture_instances.begin(); it != rx_capture_instances.end(); ++it) {
    VideoCaptureV4L2* rx = *it;
    rx->closeDevice();
  }
  exit(sig);
}

// --------------------------------------------
int rx_capture_v4l2_initialize(rx_capture_t* handle) {

  // somehow, when the application crashes/is closed, the webcam is 'disabled' and I need to reboot.
  if(!rx_capture_signal_handler_set) {
    rx_capture_signal_handler_set = true;
    signal(SIGINT, rx_capture_signal_handler);
    signal(SIGABRT, rx_capture_signal_handler);
    signal(SIGILL, rx_capture_signal_handler);
    signal(SIGSEGV, rx_capture_signal_handler);
    signal(SIGTERM, rx_capture_signal_handler);
  }

  VideoCaptureV4L2* cap = new VideoCaptureV4L2();
  rx_capture_instances.push_back(cap);
  handle->data = (void*)cap;
  return 1;
}

int rx_capture_v4l2_list_devices(rx_capture_t* handle) {
  VideoCaptureV4L2* c = (VideoCaptureV4L2*)handle->data;
  return c->listDevices();
}

int rx_capture_v4l2_print_verbose_info(rx_capture_t* handle) {
  VideoCaptureV4L2* c = (VideoCaptureV4L2*)handle->data;
  return c->printVerboseInfo();
}

int rx_capture_v4l2_is_format_supported(rx_capture_t* handle, VideoCaptureFormat fmt, int w, int h, int set) {
  printf("ERROR: we need to implement is_format_supported for v4l2\n");
  return 1;
}

int rx_capture_v4l2_open_device(rx_capture_t* handle, int device, int w, int h, VideoCaptureFormat fmt) {
  VideoCaptureV4L2* c = (VideoCaptureV4L2*)handle->data;
  return c->openDevice(device, w, h, fmt);
}

int rx_capture_v4l2_close_device(rx_capture_t* handle) {
  VideoCaptureV4L2* c = (VideoCaptureV4L2*)handle->data;
  return c->closeDevice();
}

int rx_capture_v4l2_start_capture(rx_capture_t* handle) {
  VideoCaptureV4L2* c = (VideoCaptureV4L2*)handle->data;
  return c->startCapture();
}

void rx_capture_v4l2_update(rx_capture_t* handle) {
 VideoCaptureV4L2* c = (VideoCaptureV4L2*)handle->data;
 c->update();
}


int rx_capture_v4l2_set_frame_callback(rx_capture_t* handle, rx_capture_frame_cb cb, void* user) {
  VideoCaptureV4L2* c = (VideoCaptureV4L2*)handle->data;
  c->setFrameCallback(cb, user);
  return 0;
}

int rx_capture_v4l2_get_width(rx_capture_t* handle) {
  VideoCaptureV4L2* c = (VideoCaptureV4L2*)handle->data;
  return c->getWidth();
}

int rx_capture_v4l2_get_height(rx_capture_t* handle) {
  VideoCaptureV4L2* c = (VideoCaptureV4L2*)handle->data;
  return c->getHeight();
}

const rx_capture_t rx_capture_v4l2 = { 
  rx_capture_v4l2_initialize,
  rx_capture_v4l2_list_devices,
  rx_capture_v4l2_print_verbose_info,
  rx_capture_v4l2_is_format_supported,
  rx_capture_v4l2_open_device,
  rx_capture_v4l2_close_device,
  rx_capture_v4l2_start_capture,
  rx_capture_v4l2_update,
  rx_capture_v4l2_set_frame_callback,
  rx_capture_v4l2_get_width,
  rx_capture_v4l2_get_height
};

