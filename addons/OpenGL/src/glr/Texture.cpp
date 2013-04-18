#include <glr/Texture.h>

namespace gl {

  Texture::Texture() 
    :id(0)
    ,width(0)
    ,height(0)
    ,is_allocated(false)
  {
  }

  Texture::~Texture() {
    RX_ERROR("@TODO delete texture");
    id = 0;
    width = 0;
    height = 0;
    is_allocated = false;
  }

  bool Texture::load(std::string filepath, bool datapath) {
    Image img;
    if(!img.load(filepath, datapath)) {
      RX_ERROR(ERR_GL_CANNOT_LOAD_FILE, filepath.c_str());
      return false;
    }
  
    return setPixels(img);
  }

  bool Texture::setPixels(Image& img) {

    width = img.getWidth();
    height = img.getHeight();
    type = GL_UNSIGNED_BYTE;
    target = GL_TEXTURE_2D;
    
    if(img.getPixelFormat() == RX_FMT_RGB24) {
      internal_format = GL_RGB;
      format = GL_RGB;
    }
    else if(img.getPixelFormat() == RX_FMT_RGBA32) {
      internal_format = GL_RGBA;
      format = GL_RGBA;
    }
    else {
      RX_ERROR(ERR_GL_FORMAT_NOT_SUPPORTED);
      return false;
    }
    
    if(!setPixels(img.getPixels(), width, height, target, internal_format, format, type)) {
      return false;
    }

    return true;
  }

  bool Texture::setPixels(unsigned char* pixels, 
                          unsigned int w, 
                          unsigned int h, 
                          GLenum target, 
                          GLenum internalFormat, 
                          GLenum format,
                          GLenum type)
  {

    if(type != GL_UNSIGNED_BYTE) {
      RX_ERROR(ERR_GL_UNSUPPORTED_TYPE);
      return false;
    }

    if(!id) {
      glGenTextures(1, &id);

      if(!id) {
        RX_ERROR(ERR_GL_CANNOT_CREATE_TEX);
        return false;
      }
    }

    if(is_allocated && ((w != width) || (h != height)) ) {
      RX_ERROR(ERR_GL_DIFFERENT_SIZE);
      return false;
    }
  
    this->target = target;
    this->internal_format = internalFormat;
    this->format = format;
    this->width = w;
    this->height = h;
    this->type = type;

    glBindTexture(target, id);

    if(!is_allocated) {
      glTexImage2D(target, 0, internal_format, width, height, 0, format, type, pixels);
      glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      is_allocated = true;
    }
    else {
      glTexSubImage2D(target, 0, 0, 0, width, height, format, type, pixels);
    }
    
    return true;
  }

  void Texture::print() {
    RX_VERBOSE("Texture.target: %s", targetToString(target).c_str());
    RX_VERBOSE("Texture.internal_format: %s", formatToString(internal_format).c_str());
    RX_VERBOSE("Texture.format: %s", formatToString(format).c_str());
    RX_VERBOSE("Texture.type: %s", typeToString(type).c_str());
    RX_VERBOSE("Texture.width: %d", width);
    RX_VERBOSE("Texture.height: %d", height);
  }
  
  std::string Texture::targetToString(GLenum t) {
    if(t == GL_TEXTURE_2D) {
      return "GL_TEXTURE_2D";
    }
    else {
      return "UNKNOWN";
    }
  }

  std::string Texture::formatToString(GLenum f) {
    switch(f) {
      case GL_RGB: return "GL_RGB";
      case GL_RGBA: return "GL_RGBA";
      case GL_RED: return "GL_RED";
      default: return "UNKNOWN";
    }; 
  }


  std::string Texture::typeToString(GLenum t) {
    if(t == GL_UNSIGNED_BYTE) {
      return "GL_UNSIGNED_BYTE"; 
    }
    else {
      return "UNKNOWN";
    }
  }

} // gl
