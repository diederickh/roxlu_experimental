#ifndef ROXLU_OPENGL_FILTER_H
#define ROXLU_OPENGL_FILTER_H

#include <roxlu/opengl/GL.h>
#include <experimental/Types.h>
#include <glr/FBO.h>
#include <glr/Texture.h>

namespace gl { 


  class Filter {
  public:
    virtual bool setup(int w, int h); 
    virtual void render(); 

    void setInput(Attachment a);  // set default input 
    void setOutput(Attachment a); 
    
  public:
    GLuint prog;
    Attachment input;
    Attachment output;
  };

} // namespace gl

#endif
