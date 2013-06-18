/*
 
  # YUYV422 format

  Basically the same as UYVY422 on mac, except for the type

 */

#ifndef ROXLU_GPU_IMAGE_YUYV422_H
#define ROXLU_GPU_IMAGE_YUYV422_H

#include <gpu/GPUImage_Base.h>
#include <gpu/GPUImage_UYVY422.h>

class GPUImage_YUYV422 : public GPUImage_UYVY422 {
 public:
  GPUImage_YUYV422();
  ~GPUImage_YUYV422();
  GLenum getType();
};


inline GLenum GPUImage_YUYV422::getType() {
#if defined(__APPLE__)
  return GL_UNSIGNED_SHORT_8_8_REV_APPLE;
#else
  #error not yet supported 
#endif
}


#endif
