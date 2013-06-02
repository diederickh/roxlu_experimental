#include <videocapture/edsdk/CanonTypes.h>
#include <videocapture/edsdk/Canon.h>

// ----------------------------------------

CanonTask::CanonTask(Canon* canon, CanonTaskType type) 
  :canon(canon)
  ,type(type)
  ,must_retry(false)
{
}

CanonTask::~CanonTask() {
}


// ----------------------------------------
CanonEvent::CanonEvent(CanonEventType type, void* data)
  :type(type)
  ,data(data)
{
}

CanonLiveViewData::CanonLiveViewData()
  :stream_ref(NULL)
{
}

// ----------------------------------------
CanonDevice::CanonDevice(Canon* canon)
  :ae_mode(0)
  ,av(0)
  ,iso(0)
  ,metering_mode(0)
  ,exposure_compensation(0)
  ,shutter_speed(0)
  ,image_quality(0)
  ,available_shots(0)
  ,evf_mode(0)
  ,evf_output_device(0)
  ,evf_depth_of_field_preview(0)
  ,evf_zoom(0)
  ,evf_af_mode(0)
  ,canon(canon)
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
    case kEdsPropID_AEModeSelect:            { ae_mode = data;                      break;  }
    case kEdsPropID_Tv:                      { shutter_speed = data;                break;  }
    case kEdsPropID_Av:                      { av = data;                           break;  }
    case kEdsPropID_ISOSpeed:                { iso = data;                          break;  }
    case kEdsPropID_MeteringMode:            { metering_mode = data;                break;  }
    case kEdsPropID_ExposureCompensation:    { exposure_compensation = data;        break;  }
    case kEdsPropID_Evf_Mode:                { evf_mode = data;                     break;  }
    case kEdsPropID_Evf_OutputDevice:        { evf_output_device = data;            break;  } 
    case kEdsPropID_Evf_DepthOfFieldPreview: { evf_depth_of_field_preview = data;   break;  }
    case kEdsPropID_ImageQuality:            { image_quality = data;                break;  }
    case kEdsPropID_Evf_AFMode:              { evf_af_mode = data;                  break;  }
    case kEdsPropID_Evf_Zoom:                { evf_zoom = data;                     break;  }
    case kEdsPropID_AvailableShots:          { available_shots = data;              break;  }
    default: { 
      RX_ERROR("Unhandled uint32 property for the canon device");
      return false;
      break;
    }
  };

  if(prop == kEdsPropID_Evf_OutputDevice) { 
    if(isUsingPCForLiveView()) {
       CanonEvent ev(CANON_EVENT_EVF_STARTED, NULL);
       canon->fireEvent(ev);
    }
  }

  return true;
}

bool CanonDevice::setModelName(EdsChar* name) {
  if(strlen(name) > EDS_MAX_NAME) {
    RX_ERROR("Name is too big");
    return false;
  }
  sprintf(model_name, "%s", name);
  return true;
}


void CanonDevice::print() {
  RX_VERBOSE("ae_mode: %s", canon_ae_mode_to_string((EdsAEMode)ae_mode).c_str());
  RX_VERBOSE("shutter_speed: %02X", shutter_speed);
  RX_VERBOSE("iso: %08X", iso);
  RX_VERBOSE("metering_mode: %s", canon_metering_mode_to_string(metering_mode).c_str());
  RX_VERBOSE("exposure_compensation: %02X", exposure_compensation);
  RX_VERBOSE("evf_mode: %s", (evf_mode == 0) ? "disabled" : "enabled");
  RX_VERBOSE("evf_output_device: %s", canon_evf_output_device_to_string(evf_output_device).c_str());
  RX_VERBOSE("evf_depth_of_field_preview: %d", evf_depth_of_field_preview);
  RX_VERBOSE("evf_af_mode: %s", canon_evf_af_mode_to_string(evf_af_mode).c_str());
  RX_VERBOSE("evf_zoom: %s", canon_evf_zoom_to_string(evf_zoom).c_str());
  RX_VERBOSE("image_quality: %04X", image_quality);
  RX_VERBOSE("available_shots: %d", available_shots);
}
