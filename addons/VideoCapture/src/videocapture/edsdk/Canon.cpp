#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <videocapture/edsdk/Canon.h>

// -----------------------------------------------------------
EdsError EDSCALLBACK canon_handle_object_event(EdsUInt32 event, EdsBaseRef ref, EdsVoid*  user) {
  EdsError result = EDS_ERR_OK;
  Canon* canon = static_cast<Canon*>(user);
  //  RX_VERBOSE("object event: %s", canon_event_to_string(event).c_str());
  if(event == kEdsObjectEvent_DirItemCreated) {
    canon->downloadPicture((EdsDirectoryItemRef)ref);
  }
  return result;
}

EdsError EDSCALLBACK canon_handle_property_event(EdsUInt32 event, EdsUInt32 property, EdsUInt32 param, EdsVoid* user) {
  EdsError result = EDS_ERR_OK;
  //  RX_VERBOSE("property event: %s", canon_event_to_string(event).c_str());
  //  RX_VERBOSE("property: %s", canon_property_to_string(param).c_str());
  return result;
}

EdsError EDSCALLBACK canon_handle_state_event(EdsUInt32 event, EdsUInt32 param, EdsVoid* user) {
  EdsError result = EDS_ERR_OK;
  //RX_VERBOSE("state event: %s", canon_event_to_string(event).c_str());

  return result;
}

EdsError EDSCALLBACK canon_download_progress(EdsUInt32 percent, EdsVoid* user, EdsBool* outCancel) {
  return EDS_ERR_OK;
}

// -----------------------------------------------------------

Canon::Canon() 
  :state(STATE_NONE)
  ,queue(this)
{
  EdsError err = EdsInitializeSDK();
  if(err != EDS_ERR_OK) {
    RX_ERROR("Cannot initialize EDSDK");
    ::exit(EXIT_FAILURE);
  }

}

Canon::~Canon() {
  EdsTerminateSDK();
}


// VideoCaptureBase implementation
// -----------------------------------------------------------

int Canon::listDevices() {
  EdsCameraListRef camera_list = NULL;
  EdsUInt32 count;
  EdsError err = EDS_ERR_OK;
  EdsCameraRef camera;

  err = EdsGetCameraList(&camera_list);
  if(err != EDS_ERR_OK) {
    RX_ERROR("Cannot get the camera list");
    return 0;
  }

  if(camera_list) {

    err = EdsGetChildCount(camera_list, &count);
    for(EdsUInt32 i = 0; i < count; ++i) {
      err = EdsGetChildAtIndex(camera_list, i, &camera);
      if(err != EDS_ERR_OK) {
        RX_ERROR("Error while retrieving camera: %d", int(i));
        break;
      }
      EdsRelease(camera);
      camera = NULL;
      RX_VERBOSE("[%d] Canon camera %d", int(i), int(i));
    }

    EdsRelease(camera_list);
    camera_list = NULL;
  }
  
  return count;
}

bool Canon::openDevice(int device, VideoCaptureSettings cfg) {
  EdsError err = EDS_ERR_OK;
  EdsCameraListRef camera_list = NULL;

  err = EdsGetCameraList(&camera_list);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    goto error;
  }

  err = EdsGetChildAtIndex(camera_list, device, &input_device.camera_ref);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    goto error;
  }

  err = EdsSetObjectEventHandler(input_device.camera_ref, kEdsObjectEvent_All, canon_handle_object_event, (EdsVoid *)this);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    goto error;
  }

  err = EdsSetCameraStateEventHandler(input_device.camera_ref, kEdsStateEvent_All, canon_handle_state_event, (EdsVoid *)this);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    goto error;
  }

  err = EdsSetPropertyEventHandler(input_device.camera_ref, kEdsPropertyEvent_All, canon_handle_property_event, (EdsVoid *)this);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    goto error;
  }

  if(!openSession()) {
    RX_ERROR("Cannot add the open session task");
    goto error;
  }

  state = STATE_OPENED;

  queue.start();

  EdsRelease(camera_list);

  return true;

 error:
  EdsRelease(camera_list);
  EdsRelease(input_device.camera_ref);
  return false;
}

