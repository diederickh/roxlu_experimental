#ifndef ROXLU_WEBPIMAGEH
#define ROXLU_WEBPIMAGEH

#include "Image.h"

namespace roxlu {

class WebPImage : public Image {
public:
	WebPImage();
	virtual ~WebPImage();
	virtual bool loadImage(string filePath);
	
};

}; // roxlu
#endif