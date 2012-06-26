#ifndef ROXLU_TARGAH
#define ROXLU_TARGAH

#include <string> 

using std::string;

class TGA {
public:
	TGA();
	~TGA();
	bool load(const string& filepath);
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

#endif