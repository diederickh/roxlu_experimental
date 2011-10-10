#ifndef ROXLU_ERRORH
#define ROXLU_ERRORH
#define DEBUG
#ifdef  DEBUG

#include <stdlib.h>
#include <assert.h>

#define eglGetError( )\
        {\
            for ( GLenum Error = glGetError( ); ( GL_NO_ERROR != Error ); Error = glGetError( ) )\
            {\
                switch ( Error )\
                {\
                    case GL_INVALID_ENUM:      printf( "\n%s\n\n", "GL_INVALID_ENUM"      ); assert( 0 ); break;\
                    case GL_INVALID_VALUE:     printf( "\n%s\n\n", "GL_INVALID_VALUE"     ); assert( 0 ); break;\
                    case GL_INVALID_OPERATION: printf( "\n%s\n\n", "GL_INVALID_OPERATION" ); assert( 0 ); break;\
                    case GL_STACK_OVERFLOW:    printf( "\n%s\n\n", "GL_STACK_OVERFLOW"    ); assert( 0 ); break;\
                    case GL_STACK_UNDERFLOW:   printf( "\n%s\n\n", "GL_STACK_UNDERFLOW"   ); assert( 0 ); break;\
                    case GL_OUT_OF_MEMORY:     printf( "\n%s\n\n", "GL_OUT_OF_MEMORY"     ); assert( 0 ); break;\
                    default:                                                                              break;\
                }\
            }\
        }

//#define eglGetError()
#define eglCheckFramebufferStatus( )\
        {\
            switch ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) )\
            {\
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"         ); assert( 0 ); break;\
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" ); assert( 0 ); break;\
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"        ); assert( 0 ); break;\
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"        ); assert( 0 ); break;\
                case GL_FRAMEBUFFER_UNSUPPORTED:                   printf( "\n%s\n\n", "GL_FRAMEBUFFER_UNSUPPORTED"                   ); assert( 0 ); break;\
                case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"        ); assert( 0 ); break;\
                case GL_FRAMEBUFFER_UNDEFINED:                     printf( "\n%s\n\n", "GL_FRAMEBUFFER_UNDEFINED"                     ); assert( 0 ); break;\
                default:                                                                                                                              break;\
            }\
        }

#define eglGetShaderInfoLog( Shader )\
        {\
            GLint   Status, Count;\
            GLchar *Error;\
            \
            glGetShaderiv( Shader, GL_COMPILE_STATUS, &Status );\
            \
            if ( !Status )\
            {\
                glGetShaderiv( Shader, GL_INFO_LOG_LENGTH, &Count );\
                \
                if ( Count > 0 )\
                {\
					Error = (GLchar *)malloc(Count);\
					glGetShaderInfoLog( Shader, Count, NULL, Error );\
                    \
                    printf( "%s\n", Error );\
                    \
                    free( Error );\
                    \
                    assert( 0 );\
                }\
            }\
        }

#else

#define eglGetError( )

#define eglCheckFramebufferStatus( )

#define eglGetShaderInfoLog( Shader )

#endif /*   DEBUG     */
#endif /* __ERROR_H__ */