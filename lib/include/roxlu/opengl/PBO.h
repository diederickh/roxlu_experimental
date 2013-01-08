#ifndef ROXLU_PBOH
#define ROXLU_PBOH

/*
#include "OpenGL.h"
#include "Error.h"
#include "Constants.h"
*/

#include <roxlu/opengl/OpenGLInit.h>
#include <roxlu/opengl/Error.h>
#include <roxlu/core/Constants.h>

// read: http://www.opengl.org/registry/specs/ARB/pixel_buffer_object.txt
// read: http://developer.apple.com/library/mac/#documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/opengl_texturedata/opengl_texturedata.html#//apple_ref/doc/uid/TP40001987-CH407-SW1
class PBO {
public:
#if ROXLU_GL_MODE == ROXLU_GL_STRICT
	PBO(GLuint nTarget = 0);
#else 
	PBO(GLuint nTarget = GL_PIXEL_UNPACK_BUFFER);
#endif	
	PBO& setup(int nWidth, int nHeight, GLenum nColorType = GL_RGBA);
	PBO& setPixels(unsigned char* pRGBA);
	PBO& bind(); // for reading from pbo
	PBO& unbind();
	
	// GL_PIXEL_PACK_BUFFER: for reading from a PBO
	// GL_PIXEL_UNPACK_BUFFER: for drawing into a PBO using i.e. glTexImage2D
	GLuint target; 
	GLuint pbo_id;
	GLuint texture_id;
	int buffer_size;
	int width,height;
	GLenum color_type; // GL_RGBA or GL_RGB
};

/*
Example from bottom http://www.opengl.org/registry/specs/ARB/pixel_buffer_object.txt

Streaming textures using pixel buffer objects:

        const int texWidth = 256;
        const int texHeight = 256;
        const int texsize = texWidth * texHeight * 4;
        void *pboMemory, *texData;

        // Define texture level zero (without an image); notice the
        // explicit bind to the zero pixel unpack buffer object so that
        // pass NULL for the image data leaves the texture image
        // unspecified.
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight, 0,
                     GL_BGRA, GL_UNSIGNED_BYTE, NULL);

        // Create and bind texture image buffer object
        glGenBuffers(1, &texBuffer);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, texBuffer);

        // Setup texture environment
        ...

        texData = getNextImage();

        while (texData) {

            // Reset the contents of the texSize-sized buffer object
            glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, texSize, NULL,
                         GL_STREAM_DRAW);

            // Map the texture image buffer (the contents of which
            // are undefined due to the previous glBufferData)
            pboMemory = glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,
                                    GL_WRITE_ONLY);

            // Modify (sub-)buffer data
            memcpy(pboMemory, texData, texsize);

            // Unmap the texture image buffer
            glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);

            // Update (sub-)teximage from texture image buffer
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight,
                            GL_BGRA, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));

            // Draw textured geometry
            glBegin(GL_QUADS);
            ...
            glEnd();

            texData = getNextImage();
        }

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

//-------------- glYoda on #openGL
...

glBindTexture( GL_TEXTURE_2D,              TID );
glBindBuffer ( GL_PIXEL_UNPACK_BUFFER_ARB, PBO );

...

// Diederick: WHEN DOING SUCCESSIVE UPDATE THE CALL TO glTexImage2D IS THE ONLY ONE WHICH CAN BE OMITTED.
glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL ); 

glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, srcBytes, NULL, GL_STREAM_DRAW_ARB );

GLubyte *srcPtr = ... ;

memcpy( glMapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB ), srcPtr, srcBytes );

glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB );

glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, OFFSET( ... ) );

...



//+++++++++++++++++++++++++++++++++++++++++++ other code: mac specific
// See the GL_APPLE_client_storage, GL_APPLE_texture_range specifications (i.e. http://www.opengl.org/registry/specs/APPLE/client_storage.txt, http://www.opengl.org/registry/specs/APPLE/texture_range.txt) for more information.

...

// SETUP
{
    ...

    // Bind our resource IDs.

    glBindTexture( GL_TEXTURE_2D, TID );

    // Grab a pointer to our (retained) source buffer.

    GLubyte *srcPtr = ... ;

    // Tell the server that we will retain our source buffer.

    glPixelStorei( GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE );	

    // Allocate space on the server side for this upload.

    glTextureRangeAPPLE( GL_TEXTURE_2D, srcBytes, srcPtr );

    // Tell the server that we want CACHED storage. This would be the recommended choice for most use cases.

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_CACHED_APPLE );

    // Submit our pixel data.

    glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, srcPtr );

    ...
}

...

// RENDER
{
    ...

    // Bind our resource IDs.

    glBindTexture( GL_TEXTURE_2D, TID );

    // Grab a pointer to our (retained) source buffer.

    GLubyte *srcPtr = ... ;

    // Update our pixel data.

    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, srcPtr );

    ...
}



*/

#endif