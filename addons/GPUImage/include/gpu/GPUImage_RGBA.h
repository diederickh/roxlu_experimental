/*
 
  # RGBA format

 */

#ifndef ROXLU_GPU_IMAGE_RGBA_H
#define ROXLU_GPU_IMAGE_RGBA_H

#include <gpu/GPUImage_RGB24.h>



class GPUImage_RGBA : public GPUImage_RGB24 {
 public:
  /*
  GPUImage_RGBA();
  ~GPUImage_RGB24();
  bool setup();
  void useProgram();
  void setProjectionMatrix(const float* pm);
  void setModelMatrix(const float* mm);
  void setTextureUniform(int unint);
  void genTextures(GLuint* ids, int w, int h);
  void setPixels(GLuint* ids, char* pixels, int w, int h);
  void bindTextures(GLuint* ids);
  int getNumTextures();
  */
  /* texture info */
  GLenum getInternalFormat();
  GLenum getFormat();
  //  GLenum getType();
  //  int getNumPlanes();
  //  bool isPlanar();
  /*
 public:
  GLint u_mm;
  GLint u_pm;
  GLint u_tex;
  */
};

/*
inline void GPUImage_RGB24::useProgram() {
  glUseProgram(prog);
}

inline int GPUImage_RGB24::getNumTextures() {
  return 1;
}

*/
inline GLenum GPUImage_RGBA::getInternalFormat() {
  return GL_RGBA8;
}

inline GLenum GPUImage_RGBA::getFormat() {
  return GL_RGBA;
}
/*
inline GLenum GPUImage_RGB24::getType() {
  return GL_UNSIGNED_BYTE;
}

inline int GPUImage_RGB24::getNumPlanes() {
  return 1;
}

inline bool GPUImage_RGB24::isPlanar() {
  return false;
}

*/
#endif
