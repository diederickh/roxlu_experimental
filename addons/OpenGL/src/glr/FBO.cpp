#include <assert.h>
#include <roxlu/core/Log.h>
#include <glr/FBO.h>
#include <glr/Texture.h>

namespace gl {

  FBO::FBO() 
    :fbo(0)
    ,depth_rbo(0)
    ,width(0)
    ,height(0)
  {
  }

  FBO::~FBO() {
    RX_ERROR("NEED TO DELETE THE FBO + TEXTURES");
    width = 0;
    height = 0;
    fbo = 0;
    depth_rbo = 0;
  }

  bool FBO::setup(int w, int h) {
    assert(w && h);

    width = w;
    height = h;

    glGenFramebuffers(1, &fbo);
    
    return true;
    
  }

  bool FBO::addDepthBuffer(GLenum internalFormat) {
    assert(depth_rbo == 0);
    assert(width && height);

    glGenRenderbuffers(1, &depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);

    return framebufferRenderbuffer(GL_DEPTH_ATTACHMENT, depth_rbo);
    
  }

  bool FBO::addTexture(GLenum attachment) {
    assert(width && height);
    assert(fbo);

    Texture* tex = new Texture();
    if(!tex->setPixels(NULL, width, height, GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE)) {
      RX_ERROR(ERR_GL_FBO_TEXTURE);
      return false;
    }

    if(!framebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex->getID(), 0)) {
      delete tex;
      return false;
    }

    texture_attachments[attachment] = tex;
    textures.push_back(tex);

    return true;
  }

  Texture* FBO::getTexture(GLenum attachment) {
#if !defined(NDEBUG)
    std::map<GLenum, Texture*>::iterator it = texture_attachments.find(attachment);
    if(it == texture_attachments.end()) {
      RX_ERROR(ERR_GL_TEX_NOT_ATTACHED);
      return NULL;
    }
#endif    

    return texture_attachments[attachment];
  }


  bool FBO::framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint tex, GLint level) {
    assert(width && height);
    assert(fbo);

    std::map<GLenum, Texture*>::iterator it = texture_attachments.find(attachment);
    if(it != texture_attachments.end()) {
      RX_ERROR(ERR_GL_TEX_ALREADY_ATTACHED);
      return false;
    }
    
    bind();
    glFramebufferTexture2D(target, attachment, textarget,  tex, level);
    unbind();

    color_attachments.push_back(attachment);

    return true;
  }

  bool FBO::framebufferRenderbuffer(GLenum attachment, GLuint rbo) {  
    assert(width && height && fbo);

    std::map<GLenum, GLuint>::iterator it = rbo_attachments.find(attachment);
    if(it != rbo_attachments.end()) {
      RX_ERROR(ERR_GL_FBO_ALREADY_ATTACHED);
      return false;
    }

    bind();

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
    

    unbind();

    rbo_attachments[attachment] = rbo;
    return true;
  }

  void FBO::bind(GLenum target) {
    assert(width && height && fbo);

    glBindFramebuffer(target, fbo);
  }
 
  void FBO::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void FBO::clear() {
    clear(color_attachments.size(), &color_attachments[0]);

#if 0   
    glDrawBuffers(n, bufs);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);;

    if(color_attachments.size()) {
      glDrawBuffers(color_attachments.size(), &color_attachments[0]);

      if(depth_rbo) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }
      else {
         glClear(GL_COLOR_BUFFER_BIT);
      }

    }
    else if(depth_rbo) {
      glClear(GL_COLOR_BUFFER_BIT);
    }
    else {
      RX_ERROR(ERR_GL_FBO_CLEAR);
    }
#endif

  }

  void FBO::clear(GLsizei n, const GLenum* bufs) {
    glDrawBuffers(n, bufs);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);;

  }

  void FBO::drawBuffers(std::vector<GLenum>& bufs) {
    glDrawBuffers(bufs.size(), &bufs[0]);
  }

  void FBO::drawBuffer(GLenum buf) {
    glDrawBuffer(buf);
  }


} // gl
