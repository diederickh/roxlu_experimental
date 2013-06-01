#ifndef ROXLU_CANON_TYPES_H
#define ROXLU_CANON_TYPES_H

#include <roxlu/core/Log.h>
#include <string>
#include <string.h>
#include <EDSDK.h>
#include <EDSDKErrors.h>
#include <EDSDKTypes.h>
#include <videocapture/edsdk/CanonUtils.h>

#define CANON_ERROR(errcode) { RX_ERROR("Error with EDSK: %s", canon_error_to_string(errcode).c_str()); }

class Canon;

// Each task has a type flag 
enum CanonTaskType {
  CANON_TASK_NONE,
  CANON_TASK_OPEN_SESSION,
  CANON_TASK_CLOSE_SESSION,
  CANON_TASK_TAKE_PICTURE,
  CANON_TASK_DOWNLOAD,
  CANON_TASK_GET_PROPERTY,
  CANON_TASK_EVF_START,
  CANON_TASK_EVF_END
};

// Tasks are execute asynchronously - this is a base type for tasks
class CanonTask {
 public:
  CanonTask(Canon* canon, CanonTaskType type = CANON_TASK_NONE);
  virtual ~CanonTask();

  virtual bool execute() = 0;

  bool lockUI();                                                        /* for cameras earlier than the 30D , the UI must be locked before commands are reissued */
  bool unlockUI();                                                      /* for cameras earlier than the 30D , the UI must be locked before commands are reissued */


 public:
  Canon* canon;
  CanonTaskType type; 
};

// A canon camera - used to keep track of device properties
struct CanonDevice {
  CanonDevice();

  bool setProperty(EdsPropertyID prop, EdsChar* value);
  bool setProperty(EdsPropertyID prop, EdsUInt32 data);
  bool setModelName(EdsChar* name);
  void print();

  EdsCameraRef camera_ref;
  EdsChar model_name[EDS_MAX_NAME];

  EdsUInt32 getAEMode();
  EdsUInt32 getAV();
  EdsUInt32 getISO();
  EdsUInt32 getMeteringMode();
  EdsUInt32 getExposureCompensation();
  EdsUInt32 getShutterSpeed();
  EdsUInt32 getImageQuality();
  EdsUInt32 getAvailableShots();
  EdsUInt32 getEvfMode();
  EdsUInt32 getEvfOutputDevice();
  EdsUInt32 getEvfDepthOfFieldPreview();
  EdsUInt32 getEvfZoom();
  EdsUInt32 getEvfZoomPosition();
  EdsUInt32 getEvfAFMode();

  // Taking picture parameters
  EdsUInt32 ae_mode;
  EdsUInt32 av;
  EdsUInt32 iso;
  EdsUInt32 metering_mode;
  EdsUInt32 exposure_compensation;
  EdsUInt32 shutter_speed;  
  EdsUInt32 image_quality;
  EdsUInt32 available_shots;
  EdsUInt32 evf_mode;
  EdsUInt32 evf_output_device;
  EdsUInt32 evf_depth_of_field_preview;
  EdsUInt32 evf_zoom;   
  EdsUInt32 evf_af_mode;
};

inline  EdsUInt32 CanonDevice::getAEMode() {
  return ae_mode;
}

inline  EdsUInt32 CanonDevice::getAV() {
  return av;
}

inline  EdsUInt32 CanonDevice::getISO() {
  return iso;
}

inline  EdsUInt32 CanonDevice::getMeteringMode() {
  return metering_mode;
}

inline  EdsUInt32 CanonDevice::getExposureCompensation() {
  return exposure_compensation;
}

inline  EdsUInt32 CanonDevice::getShutterSpeed() {
  return shutter_speed;
}

inline  EdsUInt32 CanonDevice::getImageQuality() {
  return image_quality;
}

inline  EdsUInt32 CanonDevice::getAvailableShots() {
  return available_shots;
}

inline  EdsUInt32 CanonDevice::getEvfMode() {
  return evf_mode;
}

inline  EdsUInt32 CanonDevice::getEvfOutputDevice() {
  return evf_output_device;
}

inline  EdsUInt32 CanonDevice::getEvfDepthOfFieldPreview() {
  return evf_depth_of_field_preview;
}

inline  EdsUInt32 CanonDevice::getEvfAFMode() {
  return evf_af_mode;
}


#endif
