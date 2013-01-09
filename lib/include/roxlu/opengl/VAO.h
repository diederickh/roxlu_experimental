#ifndef ROXLU_VAOH
#define ROXLU_VAOH

#include <roxlu/opengl/OpenGLInit.h>
#include <roxlu/opengl/Error.h>
#include <roxlu/core/Constants.h>

namespace roxlu {

  class VAO {
  public:	
    VAO();
    void create();
    void bind();
    static void unbind();
    int getID();
  private:
    GLuint vao_id;
  };

  inline int VAO::getID() {
    return vao_id;
  }

} // roxlu
#endif
