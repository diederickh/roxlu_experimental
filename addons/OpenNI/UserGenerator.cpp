#include "UserGenerator.h"
#include "OpenNI.h"

namespace ro = roxlu::openni;

// TODO generator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL); 

void XN_CALLBACK_TYPE User_NewUser(
							 xn::UserGenerator& xnGenerator
							,XnUserID id
							,void* cookie
)
{
	printf("--------------------------------------------------------------\n");
	printf(">> new user: %d\n", id);
	ro::UserGenerator* generator = static_cast<ro::UserGenerator*>(cookie);
	if(generator->needsPoseForCalibration()) {
		printf("Needs pose for calibration\n");
		}
	else {
		printf("No pose for calibration\n");
	}
}

void XN_CALLBACK_TYPE User_LostUser(
							 xn::UserGenerator& xnGenerator
							,XnUserID id
							,void* cookie
)
{
	printf("--------------------------------------------------------------\n");
	printf("Lost User: %d\n", id);
	printf("--------------------------------------------------------------\n");
	ro::UserGenerator* generator = static_cast<ro::UserGenerator*>(cookie);
	generator->getUsers()[id]->is_tracked = false;
	xnGenerator.GetSkeletonCap().Reset(id);
}

void XN_CALLBACK_TYPE Pose_Detected(
							 xn::PoseDetectionCapability& pose
							,const XnChar* strPose
							,XnUserID id
							,void* cookie
)
{
	printf("--------------------------------------------------------------\n");
	printf("Pose detected: %d\n", id);
	printf("--------------------------------------------------------------\n");
	ro::UserGenerator* generator = static_cast<ro::UserGenerator*>(cookie);
	generator->requestCalibration(id);
	generator->stopPoseDetection(id);
}


void XN_CALLBACK_TYPE Calibration_Started(
							 xn::SkeletonCapability& skeleton
							,XnUserID id
							,void* cookie
)
{
	printf("--------------------------------------------------------------\n");
	printf("Calbration started: %d\n", id);
	printf("--------------------------------------------------------------\n");
}

void XN_CALLBACK_TYPE Calibration_Completed(
							 xn::SkeletonCapability& skeleton
							,XnUserID id
							,XnCalibrationStatus status
							,void* cookie
)
{
	printf("--------------------------------------------------------------\n");
	printf("Calbration completed: %d\n", id);
	printf("--------------------------------------------------------------\n");

	ro::UserGenerator* generator = static_cast<ro::UserGenerator*>(cookie);
	if(status == XN_CALIBRATION_STATUS_OK) {
		generator->startTracking(id);
	}
	else {
		if(generator->needsPoseForCalibration()) {
			generator->startPoseDetection(id);
		}
		else {
			generator->requestCalibration(id);
		}
	}
	/*
		printf("Calibration done [%d] %ssuccessfully\n", user, (eStatus == XN_CALIBRATION_STATUS_OK)?"":"un");
	if (eStatus == XN_CALIBRATION_STATUS_OK)
	{
		if (!g_bCalibrated)
		{
			g_UserGenerator.GetSkeletonCap().SaveCalibrationData(user, 0);
			g_nPlayer = user;
			g_UserGenerator.GetSkeletonCap().StartTracking(user);
			g_bCalibrated = TRUE;
		}

		XnUserID aUsers[10];
		XnUInt16 nUsers = 10;
		g_UserGenerator.GetUsers(aUsers, nUsers);
		for (int i = 0; i < nUsers; ++i)
			g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(aUsers[i]);
	}
	*/
}

// -----------------------------------------------------------------------------

