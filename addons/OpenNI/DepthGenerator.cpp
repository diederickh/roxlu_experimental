#include "DepthGenerator.h"
#include "OpenNI.h"

namespace roxlu {
namespace openni {

DepthGenerator::DepthGenerator() 
	:width(640)
	,height(480)
{
}

DepthGenerator::~DepthGenerator() {
	generator.Release();
}

bool DepthGenerator::setup(roxlu::openni::OpenNI& ni) {
	XnStatus status = XN_STATUS_OK;
	status = generator.Create(ni.getContext());
	if(status != XN_STATUS_OK) {
		SHOW_RC(status, "DepthGenrator.setup(), creating depth generator.")
		return false;
	}
	
	XnMapOutputMode map_mode;
	map_mode.nXRes = width = XN_VGA_X_RES;
	map_mode.nYRes = height = XN_VGA_Y_RES;
	map_mode.nFPS = 30;
	status = generator.SetMapOutputMode(map_mode);
	if(status != XN_STATUS_OK) {
		SHOW_RC(status, "DepthGenerator.setup(), set output mode");
		return false;
	}
	
	status = generator.StartGenerating();
	if(status != XN_STATUS_OK) {
		SHOW_RC(status, "DepthGenerator.setup(), start generting.");
		return false;
	}
	return true;
}

void DepthGenerator::update() {
	generator.GetMetaData(meta);
}


}} // roxlu::openni