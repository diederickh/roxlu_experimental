#ifndef ROXLU_TARGAH
#define ROXLU_TARGAH

#include <iomanip>
#include <fstream>
#include <string> 
#include <roxlu/core/platform/Platform.h>
#include <cstring>

using std::string;

struct TGAHeader {
  rx_uint8 image_id_length;
  rx_uint8 color_map_type;
  rx_uint8 image_type_code;

  rx_uint8 color_map_origin;
  rx_uint16 color_map_length;
  rx_uint8 color_map_entry_size;

  rx_uint16 image_x_origin;
  rx_uint16 image_y_origin;
  rx_uint16 image_width;
  rx_uint16 image_height;
  rx_uint8 bit_count;
  rx_uint8 image_descriptor;
};

#define TGA_COLOR_TYPE_NONE 0
#define TGA_COLOR_TYPE_UNCOMPRESSED_MAPPED 1
#define TGA_COLOR_TYPE_UNCOMPRESSED_RGB 2
#define TGA_COLOR_TYPE_UNCOMPRESSED_BW 3
#define TGA_COLOR_TYPE_RLE_MAPPED 9
#define TGA_COLOR_TYPE_RLE_RGB 10
#define TGA_COLOR_TYPE_COMPRESSED_BW 11
#define TGA_COLOR_TYPE_COMPRESSED_MAPPED 32
#define TGA_COLOR_TYPE_COMPRESSED_MAPPED_QUAD 33

class TGA {
 public:
  TGA();
  TGA(const TGA& other);
  TGA& operator=(const TGA& other);
  TGA& clone(const TGA& other);
  ~TGA();

  bool load(string filepath, bool datapath = false);                                   /* load a TGA image */
  bool save(string filepath, bool datapath = false);                                   /* save the current pixels into a TGA file */

	bool setPixels(unsigned char* pixels, rx_uint32 w, rx_uint32, int numChannels);      /* copy pixel data */
  unsigned char* getPixels();                                                          /* get pointer to the pixels */

  rx_uint32 getWidth();                                                                /* returns the width of the image */
  rx_uint32 getHeight();                                                               /* returns the height of the image */
  unsigned char getNumChannels();                                                      /* get number of color channels per pixel */
  size_t numBytes();
  void print();
  std::string colorTypeToString(unsigned int t);

 public:	
  unsigned char* pixels;                     /* the pixels */
  size_t num_bytes;                          /* number of bytes in pixels */
  unsigned char bits_per_pixel;              /* number of bits per pixel, 24 for RGB, 32 for RGBA */
  unsigned char header[12];                  /* the targa header */
  unsigned char id;                          /* targa id */
  unsigned int color_type;                   /* targa image/color type */
  unsigned int num_channels;                 /* number of color channels */
  size_t stride;                             /* stride per row */
  rx_uint32 width;                           /* image width */
  rx_uint32 height;                          /* image height */
	
};

inline unsigned char* TGA::getPixels() {
  return pixels;
}

inline bool TGA::setPixels(unsigned char* data, rx_uint32 w, rx_uint32 h, int numChannels) {
  if(pixels) {
   delete[] pixels;
  }

  width = w;
  height = h;
  bits_per_pixel = numChannels * 8;
  num_channels = numChannels;
  num_bytes = width * height * (bits_per_pixel / 8);
  color_type = TGA_COLOR_TYPE_UNCOMPRESSED_RGB; 
  pixels = new unsigned char[num_bytes];

  if(!pixels) {
    return false;
  }
  memcpy(pixels, data, num_bytes);

  
  return true;
}

inline rx_uint32 TGA::getWidth() {
  return width;
}

inline rx_uint32 TGA::getHeight() {
  return height;
}

inline unsigned char TGA::getNumChannels() {
  return num_channels;
}



#endif
