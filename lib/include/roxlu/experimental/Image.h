#ifndef ROXLU_IMAGEH
#define ROXLU_IMAGEH

#include <string>
#include <roxlu/external/External.h>

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include <roxlu/external/stb_image_write.h>

#define STBI_HEADER_FILE_ONLY
#include <roxlu/external/stb_image.c>

namespace roxlu {
  struct Image {
    int width;
    int height;
    int bpp;
    unsigned char* pixels;
	
    Image();
    ~Image();
    bool load(const char* filepath);
    bool save(const char* filepath);
    void copyPixels(const unsigned char* src, int w, int h, int components);
    int getWidth();
    int getHeight();
    int getComponents();
    unsigned char* getPixels();
  };

  inline int Image::getComponents() {
    return bpp;
  }

  inline int Image::getWidth() {
    return width;
  }

  inline int Image::getHeight() {
    return height;
  }

  inline unsigned char* Image::getPixels() {
    return pixels;
  }

  inline void Image::copyPixels(const unsigned char* src, int w, int h, int components) {
    if(pixels != NULL) {
      delete[] pixels;
      pixels = NULL;
    }
    pixels = new unsigned char[w * h * components];
    memcpy(pixels, src, w * h * components);
    width = w;
    height = h;
    bpp = components;
  }

} // roxlu

#endif
