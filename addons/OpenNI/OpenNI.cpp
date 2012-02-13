#include "OpenNI.h"

namespace roxlu {
namespace openni {

OpenNI::OpenNI() {
}

OpenNI::~OpenNI() {
}

const uint8_t* OpenNI::getImagePixels() {
	return image.getPixels();
}

void OpenNI::update() {
	XnStatus status = XN_STATUS_OK;
	status = context.WaitAnyUpdateAll();
	if(status != XN_STATUS_OK) {
		SHOW_RC(status, "OpenNI.update()");
		return;
	}
	
	// update all created generators.
	vector<roxlu::openni::Generator*>::iterator it = generators.begin();
	while(it != generators.end()) {
		(*it)->update();
		++it;
	}
}

bool OpenNI::openExistingNodes() {
	XnStatus status = XN_STATUS_OK;
	xn::NodeInfoList list;
	
	status = context.EnumerateExistingNodes(list);
	if(status != XN_STATUS_OK) {
		SHOW_RC(status, "OpenNI.openExistingNodes()");
		return false;
	}
	
	xn::NodeInfoList::Iterator it = list.Begin();
	while(it != list.End()) {
		switch((*it).GetDescription().Type) {
			case XN_NODE_TYPE_DEVICE:  {
					break;
			}
			case XN_NODE_TYPE_DEPTH: {
					printf("Creating depth generator.\n");
					(*it).GetInstance(depth.getGenerator());
					generators.push_back(&depth);
					break;
			}
			case XN_NODE_TYPE_IMAGE: {
					printf("Creating image generator.\n");
					(*it).GetInstance(image.getGenerator());
					generators.push_back(&image);
					break;
			}
			case XN_NODE_TYPE_IR: {
					break;
			}
			case XN_NODE_TYPE_AUDIO: {
					break;
			}
			case XN_NODE_TYPE_PLAYER: {
					break;
			}
			default: {
				printf("Unhandled node in openExistingNodes.\n");
				break;
			}
		}	
		++it;
	}
	return true;
}


bool OpenNI::initFromXML(const string& xmlFile) {
	XnStatus status =  XN_STATUS_OK;
	xn::EnumerationErrors errors;
	
	status = context.InitFromXmlFile(xmlFile.c_str(), script_node, &errors);
	if(status != XN_STATUS_OK) {	
		logErrors(errors);
		return false;
	}
	
	openExistingNodes();
	
	BOOL_RC(status, "OpenNI.initFromXML()");
}

void OpenNI::logErrors(const xn::EnumerationErrors& errors) {
	xn::EnumerationErrors::Iterator it = errors.Begin();
	while(it != errors.End()) {
		XnChar desc[512];
		xnProductionNodeDescriptionToString(&it.Description(), desc, 512);
		printf("%s failed: %s\n", desc, xnGetStatusString(it.Error()));
		++it;
	}
}

/**
 *
 * View point: how to map the depth images; 
 * Use this to align the depth image to the rgb-image
 *
 */
bool OpenNI::setViewPoint(xn::ProductionNode& otherNode) {
	if(!depth.getGenerator().IsValid() || !image.getGenerator().IsValid()) {
		return false;
	}
	if(!depth.getGenerator().IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)) {
		return false;
	}
	
	XnStatus status = depth.getGenerator().GetAlternativeViewPointCap().SetViewPoint(otherNode);
	if(status != XN_STATUS_OK) {
		SHOW_RC(status, "OpenNI.setViewPoint()");
		return false;
	}
	return true;
}

bool OpenNI::resetViewPoint() {
	if(!depth.getGenerator().IsValid() || !image.getGenerator().IsValid()) {
		return false;
	}
	if(!depth.getGenerator().IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)) {
		return false;
	}
	XnStatus status = depth.getGenerator().GetAlternativeViewPointCap().ResetViewPoint();
	if(status != XN_STATUS_OK) {
		SHOW_RC(status, "OpenNI.resetViewPoint()");
		return false;
	}
	return true;
}

}} // roxlu::openni