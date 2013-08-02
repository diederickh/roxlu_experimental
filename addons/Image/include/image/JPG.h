#ifndef ROXLU_IMAGE_JPG_H
#define ROXLU_IMAGE_JPG_H

#include <string>
#include <stdio.h>

#if defined(_WIN32)
#  define XMD_H
#  include <jpeglib.h>
#  undef XMD_H
#else 
#  include <jpeglib.h>
#endif

#define ERR_JPG_FILE_NOT_OPENED "Cannot open the jpg file: `%s`"
#define ERR_JPG_CANNOT_ALLOC "Cannot allocate buffer space for the jpg pixels "

class JPG {
 public:
  JPG();
  JPG(const JPG& other);
  JPG& clone(const JPG& other);
  JPG& operator=(const JPG& other);

  ~JPG();
  void clear();

  bool load(std::string filename, bool datapath = false);
  bool load(unsigned char* compressed, size_t nbytes);
  bool save(std::string filename, bool datapath = false);
  bool setPixels(unsigned char* pix, int w, int h, J_COLOR_SPACE type = JCS_RGB);    /* JCS_RGB, JCS_GRAYSCALE, JCS_YCbCr, JCS_CMYK, JCS_Yckk */
  
  unsigned char* getPixels();
  unsigned int getWidth();
  unsigned int getHeight();
  unsigned int getNumChannels();
  unsigned int getNumBytes();
  unsigned int getBitDepth();
    
  void print();

 public:

  unsigned char* pixels;
  unsigned int num_bytes;
  unsigned int stride;
  unsigned int width;
  unsigned int height;
  unsigned int bit_depth;
  unsigned int num_channels;
  unsigned int quality;                                                              /* compression quality: 0 - 100, default is 80 */

  /* libjpg types */
  J_COLOR_SPACE color_space;
  J_DCT_METHOD dct_method;
  J_DITHER_MODE dither_mode; 
};



inline unsigned char* JPG::getPixels() {
  return pixels;
}

inline unsigned int JPG::getWidth() {
  return width;
}

inline unsigned int JPG::getHeight() {
  return height;
}

inline unsigned int JPG::getNumChannels() {
  return num_channels;
}

inline unsigned int JPG::getBitDepth() {
  return bit_depth;
}

inline unsigned int JPG::getNumBytes() {
  return num_bytes;
}
  
#endif
