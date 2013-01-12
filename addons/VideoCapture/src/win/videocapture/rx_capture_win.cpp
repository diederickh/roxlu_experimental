#include <videocapture/VideoCaptureDirectShow.h>
#include <videocapture/rx_capture.h>
#include <stdio.h>
#include <vector>

// --------------------------------------------
int rx_capture_directshow_initialize(rx_capture_t* handle) {
  VideoCaptureDirectShow* ds = new VideoCaptureDirectShow();
  if(!ds) {
    return 0;
  }
  handle->data = (void*)ds;
  return 1;
}

int rx_capture_directshow_list_devices(rx_capture_t* handle) {
  VideoCaptureDirectShow* ds = (VideoCaptureDirectShow*)handle->data;
  return ds->listDevices();
}

int rx_capture_directshow_print_verbose_info(rx_capture_t* handle) {
  VideoCaptureDirectShow* ds = (VideoCaptureDirectShow*)handle->data;
  return ds->printVerboseInfo();
}

int rx_capture_directshow_is_format_supported(rx_capture_t* handle, 
                                              VideoCaptureFormat fmt, 
                                              int w, int h, int set) 
{
  VideoCaptureDirectShow* ds = (VideoCaptureDirectShow*)handle->data;
  return ds->isFormatSupported(fmt, w, h, set);
}

int rx_capture_directshow_open_device(rx_capture_t* handle, 
                                     int device, 
                                     int w, 
                                     int h, 
                                     VideoCaptureFormat fmt) 
{
  VideoCaptureDirectShow* ds = (VideoCaptureDirectShow*)handle->data;
  return ds->openDevice(device, w, h, fmt);
}


int rx_capture_directshow_start_capture(rx_capture_t* handle) {
  VideoCaptureDirectShow* ds = (VideoCaptureDirectShow*)handle->data;
  return ds->startCapture();
}

int rx_capture_directshow_set_frame_callback(rx_capture_t* handle, 
                                             rx_capture_frame_cb cb, 
                                             void* user)
{
  VideoCaptureDirectShow* ds = (VideoCaptureDirectShow*)handle->data;
  return ds->setFrameCallback(cb, user);
}

int rx_capture_directshow_get_width(rx_capture_t* handle) {
  VideoCaptureDirectShow* ds = (VideoCaptureDirectShow*)handle->data;
  return ds->getWidth();
}

int rx_capture_directshow_get_height(rx_capture_t* handle) {
  VideoCaptureDirectShow* ds = (VideoCaptureDirectShow*)handle->data;
  return ds->getHeight();
}

const rx_capture_t rx_capture_directshow = {
  rx_capture_directshow_initialize,
  rx_capture_directshow_list_devices,
  rx_capture_directshow_print_verbose_info,
  rx_capture_directshow_is_format_supported,
  rx_capture_directshow_open_device,
  rx_capture_directshow_start_capture,
  rx_capture_directshow_set_frame_callback,
  rx_capture_directshow_get_width,
  rx_capture_directshow_get_height
};
