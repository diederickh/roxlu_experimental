/*

 Thin C-wrapper between OBJ-C and C++

 */

#ifndef ROXLU_VIDEOCAPTURE_MAC_C_INTERFACE_H
#define ROXLU_VIDEOCAPTURE_MAC_C_INTERFACE_H

#include <vector>

extern "C" {
#  include <libavformat/avformat.h>
}

void* avf_alloc();
void avf_dealloc(void* cap);
int avf_list_devices(void* cap);
int avf_open_device(void* cap, int device, VideoCaptureSettings cfg);
int avf_close_device(void* cap);
int avf_start_capture(void* cap);
int avf_stop_capture(void* cap);
void avf_update(void* cap);
void avf_set_frame_callback(void* cap, videocapture_frame_cb frameCB, void* user);
std::vector<AVCapability> avf_get_capabilities(void* cap, int device);

/*
std::vector<AVRational> avf_get_framerates(void* cap, int device, int width, int height, enum AVPixelFormat fmt);
std::vector<enum AVPixelFormat> avf_get_pixel_formats(void* cap, int device, int width, int height);
std::vector<AVSize> avf_get_sizes(void* cap, int device);
*/
#endif
