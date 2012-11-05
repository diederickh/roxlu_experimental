#ifndef ROXLU_OPENGLINCLUDESH
#define ROXLU_OPENGLINCLUDESH

#include <roxlu/core/platform/Platform.h>

// @todo need to get a bit more into cross platform GL :)  

#if ROXLU_GL_WRAPPER == ROXLU_GLFW
 #if ROXLU_PLATFORM == ROXLU_LINUX
   #define GL_GLEXT_PROTOTYPES
   #include <GL/gl.h>
   #include <GL/glext.h>
 #elif ROXLU_PLATFORM == ROXLU_APPLE
   #include <OpenGL/gl.h>
 #endif

#elif ROXLU_GL_WRAPPER == ROXLU_OPENFRAMEWORKS
#include "ofMain.h"
#elif ROXLU_GL_WRAPPER == ROXLU_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif ROXLU_GL_WRAPPER == ROXLU_CINDER
#elif ROXLU_GL_WRAPPER == ROXLU_COCOA
#include <OpenGL/gl.h>
#else 
#error "No GL variant defined in OpenGLInit.h so we don't know what GL includes to use"
#endif




/* #define ROXLU_OPENGLES 1	 */
/* #define ROXLU_OPENFRAMEWORKS 2 */
/* #define ROXLU_COCOA 3 */

/* #ifdef _WIN32 */
/* #define ROXLU_GL_VARIANT ROXLU_OPENFRAMEWORKS */
/* #else */
/* //	#define ROXLU_GL_VARIANT ROXLU_COCOA */
/* #endif */

/* #ifndef ROXLU_GL_VARIANT */
/* #define ROXLU_GL_VARIANT ROXLU_COCOA */
/* //#define ROXLU_GL_VARIANT ROXLU_OPENFRAMEWORKS */
/* //#define ROXLU_GL_VARIANT ROXLU_OPENGLES */
/* #define ROXLU_GL_MODE ROXLU_GL_STRICT  // modern opengl, no immediate mode etc.. (Must compile with OpenGL ES 2.x) */
/* #endif */

/* #if ROXLU_GL_VARIANT == ROXLU_OPENGLES */
/* #include <OpenGLES/ES2/gl.h> */
/* #include <OpenGLES/ES2/glext.h> */
/* #elif ROXLU_GL_VARIANT == ROXLU_OPENFRAMEWORKS */
/* #include "ofMain.h" */
/* #elif ROXLU_GL_VARIANT == ROXLU_COCOA */
/* #include <OpenGL/gl.h> */
	
/* #endif */

#endif
