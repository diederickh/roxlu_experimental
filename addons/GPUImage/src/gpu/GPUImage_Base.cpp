#include <gpu/GPUImage_Base.h>

GPUImage_Base::GPUImage_Base(AVPixelFormat fmt)
  :fmt(fmt)
  ,prog(0)
  ,vert_id(0)
  ,frag_id(0)
{
}

GPUImage_Base::~GPUImage_Base() {
  shutdown();
}

void GPUImage_Base::shutdown() {
  if(vert_id) {
    glDeleteShader(vert_id);
    vert_id = 0;
  }
  if(frag_id) {
    glDeleteShader(frag_id);
    frag_id = 0;
  }
  if(prog) {
    glDeleteProgram(prog);
    prog = 0;
  }
}

void GPUImage_Base::deleteTextures(GLuint* ids) {
  if(getNumTextures()) {
    glDeleteTextures(getNumTextures(), ids);
  }
}
