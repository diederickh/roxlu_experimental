#ifndef ROXLU_OPENGLINCLUDESH
#define ROXLU_OPENGLINCLUDESH

#include <roxlu/core/platform/Platform.h>

#define GLSL(version, shader)  "#version " #version "\n" #shader

#if defined(ROXLU_GL_CORE3)
#  include <GLXW/glxw.h>
#  include <GL/glfw3.h>
#else 
#  include <GL/glew.h>
#  include <GL/glfw3.h>
#endif



#endif
