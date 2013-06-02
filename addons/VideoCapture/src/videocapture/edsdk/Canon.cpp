#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <videocapture/edsdk/Canon.h>
#include <videocapture/edsdk/CanonUtils.h>

// -----------------------------------------------------------
EdsError EDSCALLBACK canon_handle_object_event(EdsUInt32 event, EdsBaseRef ref, EdsVoid*  user) {
  EdsError result = EDS_ERR_OK;
  Canon* canon = static_cast<Canon*>(user);

  if(event == kEdsObjectEvent_DirItemCreated) {
    canon->downloadPicture((EdsDirectoryItemRef)ref);
  }
  else if(event == kEdsObjectEvent_DirItemRequestTransfer) {
    canon->downloadPicture((EdsDirectoryItemRef)ref);
  }
  return result;
}

EdsError EDSCALLBACK canon_handle_property_event(EdsUInt32 event, EdsUInt32 property, EdsUInt32 param, EdsVoid* user) {
  Canon* canon = static_cast<Canon*>(user);
  EdsError result = EDS_ERR_OK;

  if(event == kEdsPropertyEvent_PropertyChanged) {
    canon->getProperty(property);
  }
   
  return result;
}

EdsError EDSCALLBACK canon_handle_state_event(EdsUInt32 event, EdsUInt32 param, EdsVoid* user) {
  EdsError result = EDS_ERR_OK;
  return result;
}

EdsError EDSCALLBACK canon_download_progress(EdsUInt32 percent, EdsVoid* user, EdsBool* outCancel) {
  return EDS_ERR_OK;
}

// -----------------------------------------------------------

Canon::Canon() 
  :state(STATE_NONE)
  ,queue(this)
  ,input_device(this)
{

  EdsError err = EdsInitializeSDK();
  if(err != EDS_ERR_OK) {
    RX_ERROR("Cannot initialize EDSDK");
    ::exit(EXIT_FAILURE);
  }

}

Canon::~Canon() {
  EdsError err = EdsCloseSession(getCameraRef());
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

  queue.start();

  EdsRelease(camera_list);

  return true;

 error:
  EdsRelease(camera_list);
  EdsRelease(input_device.camera_ref);
  return false;
}

void Canon::setStateOpened() {
  state = STATE_OPENED;
}
void Canon::setStateLiveView() {
  state =STATE_LIVE_VIEW;
}

bool Canon::closeDevice() {
  if(state == STATE_LIVE_VIEW) {
    endLiveView();
  }

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
  return startLiveView();
}

bool Canon::stopCapture() {
  return endLiveView();
}

void Canon::update() {
}

// Events & Callbacks
// -----------------------------------------------------------
void Canon::fireEvent(CanonEvent& ev) {

  if(ev.type == CANON_EVENT_EVF_DATA) {
    EdsUInt32 buffer_size = 0;
    unsigned char* data;
    CanonLiveViewData& live_view_data = *static_cast<CanonLiveViewData*>(ev.data);
    EdsGetLength(live_view_data.stream_ref, &buffer_size);
    EdsGetPointer(live_view_data.stream_ref, (EdsVoid**)&data);

    if(data[0] != 0x00 && data[1] != 0x00) {
      if(jpg.load(data, buffer_size)) {
        assert(cb_frame);
        cb_frame((void*)jpg.getPixels(), jpg.getNumBytes(), cb_user);
      }
    }

    downloadLiveView();
  }
  else if(ev.type == CANON_EVENT_EVF_STARTED) {
    downloadLiveView();
  }

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
  if(state == STATE_LIVE_VIEW) {
    endLiveView();
  }

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
  queue.addTask(task);
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
  EdsError err = EdsSendStatusCommand(input_device.camera_ref, kEdsCameraStatusCommand_UILock, 0);
  if(err != EDS_ERR_OK) {
    RX_ERROR("Cannot lock the ui");
    return false;
  }
  return true;
}

bool Canon::unlockUI() {
  EdsError err = EdsSendStatusCommand(input_device.camera_ref, kEdsCameraStatusCommand_UIUnLock, 0);
  if(err != EDS_ERR_OK) {
    RX_ERROR("Cannot unlock the ui");
    return false;
  }

  return true;
}

bool Canon::canStartLiveView() {
  if(!getDevice()) {
    RX_ERROR("No device found");
    return false;
  }
  return canon_can_start_live_view((EdsAEMode)getDevice()->getAEMode());
}

bool Canon::startLiveView() {
  if(!canStartLiveView()) {
    RX_ERROR("The current (dial knob) AEMode cannot be used for live view");
    return false;
  }

  CanonTaskEvfStart* task = new CanonTaskEvfStart(this);
  if(!task) {
    RX_ERROR("Cannot allocate CanonTaskEvfStart");
    return false;
  }

  queue.addTask(task);
  return true;
}

