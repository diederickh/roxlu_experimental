/*
 
  # UYVY422 format

  Mac: http://www.opengl.org/registry/specs/APPLE/ycbcr_422.txt


 */

#ifndef ROXLU_GPU_IMAGE_UYVY422_H
#define ROXLU_GPU_IMAGE_UYVY422_H

#include <roxlu/core/Log.h>
#include <gpu/GPUImage_Base.h>

#if defined(ROXLU_GL_CORE3)
static const char* UYVY_VS = ""
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

static const char* UYVY_FS = ""
  "#version 150\n"
  "uniform sampler2D u_tex;"
  "out vec4 outcol;"
  "in vec2 v_tex;"

  "const vec3 R_cf = vec3(1.164383,  0.000000,  1.596027);"
  "const vec3 G_cf = vec3(1.164383, -0.391762, -0.812968);"
  "const vec3 B_cf = vec3(1.164383,  2.017232,  0.000000);"
  "const vec3 offset = vec3(-0.0625, -0.5, -0.5);"

  "void main() {"
  "  vec3 tc =  texture( u_tex, v_tex ).rgb;"
  "  vec3 yuv = vec3(tc.g, tc.b, tc.r);"
  "  yuv += offset;"
  "  outcol.r = dot(yuv, R_cf);"
  "  outcol.g = dot(yuv, G_cf);"
  "  outcol.b = dot(yuv, B_cf);"
  "  outcol.a = 1.0;"
  "}"
  "";
#else 
# error "Only GL3 supported"
#endif

class GPUImage_UYVY422 : public GPUImage_Base {
 public:
  GPUImage_UYVY422();
  ~GPUImage_UYVY422();
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

inline void GPUImage_UYVY422::useProgram() {
  glUseProgram(prog);
}


inline int GPUImage_UYVY422::getNumTextures() {
  return 1;
}

inline GLenum GPUImage_UYVY422::getInternalFormat() {
#if defined(__APPLE__)
  return GL_RGB8;
#else
  RX_ERROR("not yet supported");
  return GL_RED;
#endif
}

inline GLenum GPUImage_UYVY422::getFormat() {
#if defined(__APPLE__)
#  if   defined(ROXLU_GL_CORE3)
  return GL_RGB_422_APPLE;
#  else
  return GL_YCBCR_422_APPLE;
#  endif
#else
  RX_ERROR("not yet supported");
  //return GL_RGB_422_APPLE;
  return GL_RED;
#endif
}

inline GLenum GPUImage_UYVY422::getType() {
#if defined(__APPLE__)
  return GL_UNSIGNED_SHORT_8_8_APPLE;
#else
  RX_ERROR("not yet supported");
  return GL_RED;
#endif
}

inline int GPUImage_UYVY422::getNumPlanes() {
  return 1;
}

inline bool GPUImage_UYVY422::isPlanar() {
  return false;
}

#endif
