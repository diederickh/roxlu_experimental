#include <glr/Texture.h>
#include <image/PNG.h>
#include <image/TGA.h>
#include <image/JPG.h>

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
    unsigned char* pixels = NULL;

    // LOAD THE IMAGE
    std::string ext = rx_get_file_ext(filepath);
    if(ext == "png") {
      PNG png;
      if(!png.load(filepath, datapath)) {
        RX_ERROR(ERR_GL_CANNOT_LOAD_FILE, filepath.c_str());
        return false;
      }
    
      if(png.getColorType() == PNG_COLOR_TYPE_RGB) {
        internal_format = GL_RGB;
        format = GL_RGB;
      }
      else if(png.getColorType() == PNG_COLOR_TYPE_RGBA) {
        internal_format = GL_RGBA;
        format = GL_RGBA;
      }
      else {
        RX_ERROR(ERR_GL_FORMAT_NOT_SUPPORTED, png.colorTypeToString(png.getColorType()).c_str());
        return false;
      }

      width = png.getWidth();
      height = png.getHeight();
      pixels = png.getPixels();
      type = GL_UNSIGNED_BYTE;
    }
    else if(ext == "jpg") {
      JPG jpg;
      if(!jpg.load(filepath, datapath)) {
        RX_ERROR(ERR_GL_CANNOT_LOAD_FILE, filepath.c_str());
        return false;
      }
      
      if(jpg.getNumChannels() == 4) {
        internal_format = GL_RGBA;
        format = GL_RGBA;
      }
      else if(jpg.getNumChannels() == 3) {
        internal_format = GL_RGB;
        format = GL_RGB;
      }
      else {
        RX_ERROR(ERR_GL_FORMAT_NOT_SUPPORTED, "UNKNOWN (JPG)");
        return false;
      }

      width = jpg.getWidth();
      height = jpg.getHeight();
      pixels = jpg.getPixels();
      type = GL_UNSIGNED_BYTE;
      
    }
    else if(ext == "tga") {
      TGA tga;
      if(!tga.load(filepath, datapath)) {
        RX_ERROR(ERR_GL_CANNOT_LOAD_FILE, filepath.c_str());
        return false;
      }
      
      if(tga.getNumChannels() == 4) {
        internal_format = GL_RGBA;
        format = GL_RGBA;
      }
      else if(tga.getNumChannels() == 3) {
        internal_format = GL_RGB;
        format = GL_RGB;
      }
      else {
        RX_ERROR(ERR_GL_FORMAT_NOT_SUPPORTED, "UNKNOWN (TGA)");
        return false;
      }

      width = tga.getWidth();
      height = tga.getHeight();
      pixels = tga.getPixels();
      type = GL_UNSIGNED_BYTE;
    }
    else {
      RX_ERROR(ERR_GL_UNSUPPORTED_IMAGE, ext.c_str());
      return false;
    }

    if(!width || !height) {
      RX_ERROR(ERR_GL_INVALID_SIZE, width, height);
      return false;
    }

    target = GL_TEXTURE_2D;
    
    if(!setPixels(pixels, width, height, GL_TEXTURE_2D, internal_format, format, type)) {
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
    if(f == GL_RGB) {
      return "GL_RGB";
    }
    else if(f == GL_RGBA) {
      return "GL_RGBA";
    }
    else {
      return "UNKNOWN";
    }
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
