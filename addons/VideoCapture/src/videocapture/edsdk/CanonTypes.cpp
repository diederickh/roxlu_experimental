#include <videocapture/edsdk/CanonTypes.h>

// ----------------------------------------

CanonTask::CanonTask(Canon* canon, CanonTaskType type) 
  :canon(canon)
  ,type(type)
{
}

CanonTask::~CanonTask() {
}

// ----------------------------------------
CanonDevice::CanonDevice()
  :ae_mode(0)
  ,av(0)
  ,iso(0)
  ,metering_mode(0)
  ,exposure_compensation(0)
  ,image_quality(0)
  ,available_shot(0)
  ,evf_mode(0)
  ,evf_output_device(0)
  ,evf_depth_of_field_preview(0)
  ,evf_zoom(0)
  ,evf_zoom_position(0)
  ,evf_af_mode(0)
{
}

bool CanonDevice::setProperty(EdsPropertyID prop, EdsChar* value) {
  switch(prop) {
    case kEdsPropID_ProductName: {
      return setModelName(value);
      break;
    }
    default: {
      RX_ERROR("Unhandled property for the canon device");
      return false;
      break;
    }
  };
  return false;
}


bool CanonDevice::setProperty(EdsPropertyID prop, EdsUInt32 data) {
  switch(prop) {
    
    default: { 
      RX_ERROR("Unhandled uint32 property for the canon device");
      return false;
      break;
    }
  };
}

bool CanonDevice::setModelName(EdsChar* name) {
  if(strlen(name) > EDS_MAX_NAME) {
    RX_ERROR("Name is too big");
    return false;
  }
  sprintf(model_name, "%s", name);
  return true;
}
