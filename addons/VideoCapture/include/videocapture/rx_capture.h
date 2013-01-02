#ifndef ROXLU_CAPTURE_H
#define ROXLU_CAPTURE_H

//#define ROXLU_CAPTURE_OSX=0
//#define ROXLU_CAPTURE_PLATFORM=ROXLU_CAPTURE_OSX
//#if ROXLU_CAPTURE_PLATFORM == ROXLU_CAPTURE_OSX
#include <videocapture/VideoCaptureMac.h>
//#endif

typedef void(*rx_capture_frame_cb)(void* pixels, size_t nbytes, void* user);

/* 

Funtions which return integer, return:
0 = error
1 > success

 */

// initializes the session 
extern rx_capture_t* rx_capture_init(); 

// returns number of found devices 
extern int rx_capture_list_devices(rx_capture_t* c);

// select the device number (see rx_capture_list_devices) as input and setup a connection 
extern int rx_capture_open_device(rx_capture_t* c, int device);

// start captureing 
extern int rx_capture_start(rx_capture_t* c);

// list as much information about capture devices as possbile
extern int rx_capture_print_verbose_info(rx_capture_t* c);

// sets the callback which is called when a new frame is received 
extern int rx_capture_set_frame_callback(rx_capture_t* c, rx_capture_frame_cb cb, void* user);

// get the width of the capture stream
extern int rx_capture_get_width(rx_capture_t* c);

// get the height of the capture stream
extern int rx_capture_get_height(rx_capture_t* c); 
#endif
