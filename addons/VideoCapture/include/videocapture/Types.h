#ifndef ROXLU_VIDEOCAPTURE_TYPES_H
#define ROXLU_VIDEOCAPTURE_TYPES_H


struct VideoCaptureSettings {
  int width;
  int height;
  enum AVPixelFormat in_pixel_format;
  enum AVPixelFormat out_pixel_format;
};

// Using libavutil/pixfmt.h
enum VideoCaptureFormat {
  VC_FMT_RGB24,   /* packed RGB 8:8:8, 24bpp, RGBRGB.. */
  VC_FMT_YUYV422, /* packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr, mac = kCMPixelFormat_422YpCbCr8_yuvs*/
  VC_FMT_UYVY422, /* packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1, mac = kCMPixelFormat_422YpCbCr8 */ 
  VC_FMT_I420,    /* yuv 4:2:0 */
  VC_NONE
};

#endif
