#ifndef ROXLU_GL_ERRORH
#define ROXLU_GL_ERRORH

#if defined(ROXLU_WITH_OPENGL)
#  if defined(ROXLU_DEBUG)
#    include <stdlib.h>
#    include <assert.h>
#    define eglGetError( )                                              \
  {                                                                     \
    for ( GLenum Error = glGetError( ); ( GL_NO_ERROR != Error ); Error = glGetError( ) ) \
      {                                                                 \
        switch ( Error )                                                \
          {                                                             \
            case GL_INVALID_ENUM:      printf( "\n%s\n\n", "GL_INVALID_ENUM"      );    assert( 0 ); break; \
            case GL_INVALID_VALUE:     printf( "\n%s\n\n", "GL_INVALID_VALUE"     );    assert( 0 ); break; \
            case GL_INVALID_OPERATION: printf( "\n%s\n\n", "GL_INVALID_OPERATION" );    assert( 0 ); break; \
            case GL_OUT_OF_MEMORY:     printf( "\n%s\n\n", "GL_OUT_OF_MEMORY"     );    assert( 0 ); break; \
            default:                   printf("Unhandled glGetError(), use debugger");  assert( 0 ); break; \
          }                                                             \
      }                                                                 \
  }

#    define eglCheckFramebufferStatus( )                                                                                                                           \
        {                                                                                                                                                          \
          switch ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) )                                                                                                    \
            {                                                                                                                                                      \
              case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"         ); assert( 0 ); break;         \
              case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" ); assert( 0 ); break;         \
              case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"        ); assert( 0 ); break;         \
              case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"        ); assert( 0 ); break;         \
              case GL_FRAMEBUFFER_UNSUPPORTED:                   printf( "\n%s\n\n", "GL_FRAMEBUFFER_UNSUPPORTED"                   ); assert( 0 ); break;         \
              case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"        ); assert( 0 ); break;         \
              case GL_FRAMEBUFFER_UNDEFINED:                     printf( "\n%s\n\n", "GL_FRAMEBUFFER_UNDEFINED"                     ); assert( 0 ); break;         \
              default:                                                                                                                              break;         \
            }                                                                                                                                                      \
        }
   
#    define eglGetShaderInfoLog( Shader )                               \
  {                                                                     \
    GLint egl_status;                                                   \
    GLint egl_count;                                                    \
    GLchar *egl_error;                                                  \
                                                                        \
    glGetShaderiv( Shader, GL_COMPILE_STATUS, &egl_status );            \
                                                                        \
    if ( !egl_status )                                                  \
      {                                                                 \
        glGetShaderiv( Shader, GL_INFO_LOG_LENGTH, &egl_count );        \
                                                                        \
        if ( egl_count > 0 )                                            \
          {                                                             \
            egl_error = (GLchar *)malloc(egl_count);                    \
            glGetShaderInfoLog( Shader, egl_count, NULL, egl_error );   \
            printf( "%s\n", egl_error );                                \
            free( egl_error );                                          \
            assert( 0 );                                                \
          }                                                             \
      }                                                                 \
  }
   
   
#   define   eglGetShaderLinkLog(id)                                             \
   {                                                                             \
        GLint ret = 0;                                                           \
        glGetProgramiv(id, GL_LINK_STATUS, &ret);                                \
                                                                                 \
        if(ret == false) {                                                       \
           GLsizei chars_written = 0;                                            \
           GLint log_length = 0;                                                 \
           glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_length);                  \
           if(log_length > 0) {                                                  \
               GLchar* buffer = new GLchar[log_length];                          \
               glGetProgramInfoLog(id, 2048, &chars_written, buffer);            \
               printf("\n%s\n\n", buffer);                                       \
               delete[] buffer;                                                  \
           }                                                                     \
       }                                                                         \
    }
   
#  else
   
#     define eglGetError( )
#     define eglGetShaderLinkLog(id)                                      
#     define eglCheckFramebufferStatus( )
#     define eglGetShaderInfoLog( Shader )
   
#  endif /*   DEBUG     */
#endif /* ROXLU_WITH_OPENGL */

#endif /* __ERROR_H__ */




