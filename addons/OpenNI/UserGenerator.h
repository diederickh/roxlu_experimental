#ifndef ROXLU_OPENNI_USER_GENERATORH
#define ROXLU_OPENNI_USER_GENERATORH


#include <map>
#include <OpenNI/XnCppWrapper.h>
#include <OpenNI/XnTypes.h>
#include "Generator.h"
#include "Macros.h"

#include "User.h"

#define MAX_NUM_OPENNI_USERS  8

using std::map;

namespace ro = roxlu::openni;

namespace roxlu {
namespace openni {

class OpenNI;

class UserGenerator : public Generator {

public:
	
	UserGenerator();
	~UserGenerator();
	
	
	virtual void update();
	xn::UserGenerator& getGenerator();
	bool setup(ro::OpenNI& ni);
	bool registerCallbacks();
	bool needsPoseForCalibration();
	void startPoseDetection(XnUserID id);
	void stopPoseDetection(XnUserID id);
	void requestCalibration(XnUserID id);
	void startTracking(XnUserID id);

	bool getAllUserPixels(xn::SceneMetaData& meta);
	bool getUserPixels(XnUserID id, xn::SceneMetaData& meta);
	map<XnUserID, ro::User*>& getUsers();
	
private:
	void updateUserPixels(ro::User* user);
	map<XnUserID, ro::User*> users;
	xn::SceneMetaData meta;
	
	XnChar calibration_pose[20];
	bool needs_pose;
	XnCallbackHandle cb_pose_detected;
	XnCallbackHandle cb_calib_end;
	XnCallbackHandle cb_calib_start;
	XnCallbackHandle cb_user;
	xn::UserGenerator generator;	
	int width;
	int height;
	int num_pixels;
	//bool tracked_users[MAX_NUM_OPENNI_USERS];
};

inline xn::UserGenerator& UserGenerator::getGenerator() {
	return generator;
}

inline bool UserGenerator::needsPoseForCalibration() {
	return needs_pose;
}

inline bool UserGenerator::getAllUserPixels(xn::SceneMetaData& meta) {
	return getUserPixels(0, meta);	
}

inline bool UserGenerator::getUserPixels(XnUserID id, xn::SceneMetaData& meta) {
	XnStatus status = generator.GetUserPixels(id, meta);
	return status == XN_STATUS_OK;
}

inline map<XnUserID, ro::User*>& UserGenerator::getUsers() {
	return users;
}

}} // roxlu::openni

#endif