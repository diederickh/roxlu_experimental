#ifndef ROXLU_RXCAPTURE_H
#define ROXLU_RXCAPTURE_H

#include <videocapture/Types.h>

// callback on framedata
typedef void(*rx_capture_frame_cb)(void* pixels, size_t nbytes, void* user);

/* 

Funtions which return integer, return:
0 = error
1 > success

*/

struct rx_capture_t {
  int (*initialize)(rx_capture_t* handle);
  int (*list_devices)(rx_capture_t* handle);
  int (*print_verbose_info)(rx_capture_t* handle);
  int (*is_format_supported)(rx_capture_t* handle, VideoCaptureFormat fmt, int w, int h, int set);
  int (*open_device)(rx_capture_t* handle, int device, int w, int h, VideoCaptureFormat fmt);
  int (*close_device)(rx_capture_t* handle);
  int (*capture_start)(rx_capture_t* handle);
  void (*update)(rx_capture_t* handle);  // must be called regularly for poll based (v4l2) io
  int (*set_frame_callback)(rx_capture_t* handle, rx_capture_frame_cb cb, void* user);
  int (*get_width)(rx_capture_t* handle);
  int (*get_height)(rx_capture_t* handle);
  void* data;
};

#if !defined(_WIN32) && defined(__APPLE__)
extern const rx_capture_t rx_capture_avfoundation;
#endif

#if defined(_WIN32)
extern const rx_capture_t rx_capture_directshow;
#endif

#ifdef __linux
extern const rx_capture_t rx_capture_v4l2;
#endif
#endif
