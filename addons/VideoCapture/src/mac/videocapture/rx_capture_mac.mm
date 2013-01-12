#ifndef ROXLU_VIDEOCAPTURE_MAC_H
#define ROXLU_VIDEOCAPTURE_MAC_H

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <videocapture/VideoCapture.h>
#import <videocapture/VideoCaptureAVFoundation.h>

int rx_capture_avfoundation_initialize(rx_capture_t* handle) {
  VideoCaptureAVFoundation* c = [[VideoCaptureAVFoundation alloc] init];
  handle->data = (void*)c;
  return 1;
}

int rx_capture_avfoundation_list_devices(rx_capture_t* handle) {
  return  [(id)handle->data listDevices];
}

int rx_capture_avfoundation_is_format_supported(
  rx_capture_t* handle, 
  VideoCaptureFormat fmt, 
  int w, int h, int set
)
{
  return [(id)handle->data isFormatSupported: fmt forWidth: w andHeight: h andApply: set];
}

int rx_capture_avfoundation_print_verbose_info(rx_capture_t* c) {
  return [(id)c->data printVerboseInfo];
}

int rx_capture_avfoundation_open_device(
   rx_capture_t* c, 
   int device, 
   int w, 
   int h, 
   VideoCaptureFormat fmt
)
{
   return [(id)c->data openDevice:device 
                       withWidth:w andHeight:h
                       andFormat:fmt andApply:1];
}

int rx_capture_avfoundation_start_capture(rx_capture_t* c) {
  return [(id)c->data captureStart];
}

int rx_capture_avfoundation_set_frame_callback(rx_capture_t* c, rx_capture_frame_cb cb, void* userData) {
  [(id)c->data setFrameCallback:cb user:userData];
  return 1;
}

int rx_capture_avfoundation_get_width(rx_capture_t* c) {
  return [(id)c->data getWidth];
}

int rx_capture_avfoundation_get_height(rx_capture_t* c) {
  return [(id)c->data getHeight];
}

const rx_capture_t rx_capture_avfoundation = { 
  rx_capture_avfoundation_initialize, 
  rx_capture_avfoundation_list_devices,
  rx_capture_avfoundation_print_verbose_info,
  rx_capture_avfoundation_is_format_supported,
  rx_capture_avfoundation_open_device,
  rx_capture_avfoundation_start_capture,
  rx_capture_avfoundation_set_frame_callback,
  rx_capture_avfoundation_get_width,
  rx_capture_avfoundation_get_height
};

#endif


