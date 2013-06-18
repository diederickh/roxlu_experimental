/*

  # GPU Drawer

  Helper to draw GPUimages.

 */

#ifndef ROXLU_GPU_IMAGE_DRAWER_H
#define ROXLU_GPU_IMAGE_DRAWER_H

extern "C" {
#  include <libavutil/pixfmt.h>
}

#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <glr/GL.h>
#include <gpu/GPUImage.h>
#include <gpu/GPUImage_Base.h>
#include <gpu/GPUImage_UYVY422.h>

class GPUDrawer {
 public:
  GPUDrawer();
  ~GPUDrawer();
  bool setup();
  bool draw(GPUImage& img, GLuint* ids);
  void setFlipVertical(bool flip);
  void setFlipHorizontal(bool flip);
  void setRotation(float radians);                                      
  void setPosition(float x, float y, float z = 0.0);
  void setSize(float w, float h);
  
 public:
  int flip_x;
  int flip_y;
  Mat4 model_matrix;
  Vec3 rotation;
  Vec3 position;
  Vec3 scale;
  float ortho_matrix[16];
  int viewport_w;
  int viewport_h;
  GPUImage_Base* img;
  GLuint vao;
  GLuint vbo;
};

inline void GPUDrawer::setFlipVertical(bool flip) {
  flip_x = (flip) ? -1 : 1;
}

inline void GPUDrawer::setFlipHorizontal(bool flip) {
  flip_y = (flip) ? -1 : 1;
}

inline void GPUDrawer::setPosition(float x, float y, float z) {
  position.set(x, y, z);
}

inline void GPUDrawer::setSize(float w, float h) {
  scale.set(w, h, 1.0);
}

inline void GPUDrawer::setRotation(float radians) {
  rotation.z = radians;
}


#endif