bool Canon::endLiveView() {
  if(state != STATE_LIVE_VIEW) {
    RX_ERROR("Cannot end the live view when it's not started");
    return false;
  }

  CanonTaskEvfEnd* task = new CanonTaskEvfEnd(this);
  if(!task) {
    RX_ERROR("Cannot allocate CanonTaskEvfEnd");
    return false;
  }

  queue.addTask(task);
  return true;
}

bool Canon::downloadLiveView() {
  if(state != STATE_LIVE_VIEW) {
    RX_ERROR("Cannot download an live view because it looks like the live view hasn't been started yet, call startLiveView first");
    return false;
  }

  CanonTaskEvfDownload* task = new CanonTaskEvfDownload(this);
  if(!task) {
    RX_ERROR("Cannot allocate a CanonTaskEvfDownload()");
    return false;
  }

  queue.addTask(task);
  return true;
}


// Init + Shutdown : will be called by the queue thread when the thread starts and ends.
// -----------------------------------------------------------
bool Canon::initialize() {

  RX_VERBOSE("Handle initialize - add tasks that get e.g. device info?");

  if(!getCameraName()) {
    RX_ERROR("Cannot get camera name");
    return false;
  }
  if(!getAEModeSelect()) {
    RX_ERROR("Cannot get AEModeSelect");
    return false;
  }
  if(!getShutterSpeed()) {
    RX_ERROR("Cannot get shutter speed");
    return false;
  }
  if(!getAperture()) {
    RX_ERROR("Cannot get aperture");
    return false;
  }
  if(!getIso()) {
    RX_ERROR("Cannot get iso");
    return false;
  }
  if(!getMeteringMode()) {
    RX_ERROR("Cannot get metering mode");
    return false;
  }
  if(!getExposureCompensation()) {
    RX_ERROR("Cannot get exposure compensation");
    return false;
  }
  if(!getLiveViewSettings()) {
    RX_ERROR("Cannot get live view settings");
    return false;
  }
  if(!getLiveViewOutputDevice()) {
    RX_ERROR("Cannot get live view output device");
    return false;
  }
  if(!getLiveViewDepthOfFieldPreview()) {
    RX_ERROR("Cannot get depth of field preview");
    return false;
  }
  if(!getLiveViewZoom()) {
    RX_ERROR("Cannot get live view zoom");
    return false;
  }
  if(!getImageQuality()) {
    RX_ERROR("Cannot get image quality");
    return false;
  }
  if(!getLiveViewAutoFocusMode()) {
    RX_ERROR("Cannot get live view auto focus mode");
    return false;
  }
  if(!getAvailableShots()) {
    RX_ERROR("Cannot get available shots");
    return false;
  }
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

bool Canon::getAEModeSelect() {
  return getProperty(kEdsPropID_AEModeSelect);
}

bool Canon::getShutterSpeed() {
  return getProperty(kEdsPropID_Tv);
}

bool Canon::getAperture() {
  return getProperty(kEdsPropID_Av);
}

bool Canon::getIso() {
  return getProperty(kEdsPropID_ISOSpeed);
}

bool Canon::getMeteringMode() {
  return getProperty(kEdsPropID_MeteringMode);
}

bool Canon::getExposureCompensation() {
  return getProperty(kEdsPropID_ExposureCompensation);
}

bool Canon::getLiveViewSettings() {
  return getProperty(kEdsPropID_Evf_Mode);
}

bool Canon::getLiveViewOutputDevice() {
  return getProperty(kEdsPropID_Evf_OutputDevice);
}

bool Canon::getLiveViewDepthOfFieldPreview() {
  return getProperty(kEdsPropID_Evf_DepthOfFieldPreview);
}

bool Canon::getImageQuality() {
  return getProperty(kEdsPropID_ImageQuality);
}

bool Canon::getLiveViewAutoFocusMode() {
  return getProperty(kEdsPropID_Evf_AFMode);
}

bool Canon::getLiveViewZoom() {
  return getProperty(kEdsPropID_Evf_Zoom);
}

bool Canon::getAvailableShots() {
  return getProperty(kEdsPropID_AvailableShots);
}

bool Canon::getProperty(EdsPropertyID prop) {
  if(!getCameraRef()) {
    RX_ERROR("Cannot get property because the camera ref is invalid");
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


// CAPABILITIES
std::vector<AVCapability> Canon::getCapabilities(int device) {
  std::vector<AVCapability> caps;

  AVCapability cap;
  cap.size.width = 928;
  cap.size.height = 616;
  cap.pixel_format = AV_PIX_FMT_RGB24;
  cap.framerate.num = 1;
  cap.framerate.den = 30;

  caps.push_back(cap);

  return caps;
  //  cap.size = 
}
