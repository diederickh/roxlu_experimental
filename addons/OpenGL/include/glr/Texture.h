#ifndef ROXLU_OPENGL_TEXTURE_H
#define ROXLU_OPENGL_TEXTURE_H

#include <assert.h>
#include <roxlu/Roxlu.h>
#include <image/Image.h>
#include <glr/Drawer.h>

#define ERR_GL_CANNOT_LOAD_FILE "Cannot load thefile: `%s`"
//#define ERR_GL_UNSUPPORTED_IMAGE "The filetype `%s` is not supported"
#define ERR_GL_FORMAT_NOT_SUPPORTED "Image format is not supported: `%s`"
#define ERR_GL_INVALID_SIZE "The size of the image is invalid: %d x %d"
#define ERR_GL_CANNOT_CREATE_TEX "Cannot create a texture"
#define ERR_GL_DIFFERENT_SIZE "The texture was already created but the given size is different then the allocated size"
#define ERR_GL_UNSUPPORTED_TYPE "The `type` is not supported" 
#define ERR_GL_INVALID_TEX_ID "The texture ID is invalid. Did you load an image yet? or called setPixels()? "

namespace gl {

  class Texture {
  public:
    Texture();
    ~Texture();

    bool load(std::string filepath, bool datapath = false);                   /* load an image as texture */
    
    bool setPixels(Image& img);                                               /* set the pixels from this image */

    bool setPixels(unsigned char* pixels, 
                   unsigned int w,
                   unsigned int h, 
                   GLenum target, 
                   GLenum internalFormat, 
                   GLenum format, 
                   GLenum type);

    
    void draw(float x, float y, float w = 0, float h = 0);                     /* draw the texture at these coordinates with the given width and height */
    void draw(const float* mm);                                                /* draw the texture using the given model matrix, make sure to set the width and height too */
    unsigned int getWidth();                                                   /* returns the height as found when loading the image, or set when calling setPixels() */
    unsigned int getHeight();                                                  /* returns the width as found when loading the image, or set when calling setPixels() */
    GLuint getID();                                                            /* returns the texture id */
    void bind();
    void unbind();
    bool isAllocated();                                                        /* returns true when the texture is allocated and ready to be used */

    void print();
    std::string formatToString(GLenum f);
    std::string typeToString(GLenum t);
    std::string targetToString(GLenum t);

  private:
    bool is_allocated;                                                          /* indicates if the texture was allocated */
    GLuint id;                                                                  /* the texture ID */
    GLenum internal_format;                                                     /* e.g. GL_RGBA, GL_RGB, etc.. internal format of the pixels */
    GLenum format;                                                              /* e.g. GL_RGBA, GL_RGBA; the format of the pixel data given to setPixels(), or automatically detected when calling load() */
    GLenum target;                                                              /* e.g. GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE */
    GLenum type;                                                                /* e.g. GL_UNSIGNED_BYTE */
    unsigned int width;
    unsigned int height;
  };


  inline unsigned int Texture::getWidth() {
    return width;
  }

  inline unsigned int Texture::getHeight() {
    return height;
  }

  inline GLuint Texture::getID() {
    return id;
  }

  inline void Texture::bind() {
    if(!id) {
      RX_ERROR(ERR_GL_INVALID_TEX_ID);
      return;
    }
    glBindTexture(target, id);
  }

  inline void Texture::unbind() {
    if(!id) {
      RX_ERROR(ERR_GL_INVALID_TEX_ID);
      return;
    }
    glBindTexture(target, 0);
  }

  inline bool Texture::isAllocated() {
    return is_allocated;
  }

  inline void Texture::draw(float x, float y, float w, float h) {
    assert(glr_context);
    glr_context->drawTexture(*this, x, y, w, h);
  }

  inline void Texture::draw(const float* mm) {
    assert(glr_context);
    glr_context->drawTexture(*this, mm);
  }

} // gl
#endif

