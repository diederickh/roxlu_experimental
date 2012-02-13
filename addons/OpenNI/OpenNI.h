#ifndef ROXLU_OPENNIH
#define ROXLU_OPENNIH

#include <OpenNI/XnCppWrapper.h>
#include <OpenNI/XnTypes.h>

#include <string>
#include <vector>

#include "Macros.h"
#include "Generator.h"
#include "ImageGenerator.h"
#include "DepthGenerator.h"
#include "UserGenerator.h"

using std::string;
using std::vector;

namespace ro = roxlu::openni;

namespace roxlu {
namespace openni {

class OpenNI {

public:

	OpenNI();
	~OpenNI();
	void update();
	bool initFromXML(const string& xmlFile);
	const uint8_t* getImagePixels();
	ro::ImageGenerator& getImageGenerator();
	xn::ImageMetaData& getImageMetaData();
	xn::DepthMetaData& getDepthMetaData();
	xn::DepthGenerator& getDepthGenerator();
	int getDepthWidth();
	int getDepthHeight();
	xn::Context& getContext();
	void setGlobalMirror(bool mirror);
	
	bool resetViewPoint();
	bool setViewPoint(xn::ProductionNode& otherNode);
private:
	void logErrors(const xn::EnumerationErrors& errors);
	bool openExistingNodes();
	vector<roxlu::openni::Generator*> generators;
	
	xn::Context context;
	xn::ScriptNode script_node;
	roxlu::openni::ImageGenerator image;
	roxlu::openni::DepthGenerator depth;
};

inline ro::ImageGenerator& OpenNI::getImageGenerator() {
	return image;
}

inline xn::ImageMetaData& OpenNI::getImageMetaData() {
	return image.getMetaData();
}

inline xn::Context& OpenNI::getContext() {
	return context;
}

inline xn::DepthGenerator& OpenNI::getDepthGenerator() {
	return depth.getGenerator();
}

inline xn::DepthMetaData& OpenNI::getDepthMetaData() {
	return depth.getMetaData();
}

inline int OpenNI::getDepthWidth() {
	return depth.getWidth();
}

inline int OpenNI::getDepthHeight() {
	return depth.getHeight();
}

inline void OpenNI::setGlobalMirror(bool mirror) {
	context.SetGlobalMirror(mirror);
}

}} // roxlu

#endif