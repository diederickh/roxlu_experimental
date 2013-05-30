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
  CANON_TASK_GET_PROPERTY
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

  EdsCameraRef camera_ref;
  EdsChar model_name[EDS_MAX_NAME];

  // Taking picture parameters
  EdsUInt32 ae_mode;
  EdsUInt32 av;
  EdsUInt32 iso;
  EdsUInt32 metering_mode;
  EdsUInt32 exposure_compensation;
  EdsUInt32 image_quality;
  EdsUInt32 available_shot;
  EdsUInt32 evf_mode;
  EdsUInt32 evf_output_device;
  EdsUInt32 evf_depth_of_field_preview;
  EdsUInt32 evf_zoom;
  EdsUInt32 evf_zoom_position;
  EdsUInt32 evf_af_mode;
  EdsFocusInfo focus_info;

  // List of values in which taking a picture parameter can be
  EdsPropertyDesc ae_mode_desc;
  EdsPropertyDesc av_desc;
  EdsPropertyDesc tv_desc;
  EdsPropertyDesc iso_desc;
  EdsPropertyDesc metering_mode_desc;
  EdsPropertyDesc exposure_compensation_desc;
  EdsPropertyDesc image_quality_desc;
  EdsPropertyDesc evf_af_mode_desc;

};

#endif
