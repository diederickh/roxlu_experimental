#include <videocapture/V4L2Capture.h>
#include <videocapture/rx_capture.h>
#include <stdio.h>
#include <signal.h>
#include <vector>

// We need to close each device when the application crashes; On ubuntu 12.10, the
// device isn't shutdown properly.
// 
// @todo figure out how to handle this in a nice/clean way
// --------------------------------------------
static std::vector<rx_capture_t*> rx_capture_instances;
static bool rx_capture_signal_handler_set = false;
static void rx_capture_signal_handler(int sig) {
  for(std::vector<rx_capture_t*>::iterator it = rx_capture_instances.begin(); it != rx_capture_instances.end(); ++it) {
    rx_capture_t* rx = *it;
    rx->cap->closeDevice();
  }
  exit(sig);
}

// --------------------------------------------
rx_capture_t* rx_capture_init() {

  // somehow, when the application crashes/is closed, the webcam is 'disabled' and I need to reboot.
  if(!rx_capture_signal_handler_set) {
    rx_capture_signal_handler_set = true;
    signal(SIGINT, rx_capture_signal_handler);
    signal(SIGABRT, rx_capture_signal_handler);
    signal(SIGILL, rx_capture_signal_handler);
    signal(SIGSEGV, rx_capture_signal_handler);
    signal(SIGTERM, rx_capture_signal_handler);
  }

  rx_capture_t* c = new rx_capture_t();
  c->cap = new V4L2Capture();
  rx_capture_instances.push_back(c);
  return c;
}

int rx_capture_list_devices(rx_capture_t* c) {
  return c->cap->listDevices();
}

int rx_capture_open_device(rx_capture_t* c, int device) {
  return c->cap->openDevice(device);
}

int rx_capture_start(rx_capture_t* c) {
  return c->cap->startCapture();
}

int rx_capture_print_verbose_info(rx_capture_t* c) {
  return c->cap->printVerboseInfo();
}

int rx_capture_set_frame_callback(rx_capture_t* c, rx_capture_frame_cb cb, void* user) {
  printf("@TODO IMPLEMENT SET_FRAME_CALLBACK\n");
  return 0;
}

int rx_capture_get_width(rx_capture_t* c) {
  return c->cap->getWidth();
}

int rx_capture_get_height(rx_capture_t* c) {
  return c->cap->getHeight();
}