namespace roxlu {
namespace openni {


UserGenerator::UserGenerator()
	:needs_pose(true)
	,num_pixels(0)
{
	// We create the users in advance.
	for(int i = 0; i < MAX_NUM_OPENNI_USERS; ++i) {
		ro::User* u = new ro::User(i);
		users.insert(std::pair<XnUserID, ro::User*>(i, u));		
	}
}

UserGenerator::~UserGenerator() {
	generator.Release();
	for(int i = 0; i < MAX_NUM_OPENNI_USERS; ++i) {
		delete users[i];
	}

}

bool UserGenerator::setup(roxlu::openni::OpenNI& ni) {
	if(!ni.getDepth().getGenerator().IsValid()) {
		printf("UserGenerator.setup - Depth generator is not valid.\n");
		exit(0);
	}

	
	// Get the width/height from depth maps
	XnStatus status = XN_STATUS_OK;
	XnMapOutputMode map_mode;
	status = ni.getDepth().getGenerator().GetMapOutputMode(map_mode);
	if(status != XN_STATUS_OK) {
		SHOW_RC(status, "UserGenerator.setup() - GetMapOutputMode");
		exit(0);
	}
	
	width = map_mode.nXRes;
	height = map_mode.nYRes;
	num_pixels = width * height;
	
	// Setup uses now we know how the images are.
	for(int i = 0; i < MAX_NUM_OPENNI_USERS; ++i) {
		users[i]->setup(width, height);
	}
	
	// Create user generator.
	status = generator.Create(ni.getContext());
	if(status != XN_STATUS_OK) {
		SHOW_RC(status, "UserGenerator.setup() - generator.Create");
		exit(0);
	}	
	
	// Register callbacks
	if(!registerCallbacks()) {
		return false;
	}
	
	// Get all skeleton info.
	generator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
	
	// Must call this when using a recording
	ni.getContext().StartGeneratingAll();
	return true;
}

void UserGenerator::update() {
	XnUserID user_ids[MAX_NUM_OPENNI_USERS];
	XnUInt16 num_users = MAX_NUM_OPENNI_USERS;
	generator.GetUsers(user_ids, num_users);
	
	for(int i = 0; i < MAX_NUM_OPENNI_USERS; ++i) {
		users[i]->is_tracked = false;
	}
	
	for(int i = 0; i < num_users; ++i) {
		updateUserPixels(users[user_ids[i]]);
		users[user_ids[i]]->is_tracked = true;
	}
}


bool UserGenerator::registerCallbacks() {
	XnStatus status = XN_STATUS_OK;
	
	// User callbacks
	status = generator.RegisterUserCallbacks(User_NewUser, User_LostUser, this, cb_user);
	if(status != XN_STATUS_OK) {
		SHOW_RC(status, "UserGenerator.registerCallbacks() - user callbacks");
		return false;
	}
	
	
	// When we need pose for calibration, register callbacks.
	if(generator.GetSkeletonCap().NeedPoseForCalibration()) {
		needs_pose = false;
		
		// Is pose calib. supported?
		if(!generator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)) {
			printf("UserGenerator.registerCallbacks - pose calibration capab. not suppoerted\n");
			return false;
		}
			
		// Start calibration.
		status = generator.GetSkeletonCap().RegisterToCalibrationStart(Calibration_Started, this, cb_calib_start);
		if(status != XN_STATUS_OK) {
			SHOW_RC(status, "UserGenerator.registerCallbacks() - calibration start callback");
			return false;
		}
	
		// Calibration complete
		status = generator.GetSkeletonCap().RegisterToCalibrationComplete(Calibration_Completed, this, cb_calib_end);
		if(status != XN_STATUS_OK) {
			SHOW_RC(status, "UserGenerator.registerCallbacks() - calibration end");
			return false;
		}
		
		// Pose detected
		status = generator.GetPoseDetectionCap().RegisterToPoseDetected(Pose_Detected, NULL, cb_pose_detected);
		if(status != XN_STATUS_OK) {
			SHOW_RC(status, "UserGenerator.registerCallbacks() - pose detected");
			return false;
		}
		
		// What calibration pose do we need
		generator.GetSkeletonCap().GetCalibrationPose(calibration_pose);
	}
	else {
		needs_pose = false;
	}
	

	return true;	
}


void UserGenerator::startPoseDetection(XnUserID id) {
	generator.GetPoseDetectionCap().StartPoseDetection(calibration_pose, id);
}

void UserGenerator::stopPoseDetection(XnUserID id) {
	generator.GetPoseDetectionCap().StopPoseDetection(id);
}

void UserGenerator::requestCalibration(XnUserID id) {
	generator.GetSkeletonCap().RequestCalibration(id, TRUE);
}

void UserGenerator::startTracking(XnUserID id) {
	generator.GetSkeletonCap().StartTracking(id);
}
	
void UserGenerator::updateUserPixels(ro::User* user) {
	if(generator.GetUserPixels(user->getID(), meta) != XN_STATUS_OK) {
	//if(generator.GetUserPixels(0, meta) != XN_STATUS_OK) {
		printf("UserGenerator.updateUserPixels(), cannot get pixels\n");
		return;
	}
	unsigned short* user_labels = (unsigned short*)meta.Data();
	if(user_labels == NULL) {
		return;
	}
	for(int i = 0; i < num_pixels; ++i) {
		if(user_labels[i] == user->id) {
			user->mask_pixels[i] = 255;
		}
		else {
			user->mask_pixels[i] = 0;
		}
	}
}


}} // roxlu::openni