#ifndef ROXLU_OPENGLINCLUDESH
#define ROXLU_OPENGLINCLUDESH

#define ROXLU_OPENGLES			1
#define ROXLU_OPENFRAMEWORKS		2
#define ROXLU_COCOA				3

#ifdef _WIN32
	#define ROXLU_GL_VARIANT 	ROXLU_OPENFRAMEWORKS
#else
//	#define ROXLU_GL_VARIANT 	ROXLU_COCOA
#endif


#define ROXLU_GL_VARIANT 	ROXLU_COCOA
//#define ROXLU_GL_VARIANT 	ROXLU_OPENFRAMEWORKS
//#define ROXLU_GL_VARIANT 		ROXLU_OPENGLES
#define ROXLU_GL_MODE		ROXLU_GL_STRICT  // modern opengl, no immediate mode etc.. (Must compile with OpenGL ES 2.x)


#if ROXLU_GL_VARIANT == ROXLU_OPENGLES
	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>
#elif ROXLU_GL_VARIANT == ROXLU_OPENFRAMEWORKS
	#include "ofMain.h"
#elif ROXLU_GL_VARIANT == ROXLU_COCOA
	#include <OpenGL/gl.h>
	
#endif

#endif