#include "ImageGenerator.h"
#include "OpenNI.h"

namespace roxlu {
namespace openni {

ImageGenerator::ImageGenerator() {
}

ImageGenerator::~ImageGenerator() {
	generator.Release();
}


void ImageGenerator::update() {
	generator.GetMetaData(meta);
}

const uint8_t* ImageGenerator::getPixels() {
	if(!generator.IsValid()) {
		return NULL;
	}
	return (const uint8_t*)meta.Data();
}

}} // roxlu::openni