#ifndef ROXLU_VIDEOCAPTURE_GL_SURFACE_H
#define ROXLU_VIDEOCAPTURE_GL_SURFACE_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <videocapture/Types.h>
#include <roxlu/opengl/OpenGLInit.h>
#include <roxlu/opengl/Error.h>
#include <roxlu/core/Log.h>

#define VIDEO_CAP_NUM_PBOS 2
#define VIDEO_CAP_PI 3.14159265

static const char* VIDEO_CAP_VS = GLSL(120,
  uniform mat4 u_mm;
  uniform mat4 u_pm;
  uniform mat4 u_scale_matrix;
  uniform mat4 u_rot_matrix;
  attribute vec4 a_pos;
  attribute vec2 a_tex;
  varying vec2 v_tex;
  void main() {
    gl_Position = u_pm * u_mm * u_rot_matrix * u_scale_matrix * a_pos;
    v_tex = a_tex;
  }
);

static const char* VIDEO_CAP_FS = GLSL(120, 
  uniform sampler2DRect u_tex;
  varying vec2 v_tex;
  void main() {
    vec4 col = texture2DRect(u_tex, v_tex);

    gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    gl_FragColor = col;
  }
);

struct VideoCaptureVertex {
  float x,y,s,t;
  void set(float xx, float yy, float ss, float tt) {  x = xx; y = yy; s = ss; t = tt; }
  void uv(float ss, float tt) { s = ss; t = tt; } 
};


class VideoCaptureGLSurface {
 public:
  VideoCaptureGLSurface();
  ~VideoCaptureGLSurface();
  void setup(int w, int h, VideoCaptureFormat fmt);
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
  VideoCaptureFormat fmt;
  
  static GLuint prog;
  static GLint u_pm;
  static GLint u_mm;
  static GLint u_rot_matrix;
  static GLint u_scale_matrix;
  static GLint u_tex;
  static GLfloat pm[16];

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
