#ifndef ROXLU_IMAGE_JPG_H
#define ROXLU_IMAGE_JPG_H

#include <string>
#include <stdio.h>

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
  bool save(std::string filename, bool datapath = false);

  
  unsigned char* getPixels();
  unsigned int getWidth();
  unsigned int getHeight();
  unsigned int getNumChannels();
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

    
  
#endif
