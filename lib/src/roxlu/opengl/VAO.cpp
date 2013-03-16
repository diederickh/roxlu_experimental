#include <roxlu/opengl/GL.h>

#ifdef ROXLU_WITH_OPENGL

#include <roxlu/opengl/VAO.h>
#include <roxlu/core/platform/Platform.h>

namespace roxlu {
  VAO::VAO()
    :vao_id(-1)
  {
  }

  void VAO::create() {
    glGenVertexArrays(1, &vao_id); eglGetError();
  }
	
  void VAO::bind() {
    if(vao_id == -1) {
      create();
    }
    glBindVertexArray(vao_id); eglGetError();
  }
	
  void VAO::unbind() {
    glBindVertexArray(0); eglGetError();
  }

} // roxlu

#endif // ROXLU_WITH_OPENGL
