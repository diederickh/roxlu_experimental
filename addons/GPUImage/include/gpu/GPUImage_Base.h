#ifndef ROXLU_GPU_IMAGE_BASE_H
#define ROXLU_GPU_IMAGE_BASE_H

extern "C" {
#  include <libavutil/pixfmt.h>
}


#include <roxlu/opengl/GL.h>
#include <roxlu/core/Utils.h>

class GPUImage_Base {
 public:
  GPUImage_Base(AVPixelFormat fmt);
  virtual ~GPUImage_Base();

  virtual bool setup() = 0;                                                  /* creates the color conversion shader */
  virtual void useProgram() = 0;                                             /* enable the program that uses the color conversion shader; make sure to call this before calling `setModelMatrix()`, `setProjectionMatrix()` */
  virtual void setModelMatrix(const float* mm) = 0;                          /* set the model matrix, call `useProgram()` once first */
  virtual void setProjectionMatrix(const float* pm) = 0;                     /* set the projection matrix, call `useProgram()` once first */
  virtual int getNumTextures() = 0;                                          /* return the number of texture needed; make sure that the array you pass into genTextures() has space for this amount of textures */
  virtual void genTextures(GLuint* ids, int w, int h) = 0;                   /* create the necessary textures for the shader */
  virtual void setPixels(GLuint* ids, char* pixels, int w, int h) = 0;       /* set the pixels for the given textures id(s). the ids should be an array with the same amount of textures as `getNumTextures()`. Some color formats use multiple textures to do the conversion */
  virtual void bindTextures(GLuint* ids) = 0;                                /* bind the textures at the correct texture units and set the uniforms. make sure to call `useProgram()` once first */

  /* get texture information */
  virtual GLenum getInternalFormat() = 0;
  virtual GLenum getFormat() = 0;
  virtual GLenum getType() = 0;
  virtual int getNumPlanes() = 0;
  virtual bool isPlanar() = 0;

  /* general + helpers*/
  void deleteTextures(GLuint* ids);                            /* helper to delete the given textures for the current format; so we assume that we receive an array of ids with the same size as `getNumTextures()` */
  AVPixelFormat getPixelFormat();                              /* get the pixel format for which this shader is used */
  void shutdown();                                             /* frees memory, delete shaders/program */

 public:
  AVPixelFormat fmt;
  GLuint prog;
  GLuint vert_id;
  GLuint frag_id;
};

inline AVPixelFormat GPUImage_Base::getPixelFormat() {
  return fmt;
}

#endif
