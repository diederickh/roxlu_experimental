// @deprecated - GPUImage/GPUImageDrawer have taken this functionality in
// a separate addon. 2013.06.18
#ifndef ROXLU_VIDEOCAPTURE_GL_SURFACE_H
#define ROXLU_VIDEOCAPTURE_GL_SURFACE_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <roxlu/opengl/GL.h>
#include <videocapture/Types.h>
#include <roxlu/opengl/Error.h>
#include <roxlu/core/Log.h>

#define VIDEO_CAP_NUM_PBOS 2
#define VIDEO_CAP_PI 3.14159265

// @todo create GL3 versions of the video capture shaders
#if defined(ROXLU_GL_CORE3) 
static const char* VIDEO_CAP_VS = "" 
  " #version 150\n"
  " uniform mat4 u_mm; "
  " uniform mat4 u_pm; "
  " uniform mat4 u_scale_matrix; "
  " uniform mat4 u_rot_matrix; "
  " in vec4 a_pos; "
  " in vec2 a_tex; "
  " out vec2 v_tex;" 
  " void main() { "
  "   gl_Position = u_pm * u_mm * u_rot_matrix * u_scale_matrix * a_pos; "
  "   v_tex = a_tex; "
  " }" 
  "";


static const char* VIDEO_CAP_FS = ""
  " #version 150\n"
  " uniform sampler2DRect u_tex; " 
  " in vec2 v_tex; "
  " out vec4 outcol; "
  " void main() { "
  "   vec4 col = texture(u_tex, v_tex); "
  "   outcol = col; "
  "   outcol.a = 1.0; "
  " } "
  "";
#else 
static const char* VIDEO_CAP_VS = "" 
  " #version 120\n"
  " #extension GL_ARB_texture_rectangle : enable\n"
  " uniform mat4 u_mm; "
  " uniform mat4 u_pm; "
  " uniform mat4 u_scale_matrix; "
  " uniform mat4 u_rot_matrix; "
  " attribute vec4 a_pos; "
  " attribute vec2 a_tex; "
  " varying vec2 v_tex;" 
  " void main() { "
  "   gl_Position = u_pm * u_mm * u_rot_matrix * u_scale_matrix * a_pos; "
  "  v_tex = a_tex; "
  " }" 
  "";


static const char* VIDEO_CAP_FS = ""
  " #version 120\n"
  " #extension GL_ARB_texture_rectangle : enable\n"
  " uniform sampler2DRect u_tex; " 
  " varying vec2 v_tex; "
  " void main() { "
  "   vec4 col = texture2DRect(u_tex, v_tex); "
  "   gl_FragColor = col; "
  "   gl_FragColor.a = 1.0; "
  " } "
  "";
#endif

struct VideoCaptureVertex {
  float x,y,s,t;
  void set(float xx, float yy, float ss, float tt) {  x = xx; y = yy; s = ss; t = tt; }
  void uv(float ss, float tt) { s = ss; t = tt; } 
};


class VideoCaptureGLSurface {
 public:
  VideoCaptureGLSurface();
  ~VideoCaptureGLSurface();

#if defined(__APPLE__) && !defined(ROXLU_WITH_GLFW3_AND_GLXW)
  void setup(int w, int h, GLenum internalFormat = GL_RGBA, GLenum format = GL_YCBCR_422_APPLE, GLenum type = GL_UNSIGNED_SHORT_8_8_APPLE);     /* the GL_YCBCR_422_APPLE format is the same as libav AV_PIX_FMT_UYVY422, also called "yuv2" */
#else
  void setup(int w, int h, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE);
#endif

  void setPixels(unsigned char* pixels, size_t nbytes);
  void draw(int x, int y, int width = 0, int height = 0);
  void reset();
  void flip(bool vert, bool hor);
  void rotate(float degrees);
 private:
  void setupGL();
 private:
  bool is_setup;
  int surface_w;
  int surface_h;
  int window_w;
  int window_h;
  
  static GLuint prog;
  static GLint u_pm;
  static GLint u_mm;
  static GLint u_rot_matrix;
  static GLint u_scale_matrix;
  static GLint u_tex;
  static GLfloat pm[16];

  GLenum tex_internal_format;
  GLenum tex_format;
  GLenum tex_type;

  GLuint vbo;
  GLuint vao;
  GLuint tex;
  GLuint pbos[VIDEO_CAP_NUM_PBOS];
  GLfloat mm[16];
  GLfloat rot_matrix[16];
  GLfloat scale_matrix[16];
  int write_dx;
  int read_dx;
  int num_bytes;
  VideoCaptureVertex vertices[6];
};


#endif
