#ifndef ROXLU_OPENNI_DEPTH_GENERATORH
#define ROXLU_OPENNI_DEPTH_GENERATORH

#include <OpenNI/XnCppWrapper.h>
#include <OpenNI/XnTypes.h>
#include "Generator.h"

namespace roxlu {
namespace openni {
class OpenNI;

class DepthGenerator : public Generator {
public:
	
	DepthGenerator();
	~DepthGenerator();
	bool setup(roxlu::openni::OpenNI& ni);
	virtual void update();
	xn::DepthGenerator& getGenerator();
	xn::DepthMetaData& getMetaData();
	int getWidth();
	int getHeight();

private:
	int width;
	int height;
	xn::DepthGenerator generator;
	xn::DepthMetaData meta;
};

inline xn::DepthGenerator& DepthGenerator::getGenerator() {
	return generator;
}

inline xn::DepthMetaData& DepthGenerator::getMetaData() {
	return meta;
}

inline int DepthGenerator::getWidth() {
	return width;
}

inline int DepthGenerator::getHeight() {
	return height;
}


}} // roxlu::openni

#endif