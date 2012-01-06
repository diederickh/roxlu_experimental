#ifndef ROXLU_OPENGLINCLUDESH
#define ROXLU_OPENGLINCLUDESH

#define ROXLU_OPENGLES	1
#define ROXLU_OPENGL	2

#define ROXLU_OPENGL_VARIANT 	ROXLU_OPENGL


#if ROXLU_OPENGL_VARIANT == ROXLU_OPENGLES
	#include <OpenGLES/ES2/gl.h>
	//#include <OpenGLES/ES1/gl.h>
#elif ROXLU_OPENGL_VARIANT == ROXLU_OPENGL
	//#ifdef OF_VERSION
	//	#include "ofMain.h"
	//#else
		#include <OpenGL/gl.h>
	//#endif
	
#endif



#endif