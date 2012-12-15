#ifndef ROXLU_OPENGLINCLUDESH
#define ROXLU_OPENGLINCLUDESH

#include <roxlu/core/platform/Platform.h>


#if ROXLU_GL_WRAPPER == ROXLU_GLFW
 #if ROXLU_PLATFORM == ROXLU_LINUX
   #define GL_GLEXT_PROTOTYPES
   #include <GL/gl.h>
   #include <GL/glext.h>
 #elif ROXLU_PLATFORM == ROXLU_APPLE
  #include <OpenGL/gl.h>

// #ifndef glGenVertexArrays 
    #define glGenVertexArrays glGenVertexArraysAPPLE
//#endif
//#ifndef glBindVertexArrayAPPLE
    #define glBindVertexArray glBindVertexArrayAPPLE
//  #endif 


 #endif
#elif ROXLU_GL_WRAPPER == ROXLU_OPENFRAMEWORKS
#include "ofMain.h"
#elif ROXLU_GL_WRAPPER == ROXLU_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#elif ROXLU_GL_WRAPPER == ROXLU_CINDER
#elif ROXLU_GL_WRAPPER == ROXLU_COCOA
#include <OpenGL/gl.h>
#endif

#endif
