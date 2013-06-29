#ifndef ROXLU_GPU_IMAGE_H
#define ROXLU_GPU_IMAGE_H

extern "C" {
#  include <libavutil/pixfmt.h>
}

#include <assert.h>
#include <roxlu/opengl/GL.h>
#include <roxlu/core/Utils.h>
#include <gpu/GPUImage_Base.h>
#include <gpu/GPUImage_UYVY422.h>
#include <gpu/GPUImage_YUYV422.h>
#include <gpu/GPUImage_RGB24.h>
#include <gpu/GPUImage_RGBA.h>

class GPUImage {
 public:
  GPUImage();
  ~GPUImage();

  bool setup(AVPixelFormat fmt);                                             /* create a new gpuimage for the given width/height/fmt */
  void genTextures(GLuint* ids, int w, int h);                               /* creates the necessary texture(s) for the format, for planar formats (YUV420P) you need to provide an GLuint array with `getNumTextures()` elements. */
  int getNumTextures();                                                      /* return the number to GL-textures the current image needs; this depends onthe pixel format */
  void deleteTextures(GLuint* ids);                                          /* use the underlaying color space conversion schader to delete the texture(s). We assume we get an array that has `getNumTextures()` elements.  */
  void setPixels(GLuint* ids, char* pixels, int w, int h);                   /* upload pixel data */
  void bindTextures(GLuint* ids);
  
  /* GPUImage_Base wrappers */
  /* ---------------------------------------------------------------- */
  void useProgram();                                                         /* will enable the shader / glUseProgram() */
  void setModelMatrix(const float* mm);                                      /* set the model matrix for the underlaying shader. if you don't want any transformations, use identity  */
  void setProjectionMatrix(const float* pm);                                 /* set the projection matrix */

  /* get texture information */
  GLenum getInternalFormat();                                                /* returns the opengl internal format for glTexImage*D */
  GLenum getFormat();                                                        /* returns the format that describes the texture data; how are the components organized? GL_BGR, GL_RGB, GL_R, etc...  */
  GLenum getType();                                                          /* return the open data type for glTexImage* like functions */
  int getNumPlanes();                                                        /* when the pixel format is planar it returns the number of planes */
  bool isPlanar();                                                           /* check if the pixel format is planar */

  /* general */
  AVPixelFormat getPixelFormat();                                            /* get the pixel format for which this shader is used */
  /* ---------------------------------------------------------------- */

 public:
  GPUImage_Base* imp;                                                        /* the underlaying implementation, this is any of the GPUImage_* types */
};

inline void GPUImage::deleteTextures(GLuint* ids) {
  assert(imp);
  imp->deleteTextures(ids);
}

inline int GPUImage::getNumTextures() {
 assert(imp);
 return imp->getNumTextures();
}

inline void GPUImage::bindTextures(GLuint* ids) {
  assert(imp);
  imp->bindTextures(ids);
}

inline void GPUImage::setPixels(GLuint* dest, char* pixels, int w, int h) {
  assert(imp);
  imp->setPixels(dest, pixels, w, h);
}

inline void GPUImage::useProgram() {
  assert(imp);
  imp->useProgram();
}

inline void GPUImage::setModelMatrix(const float* mm) {
  assert(imp);
  imp->setModelMatrix(mm);
}

inline void GPUImage::setProjectionMatrix(const float* pm) {
  assert(imp);
  imp->setProjectionMatrix(pm);
}

inline GLenum GPUImage::getInternalFormat() {
  assert(imp);
  return imp->getInternalFormat();
}

inline GLenum GPUImage::getFormat() {
  assert(imp);
  return imp->getFormat();
}

inline GLenum GPUImage::getType() {
  assert(imp);
  return imp->getType();
}

inline int GPUImage::getNumPlanes() {
  assert(imp);
  return imp->getNumPlanes();
}

inline bool GPUImage::isPlanar() {
  assert(imp);
  return imp->isPlanar();
}

inline AVPixelFormat GPUImage::getPixelFormat() {
  assert(imp);
  return imp->getPixelFormat();
}

inline void GPUImage::genTextures(GLuint* dest, int w, int h) {
  imp->genTextures(dest, w, h);
}

#endif
