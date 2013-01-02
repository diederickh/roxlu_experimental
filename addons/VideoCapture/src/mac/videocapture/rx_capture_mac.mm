#ifndef ROXLU_VIDEOCAPTURE_MAC_H
#define ROXLU_VIDEOCAPTURE_MAC_H

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <videocapture/VideoCapture.h>

rx_capture_t* rx_capture_init() {
  rx_capture_t* c = [[rx_capture_t alloc] init];
  return c;
}

int rx_capture_list_devices(rx_capture_t* c) {
  return [c listDevices];
}

int rx_capture_print_verbose_info(rx_capture_t* c) {
  return [c printVerboseInfo];
}

int rx_capture_open_device(rx_capture_t* c, int device) {
  printf("open device\n");
  return [c openDevice:device];
}

int rx_capture_start(rx_capture_t* c) {
  return [c start];
}

int rx_capture_set_frame_callback(rx_capture_t* c, rx_capture_frame_cb cb, void* userData) {
  [c setFrameCallback:cb user:userData];
  return 1;
  //- (void) setFrameCallback:(rx_mac_capture_frame_cb) frameCB user:(void*) frameUser;
}

int rx_capture_get_width(rx_capture_t* c) {
  return [c getWidth];
}

int rx_capture_get_height(rx_capture_t* c) {
  return [c getHeight];
}
#endif


