/*

  Roxlu OpenGL 
  ------------
  Because the roxlu lib uses OpenGL a lot we need to get access to the 
  function declarations. Because each framework (GLFW, openFrameworks,
  Cinder, etc..) has their own OpenGL context code, we need to have a
  flag which tells use what files we need to include to get access to 
  the GL funtions.

  There is one special/silly case which I haven't found a good 
  solution for. This is glGenVertexArrays() in combination with 
  glew. Because Glew has some defines for glGenVertexArrays and 
  other functions, compiling some addons and roxlu into a static 
  library works fine but when actually linked with this lib 
  the function definitions are never found and the application 
  would crash whenever you call one of these. 
  One solution I found to work, is by defining a custom 
  glGenVertexArrays/glBindVetexArray (see below) for Apple. 
  

 */

#ifndef ROXLU_OPENGL_GL_H
#define ROXLU_OPENGL_GL_H

#define GLSL(version, shader)  "#version " #version "\n" #shader

// INCLUDES
#if defined(ROXLU_WITH_GLFW3_AND_GLEW)
#  define ROXLU_WITH_OPENGL
#  define GLEW_STATIC
#  include <GL/glew.h>
#  include <GL/glfw3.h>

// TMP - GET NATIVE - TESTING WITH VDPAU ON LINUX, SEE 038
#if defined(__linux)
#  define GLFW_EXPOSE_NATIVE_X11
#  define GLFW_EXPOSE_NATIVE_GLX
#  include <GL/glfw3native.h>
#endif
// END - GET NATIVE

#elif defined(ROXLU_WITH_GLFW3_AND_GLXW)
#  define ROXLU_GL_CORE3
#  define ROXLU_WITH_OPENGL
#  if !defined(__linux)
#    include <OpenGL/gl3.h>
#    include <OpenGL/gl3ext.h>
#  endif
#  if defined(__linux)
#    include <GLXW/glxw.h>       // @todo - test if this is necessary on linux, gives errors on mac
#  endif
#define GLFW_INCLUDE_NONE
#  include <GL/glfw3.h>
#elif defined(ROXLU_WITH_OPENFRAMEWORKS)
#  define ROXLU_WITH_OPENGL
#  include "ofMain.h"
#elif defined(ROXLU_WITH_OSX_NATIVE)
#  define ROXLU_WITH_OPENGL
#  include <OpenGL/gl.h>
#endif

// FIXES (NOT NEEDED FOR GL3)
/*
#if defined(__APPLE__)
#  if !defined(ROXLU_WITH_GLFW3_AND_GLXW)
#    undef glGenVertexArrays
#    define glGenVertexArrays glGenVertexArraysAPPLE
#    undef glBindVertexArray
#    define glBindVertexArray glBindVertexArrayAPPLE
#  endif
#endif
*/

#if defined(ROXLU_WITH_OSX_NATIVE)
//#  define GL_TEXTURE_RECTANGLE GL_TEXTURE_RECTANGLE_EXT  
#endif

#endif // ROXLU_OPENGL_GL_H
