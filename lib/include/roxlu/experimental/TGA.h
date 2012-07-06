#ifndef ROXLU_TARGAH
#define ROXLU_TARGAH

#include <iomanip>
#include <fstream>
#include <string> 
#include <roxlu/core/platform/Platform.h>

using std::string;

namespace roxlu {

struct TGAHeader {
	uint8_t image_id_length;
	uint8_t color_map_type;
	uint8_t image_type_code;

	uint8_t color_map_origin;
	uint16_t color_map_length;
	uint8_t color_map_entry_size;

	uint16_t image_x_origin;
	uint16_t image_y_origin;
	uint16_t image_width;
	uint16_t image_height;
	uint8_t bit_count;
	uint8_t image_descriptor;
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
	uint32_t getWidth();
	uint32_t getHeight();
	unsigned char getBPP();
	
	unsigned char* pixels;
	unsigned char bpp;
	unsigned char header[12];
	unsigned char id;
	uint32_t width;
	uint32_t height;
	size_t num_bytes;
	
	
};

inline unsigned char* TGA::getPixels() {
	return pixels;
}

inline uint32_t TGA::getWidth() {
	return width;
}

inline uint32_t TGA::getHeight() {
	return height;
}

inline unsigned char TGA::getBPP() {
	return bpp;
}

} // roxlu

#endif