#ifndef ROXLU_OPENGL_COMPOSITOR_H
#define ROXLU_OPENGL_COMPOSITOR_H

#include <assert.h>
#include <vector>
#include <roxlu/opengl/GL.h>
#include <experimental/Types.h>
#include <experimental/Filter.h>
#include <experimental/filters/Demo.h>
#include <experimental/filters/VerticalBlur.h>
#include <experimental/filters/HorizontalBlur.h>
#include <experimental/filters/Luminance.h>
#include <glr/Mesh.h>
#include <glr/FBO.h>

#define ERR_RPASS_FBO_SIZE "The FBO size is invalid"
#define ERR_RPASS_ADD_FILTERS "You can only create the render pass after you've added filters!"
#define ERR_RPASS_FBO_SETUP "Cannot setup the FBO"
#define ERR_RPASS_FULLSCREEN_SHADER "Cannot create the fullscreen shader"

namespace gl {

  static const char* COMP_FULLSCREEN_VS = GLSL(120,
                                               attribute vec4 a_pos;
                                               attribute vec2 a_tex;
                                               varying vec2 v_tex;
                                               void main() {
                                                 gl_Position = a_pos;
                                                 v_tex = a_tex;
                                               }
  );

  static const char* COMP_FULLSCREEN_FS = GLSL(120,
                                               uniform sampler2D u_tex;
                                               varying vec2 v_tex;
                                               void main() {
                                                 vec4 col = texture2D(u_tex, v_tex);
                                                 gl_FragColor =  col;
                                               }
  );

  void print_gl_enum(GLenum e);                                 /* just prints the GL_COLOR_ATTACHMENT# value, handy while debugging  */

  class Compositor {
  public:
    Compositor();
    ~Compositor();

    void setup(int w, int h);
    void addFilter(Filter* filter, int input, int output);      /* first add all the filters you want, then call create */
    bool create();                                              /* create the fbo for the compositor and setup all filters */

    void begin();                                               /* call begin(), then draw your scene, then call end() */
    void end();                                                 /* after drawing your scene call end() */
    void draw(int num);                                         /* draw(), draws the given attachment point */

    void createAttachment(int num);                             /* you can use this if you want to create another attachment point. This can be handy if you want to create the filter graph with some more advanced features, like bloom */
    bool getAttachment(int num, Attachment& result);            /* this finds the attachment point by: `GL_COLOR_ATTACHMENT0 + num` and returns true if found, else false */

  private:
    bool setupShaders();                                        /* sets up the internally used fullscreen shader */
    bool setupBuffers();                                        /* sets up the fullscreen VBO */
    bool setupFBO();                                            /* creates the FBO + all the nessary textures and attachments */
    void createDefaultAttachments();                            /* creates the two default color attachments that we use for simple shaders: GL_COLOR_ATTACHMENT0 and GL_COLOR_ATTACHMENT1. By default we render the scene into GL_COLOR_ATTACHMENT0, see begin() */
    GLuint createTexture();                                     /* just an helper to create a 2D texture */

  public:
    bool initialized;                                           /* is set to true when the default attachments have been creatd */
    gl::Mesh<VertexPT> fullscreen_quad;                         /* the fullscreen mesh */
    gl::Shader fullscreen_shader;                               /* shader for fullscreen rendering */
    std::vector<Attachment> attachments;                        /* all the created attachments */
    std::vector<GLuint> textures;                               /* reference to all textures */
    std::vector<GLenum> buffers;                                /* all attchment points (GL_COLOR_ATTACHMENT#) */
    GLint fullscreen_u_tex;                                     /* uniform of the fullscreen texture */
    GLuint fbo;                                                 /* the fbo */
    GLuint depth;                                               /* depth buffer handle */
    int fbo_w;                                                  /* width of the fbo */
    int fbo_h;                                                  /* height of the fbo */
    std::vector<Filter*> filters;                               /* the filters which we apply */
    
  };

} // namespace gl

#endif
