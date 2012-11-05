#ifndef ROXLU_TARGAH
#define ROXLU_TARGAH

#include <iomanip>
#include <fstream>
#include <string> 
#include <roxlu/core/platform/Platform.h>
#include <cstring>

using std::string;

namespace roxlu {

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


class TGA {
public:
	TGA();
	~TGA();
	bool load(const string& filepath);
	bool save(const string& filename, unsigned char* rgb, int w, int h);
	size_t numBytes();
	void print();
	
	unsigned char* getPixels();
	rx_uint32 getWidth();
	rx_uint32 getHeight();
	unsigned char getBPP();
	
	unsigned char* pixels;
	unsigned char bpp;
	unsigned char header[12];
	unsigned char id;
	rx_uint32 width;
	rx_uint32 height;
	size_t num_bytes;
	
	
};

inline unsigned char* TGA::getPixels() {
	return pixels;
}

inline rx_uint32 TGA::getWidth() {
	return width;
}

inline rx_uint32 TGA::getHeight() {
	return height;
}

inline unsigned char TGA::getBPP() {
	return bpp;
}

} // roxlu

#endif
