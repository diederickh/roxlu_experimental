#ifndef ROXLU_OPENGL_FBO_H
#define ROXLU_OPENGL_FBO_H

#include <roxlu/opengl/GL.h>
#include <map>
#include <vector>

#define ERR_GL_FBO_ALREADY_ATTACHED "The given attachment point is already in use."
#define ERR_GL_TEX_ALREADY_ATTACHED "The given attachment for the texture is already in use"
#define ERR_GL_TEX_NOT_ATTACHED "Couldn't find the texture for the given attachment"
#define ERR_GL_FBO_TEXTURE "Error while creating the texture"
#define ERR_GL_FBO_CLEAR "Cannot clear because no color or depth attachment was set"

namespace gl {

  class Texture;

class FBO {
 public:
  FBO();
  ~FBO();
  bool setup(int w, int h);

  void clear();
  void clear(GLsizei n, const GLenum* bufs);

  bool addTexture(GLenum attachment);
  bool addDepthBuffer(GLenum internalForamt = GL_DEPTH_COMPONENT16);
  
  Texture* getTexture(GLenum attachment);
  size_t getNumTextures();
  std::vector<Texture*>& getTextures();

  bool framebufferRenderbuffer(GLenum attachment, GLuint rbo);
  bool framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint tex, GLint level = 0);

  void bind(GLenum target = GL_FRAMEBUFFER);
  void unbind();
  
  void drawBuffer(GLenum buf);
  void drawBuffers(std::vector<GLenum>& bufs);    /* set the given buffers as active draw buffers */
  std::vector<GLenum>& getColorAttachments();     /* get a vector with all the color attachments that have been created */


 public:
  int width;                                       /* width of the FBO */
  int height;                                      /* height of the FBO */
  std::map<GLenum, GLuint> rbo_attachments;        /* rbo attachments onto this FBO */
  std::map<GLenum, Texture*> texture_attachments;  /* added textures and their attachment points */
  std::vector<GLenum> color_attachments;           /* added textures (used as a helper to clear the complete fbo)  */
  std::vector<Texture*> textures;                  /* get textures */

  GLuint fbo;                                    /* the fbo id*/
  GLuint depth_rbo;                              /* depth buffer */

};

 inline size_t FBO::getNumTextures() {
    return texture_attachments.size();
 } 
 
 inline std::vector<GLenum>& FBO::getColorAttachments() {
   return color_attachments;
 }

 inline std::vector<Texture*>& FBO::getTextures() {
   return textures;
 }

} // gl
#endif
