#ifndef ROXLU_OPENNI_IMAGEGENERATORH
#define ROXLU_OPENNI_IMAGEGENERATORH

#include <OpenNI/XnCppWrapper.h>
#include <OpenNI/XnTypes.h>
#include "Generator.h"


namespace roxlu {
namespace openni {

class OpenNI;

class ImageGenerator : public Generator {

public:

	ImageGenerator();
	~ImageGenerator();
	
	virtual void update();
	const uint8_t* getPixels();
	xn::ImageGenerator& getGenerator();
	xn::ImageMetaData& getMetaData();	
	
private:
	xn::ImageMetaData meta;
	xn::ImageGenerator generator;
};

inline xn::ImageGenerator& ImageGenerator::getGenerator() {
	return generator;
}

inline xn::ImageMetaData& ImageGenerator::getMetaData() {
	return meta;
}

}} // roxlu

#endif