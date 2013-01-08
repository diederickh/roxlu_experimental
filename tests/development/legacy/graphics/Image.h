#ifndef ROXLU_IMAGEH
#define ROXLU_IMAGEH

#include "../Roxlu.h"
#include <string>
#include "File.h"

using std::string;
namespace roxlu {

class Image {
public:
	Image();
	virtual ~Image();
	bool loadImageFromDataPath(string fileName);
	virtual bool loadImage(string filePath) = 0;
	unsigned char* getPixels();
	int width;
	int height;
	int bytes_per_pixel;
	unsigned char* raw_pixels;
};

}
#endif