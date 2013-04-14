#ifndef ROXLU_IMAGE_PNG_H
#define ROXLU_IMAGE_PNG_H

#include <stdio.h>
#include <string>
#include <png.h>

#define PNG_ERR_LOAD_NO_FILE "Cannot load file, does it exist: %s"
#define PNG_ERR_SAVE_NO_FILE "Cannot save file; no access maybe: %s"
#define PNG_ERR_CANNOT_READ "Cannot read from file."
#define PNG_ERR_SIG "PNG has wrong signature"
#define PNG_ERR_CREATE_READ_ST "PNG cannot create the read struct; out of mem"
#define PNG_ERR_CREATE_INFO_PTR "PNG cannot create info_ptr; out of mem"
#define PNG_ERR_ALREADY_LOADED "PNG is already loaded"
#define PNG_ERR_IMG_TOO_BIG "The image is to big to be stored in memory. we cant allocate mem for it"
#define PNG_ERR_CREATE_WRITE_ST "Error while setting up the write data to save as png"
#define PNG_ERR_UNSUPPORTED_FORMAT "The png format is not supported"
#define PNG_ERR_SIZE "Invalid size: w: %d, h: %d"
#define PNG_ERR_STRIDE "Invalid stride: %d"
#define PNG_ERR_PIXELS "Invalid pixels (probably not set)"
#define PNG_ERR_BITDEPTH "Invalid bit depth: %d"
#define PNG_V_WIDTH "png.width: %d"
#define PNG_V_HEIGHT "png.height: %d"
#define PNG_V_BITDEPTH "png.bit_depth: %d"
#define PNG_V_NCHANNELS "png.num_channels: %d"
#define PNG_V_COLORTYPE "png.color_type: %s"
#define PNG_V_NUMBYTES "png.num_bytes: %d"

typedef unsigned int png_uint_32;

class PNG {
public:
  PNG();
  PNG(const PNG& other);
  PNG& operator=(const PNG& other);
  ~PNG();

  bool load(std::string filename, bool datapath = false);
  bool save(std::string filename, bool datapath = false);

  bool setPixels(unsigned char* pix, png_uint_32 w, png_uint_32 h, png_uint_32 type);
  unsigned char* getPixels();
  png_uint_32 getNumChannels();
  png_uint_32 getColorType();
  png_uint_32 getWidth();
  png_uint_32 getHeight();

  void print();
  std::string colorTypeToString(png_uint_32 ct);

public:
  unsigned char* pixels; 
  unsigned int num_bytes;                                                  /* number of bytes in the pixels array */
  unsigned int stride;                                                     /* stride of the buffer, length in bytes of one row */
  png_uint_32 width;                                                       /* width of the image */
  png_uint_32 height;                                                      /* height of the image */
  png_uint_32 bit_depth;                                                   /* bit depth per color channels, so e.g. per R, per G, per B etc.. */
  png_uint_32 color_type;                                                  /* color type: RGB, RGBA, Luminance, Luminance alpha, palette, etc.. */
  png_uint_32 num_channels;                                                /* number of channels, e.g. 3 for RGB, 1 for grayscale */
};


inline std::string PNG::colorTypeToString(png_uint_32 ct) {
  switch(ct) {
    case PNG_COLOR_TYPE_GRAY: return "PNG_COLOR_TYPE_GRAY";
    case PNG_COLOR_TYPE_PALETTE: return "PNG_COLOR_TYPE_PALETTE";
    case PNG_COLOR_TYPE_RGB: return "PNG_COLOR_TYPE_RGB";
    case PNG_COLOR_TYPE_RGB_ALPHA: return "PNG_COLOR_TYPE_RGB_ALPHA";
    case PNG_COLOR_TYPE_GRAY_ALPHA: return "PNG_COLOR_TYPE_GRAY_ALPHA";
    default: return "COLOR_TYPE_UNKNOWN";
  }
}

inline unsigned char* PNG::getPixels() {
  return pixels;
}

inline png_uint_32 PNG::getNumChannels() {
  return num_channels;
}

inline png_uint_32 PNG::getColorType() {
  return color_type;
}

inline png_uint_32 PNG::getWidth() {
  return width;
}

inline png_uint_32 PNG::getHeight() {
  return height;
}

#endif
