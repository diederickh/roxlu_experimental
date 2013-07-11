/*
 
  # YUYV422 format

  Basically the same as UYVY422 on mac, except for the type. When 
  the GL version doesn't have support for a specialized 422 texture
  format the "UYVY_USE_RGBA8_FORMAT" will be defined. If so, we 
  use a slightly adjusted fragment shader which switches the y1,y2,cb,cr
  elements around (see the shader below). On mac, this is not necessary!

 */

#ifndef ROXLU_GPU_IMAGE_YUYV422_H
#define ROXLU_GPU_IMAGE_YUYV422_H

#include <roxlu/core/Log.h>
#include <gpu/GPUImage_Base.h>
#include <gpu/GPUImage_UYVY422.h>


#if defined(UYVY_USE_RGBA8_FORMAT) 
// When RGBA8 format is used, we need to flip the cb,cr,y1 and y1 around
static const char* YUYV422_FS = ""
  "#version 150\n"
  "uniform sampler2D u_tex;"
  "out vec4 outcol;"
  "in vec2 v_tex;"

  "const vec3 R_cf = vec3(1.164383,  0.000000,  1.596027);"
  "const vec3 G_cf = vec3(1.164383, -0.391762, -0.812968);"
  "const vec3 B_cf = vec3(1.164383,  2.017232,  0.000000);"
  "const vec3 offset = vec3(-0.0625, -0.5, -0.5);"

  "void main() {"
  "  int width = textureSize(u_tex, 0).x * 2;"
  "  float tex_x = v_tex.x; "
  "  int pixel = int(floor(width * tex_x)) % 2; "
  "  vec4 tc =  texture( u_tex, v_tex ).rgba;"
  "  float cr = tc.r; "
  "  float y2 = tc.g; "
  "  float cb = tc.b; "
  "  float y1 = tc.a; "
  "  float y = (pixel == 1) ? y2 : y1; "
  "  vec3 yuv = vec3(y, cb, cr);"
  "  yuv += offset;"
  "  outcol.r = dot(yuv, R_cf);"
  "  outcol.g = dot(yuv, G_cf);"
  "  outcol.b = dot(yuv, B_cf);"
  "  outcol.a = 1.0;"
  "}"
  "";
#endif

class GPUImage_YUYV422 : public GPUImage_UYVY422 {
 public:
  GPUImage_YUYV422();
  ~GPUImage_YUYV422();

#if defined(UYVY_USE_RGBA8_FORMAT)
  bool setup();
#endif

  GLenum getType();
};

#if defined(UYVY_USE_RGBA8_FORMAT)
inline bool GPUImage_YUYV422::setup() {
  return setupShader(UYVY_VS, YUYV422_FS);
}
#endif

inline GLenum GPUImage_YUYV422::getType() {
#if defined(__APPLE__)
  return GL_UNSIGNED_SHORT_8_8_REV_APPLE;
#else
  return GL_UNSIGNED_INT_8_8_8_8;
#endif
}

#endif
