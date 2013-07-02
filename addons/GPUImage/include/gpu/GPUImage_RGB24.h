/*
 
  # RGB24 format

 */

#ifndef ROXLU_GPU_IMAGE_RGB24_H
#define ROXLU_GPU_IMAGE_RGB24_H

#include <gpu/GPUImage_Base.h>

#if defined(ROXLU_GL_CORE3)
static const char* RGB_VS = ""
  "#version 150\n"
  "uniform mat4 u_pm;"
  "uniform mat4 u_mm;"
  "in vec4 a_pos;"
  "in vec2 a_tex;"
  "out vec2 v_tex;"

  "void main() {"
  " gl_Position = u_pm * u_mm * a_pos; "
  " v_tex = a_tex;"
  "}"
  "";

static const char* RGB_FS = ""
  "#version 150\n"
  "uniform sampler2D u_tex;"
  "out vec4 outcol;"
  "in vec2 v_tex;"

  "void main() {"
  "  outcol =  texture( u_tex, v_tex );"
  "}"

  "";
#else 
# error "Only GL3 supported"
#endif

class GPUImage_RGB24 : public GPUImage_Base {
 public:
  GPUImage_RGB24();
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

  /* texture info */
  GLenum getInternalFormat();
  GLenum getFormat();
  GLenum getType();
  int getNumPlanes();
  bool isPlanar();

 public:
  GLint u_mm;
  GLint u_pm;
  GLint u_tex;
};

inline void GPUImage_RGB24::useProgram() {
  glUseProgram(prog);
}

inline int GPUImage_RGB24::getNumTextures() {
  return 1;
}

inline GLenum GPUImage_RGB24::getInternalFormat() {
  return GL_RGB8;
}

inline GLenum GPUImage_RGB24::getFormat() {
  return GL_RGB;
}

inline GLenum GPUImage_RGB24::getType() {
  return GL_UNSIGNED_BYTE;
}

inline int GPUImage_RGB24::getNumPlanes() {
  return 1;
}

inline bool GPUImage_RGB24::isPlanar() {
  return false;
}

#endif