bool Canon::closeDevice() {
  RX_VERBOSE("@todo unset the property event handler");

  if(state != STATE_OPENED) {
    RX_ERROR("Cannot close the device because we didn't opened a device yet. Did you call openDevice()?");
    return false;
  }

  if(!closeSession()) {
    RX_ERROR("Cannot add the close session task");
    return false;
  }

  queue.stop();

  return true;
}

bool Canon::startCapture() {
  return true;
}

bool Canon::stopCapture() {
  return true;
}

void Canon::update() {
}

// Camera control
// -----------------------------------------------------------
bool Canon::openSession() {
  if(state == STATE_OPENED) {
    RX_ERROR("Cannot re-open the session, did you already open it? make sure to call closeSession first");
    return false;
  }  
  
  CanonTaskOpenSession* task = new CanonTaskOpenSession(this);
  if(!task) {
    RX_ERROR("Cannot allocate the open session task");
    return false;
  }
  
  queue.addTask(task);
  return true;
}

bool Canon::closeSession() {
  if(state != STATE_OPENED) {
    RX_ERROR(ERR_CANON_NOT_OPENED);
    return false;
  }  
  
  CanonTaskCloseSession* task = new CanonTaskCloseSession(this);
  if(!task) {
    RX_ERROR("Cannot allocate the close session task");
    return false;
  }
  
  queue.addTask(task);
  return true;
}

bool Canon::takePicture() {
  if(state != STATE_OPENED) {
    RX_ERROR(ERR_CANON_NOT_OPENED);
    return false;
  }

  CanonTaskTakePicture* task = new CanonTaskTakePicture(this);
  if(!task) {
    RX_ERROR("Cannot allocate take picture task");
    return false;
  }
  uint64_t n = rx_millis();
  queue.addTask(task);
  uint64_t d = rx_millis() - n;
  RX_VERBOSE("Took: %ld", d);
  return true;
}

bool Canon::downloadPicture(EdsDirectoryItemRef item) {

  if(state != STATE_OPENED) {
    RX_ERROR(ERR_CANON_NOT_OPENED);
    return false;
  }

  CanonTaskDownload* task = new CanonTaskDownload(this, item);
  if(!task) {
    RX_ERROR("Cannot allocate CanonTaskDownload");
    return false;
  }

  queue.addTask(task);
  return true;
}

bool Canon::lockUI() {
  if(state != STATE_OPENED) {
    RX_ERROR(ERR_CANON_NOT_OPENED);
    return false;
  }

  EdsError err = EdsSendStatusCommand(input_device.camera_ref, kEdsCameraStatusCommand_UILock, 0);
  if(err != EDS_ERR_OK) {
    RX_ERROR("Cannot lock the ui");
    return false;
  }
  return true;
}

bool Canon::unlockUI() {
  if(state != STATE_OPENED) {
    RX_ERROR(ERR_CANON_NOT_OPENED);
    return false;
  }

  EdsError err = EdsSendStatusCommand(input_device.camera_ref, kEdsCameraStatusCommand_UIUnLock, 0);
  if(err != EDS_ERR_OK) {
    RX_ERROR("Cannot unlock the ui");
    return false;
  }

  return true;
}


// Init + Shutdown : will be called by the queue thread when the thread starts and ends.
// -----------------------------------------------------------
bool Canon::initialize() {
  RX_VERBOSE("Handle initialize - add tasks that get e.g. device info?");
  return true;
}

bool Canon::shutdown() {
  RX_VERBOSE("Handle shutdown - cleanup what is created in openDevice()");
  state = STATE_NONE;
  return true;
}


// Get properties
// -----------------------------------------------------------
bool Canon::getCameraName() {
  return getProperty(kEdsPropID_ProductName);
}

bool Canon::getProperty(EdsPropertyID prop) {
 if(state != STATE_OPENED) {
    RX_ERROR(ERR_CANON_NOT_OPENED);
    return false;
  }

 CanonTaskProperty* task = new CanonTaskProperty(this, prop);
  if(!task) {
    RX_ERROR("Cannot allcate the property");
    return false;
  }
  queue.addTask(task);
  return true;
}


