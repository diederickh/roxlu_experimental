#ifndef ROXLU_IMAGE_IMAGE_H
#define ROXLU_IMAGE_IMAGE_H

#include <assert.h>
#include <string>
#include <roxlu/core/Constants.h>
#include <roxlu/core/Log.h>

#include <image/PNG.h>
#include <image/JPG.h>
#include <image/TGA.h>

#define ERR_IMG_UNSUPPORTED_FORMAT "The given image pixel format is not supported"
#define ERR_IMG_UNSUPPORTED_EXT "The given image type is not supported (jpg, png, tga)"
#define ERR_IMG_CANNOT_ALLOCATE "Cannot allocate the pixel buffer (out of memory?)"

// ------------------------------------------------

class Image {
 public:
  Image();
  Image(const Image& other);
  Image& operator=(const Image& other);
  Image& clone(const Image& other);
  ~Image();
  void deallocate();                                                    /* deallocates allocated memory and resets all members to default */

  bool load(std::string filename, bool datapath = false);
  bool save(std::string filename, bool datapath = false);

  unsigned char* getPixels();
  unsigned int getWidth();
  unsigned int getHeight();
  unsigned int getPixelFormat(); 
  unsigned int getBitsPerPixel();
  unsigned int getStride();

  void setWidth(int w);
  void setHeight(int h);
  void setStride(unsigned int stride);
  bool setPixelFormat(unsigned int format);

  bool copyPixels(unsigned char* pix, unsigned int w, unsigned int h, unsigned int fmt);

  unsigned char& operator[](unsigned int dx);

 public:
  unsigned char* pixels;
  unsigned int stride;
  unsigned int width;
  unsigned int height;
  unsigned int bits_per_pixel;
  unsigned int fmt;
};

bool rx_load_png(Image& img, std::string filename, bool datapath = false);
bool rx_load_jpg(Image& img, std::string filename, bool datapath = false);
bool rx_load_tga(Image& img, std::string filename, bool datapath = false);

bool rx_save_png(Image& img, std::string filename, bool datapath = false);

// ------------------------------------------------

inline unsigned char* Image::getPixels() {
  return pixels;
}

inline unsigned int Image::getWidth() {
  return width;
}

inline unsigned int Image::getHeight() {
  return height;
}

inline unsigned int Image::getStride() {
  return stride;
}

inline unsigned int Image::getPixelFormat() {
  return fmt;
}

inline void Image::setWidth(int w) {
  width = w;
}

inline void Image::setHeight(int h) {
  height = h;
}

inline bool Image::setPixelFormat(unsigned int format) {
  fmt = format;

  switch(fmt) {

    case RX_FMT_GRAY8: {
      bits_per_pixel = 8;
      break;
    }

    case RX_FMT_RGBA32: {
      bits_per_pixel = 32;
      break;
    }

    case RX_FMT_BGR24:
    case RX_FMT_RGB24: {
      bits_per_pixel = 24;
      break;
    };

    case RX_FMT_NONE:
    default: {
      RX_ERROR(ERR_IMG_UNSUPPORTED_FORMAT);
      return false;
    }
  }

  return true;
}

inline void Image::setStride(unsigned int imageStride) {
  stride = imageStride;
}

inline unsigned char& Image::operator[](unsigned int dx) {
  return pixels[dx];
}
#endif
