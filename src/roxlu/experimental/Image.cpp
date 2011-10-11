#include "Image.h"

namespace roxlu {

Image::Image() 
	:width(0)
	,height(0)
	,bytes_per_pixel(0)
	,raw_pixels(NULL)
{
}

Image::~Image() {
	if(raw_pixels != NULL) {
		delete raw_pixels;
	}
}
unsigned char* Image::getPixels() {
	return raw_pixels;
}

bool Image::loadImageFromDataDir(string fileName) {
	return loadImage((string)File::toDataPath(fileName));
}

}