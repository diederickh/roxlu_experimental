#include "OpenNI.h"
#include <OpenNI/XnLog.h>

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
					(*it).GetInstance(player.getPlayer());
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

bool OpenNI::initFromRecording(const string& recording) {
	XnStatus status = context.Init();
	if(status != XN_STATUS_OK) {
		printf("OpenNI.initFromRecording() - error");
		return false;
	}

	xnLogInitFromXmlFile("../Frameworks/OpenNI.framework/Resources/log.xml");
	SHOW_RC(status,  "Init log from xml");
	
	addLicense("PrimeSense", "0KOIk2JeIBYClPWVnMoRKn5cdY4=");

	status = context.OpenFileRecording(recording.c_str(), player.getPlayer());
	SHOW_RC(status, "OpenNI.initFromRecording() - setup from recording");

	if(status!=XN_STATUS_OK) return false;

	openExistingNodes();

	BOOL_RC(status, "OpenNI.initFromRecording()");
}

void OpenNI::addLicense(const string& vendor, const string& key) {
	XnLicense license = {0};
	XnStatus status = XN_STATUS_OK;
	status = xnOSStrNCopy(license.strVendor, vendor.c_str(), vendor.size(), sizeof(license.strVendor));
	if(status != XN_STATUS_OK) {
		printf("OpenNI.addLicense() - vendor copy error\n");
		return;
	}

	status = xnOSStrNCopy(license.strKey, key.c_str(), key.size(), sizeof(license.strKey));
	if(status != XN_STATUS_OK) {
		printf("OpenNI.addLicense() - key copy error\n");
		return;
	}

	status = context.AddLicense(license);
	SHOW_RC(status, "AddLicense");
	xnPrintRegisteredLicenses();
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