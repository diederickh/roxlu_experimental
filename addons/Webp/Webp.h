#ifndef ROXLU_WEBPH
#define ROXLU_WEBPH

#include "Roxlu.h"
#include <string>
#include "./decode.h"

class WebP {
public:
	WebP();
	WebP(const string& filename);
	~WebP();
	bool load(const string& filename);
	int getWidth();
	int getHeight();
	unsigned char* getPixels();
	WebPDecoderConfig config;

private:
	bool loaded;
};

inline unsigned char* WebP::getPixels() {
	return config.output.u.RGBA.rgba;
}

inline int WebP::getWidth() {
	return config.output.width;
}

inline int WebP::getHeight() {
	return config.output.height;
}


#endif