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
	unsigned char* getPixels();
	WebPDecoderConfig config;
	unsigned char* data;
};

#endif