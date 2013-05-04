#ifndef ROXLU_OPENGL_FILTER_H
#define ROXLU_OPENGL_FILTER_H

#include <roxlu/opengl/GL.h>
#include <glr/FBO.h>
#include <glr/Texture.h>

namespace gl { 

  class Filter {
  public:
    virtual bool setup(int w, int h); 
    virtual void render(GLuint tex); 
  public:
    GLuint prog;
  };

} // namespace gl

#endif
