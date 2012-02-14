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
#include "Recorder.h"
#include "Player.h" 

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
	bool initFromRecording(const string& recording);
	void addLicense(const string& vendor, const string& key);
	
	const uint8_t* getImagePixels();
	ro::ImageGenerator& getImage();
	xn::ImageMetaData& getImageMetaData();
	xn::DepthMetaData& getDepthMetaData();
	ro::DepthGenerator& getDepth();
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
	ro::ImageGenerator image;
	ro::DepthGenerator depth;
	ro::Player player;
};

inline ro::ImageGenerator& OpenNI::getImage() {
	return image;
}

inline xn::ImageMetaData& OpenNI::getImageMetaData() {
	return image.getMetaData();
}

inline xn::Context& OpenNI::getContext() {
	return context;
}

inline ro::DepthGenerator& OpenNI::getDepth() {
	return depth;
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