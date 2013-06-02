#include <videocapture/edsdk/Canon.h>
#include <videocapture/edsdk/CanonUtils.h>
#include <videocapture/edsdk/CanonTaskProperty.h>
#include <roxlu/core/Log.h>

CanonTaskProperty::CanonTaskProperty(Canon* canon, EdsPropertyID propertyID)
  :CanonTask(canon, CANON_TASK_GET_PROPERTY)
  ,property_id(propertyID)
{
  
}

CanonTaskProperty::~CanonTaskProperty() {
}

bool CanonTaskProperty::execute() {
  if(canon->isLegacy()) {
    canon->lockUI();
  }

  getProperty(property_id);

  if(canon->isLegacy()) {
    canon->unlockUI();
  }

  return true;
}



bool CanonTaskProperty::getProperty(EdsPropertyID prop) {
  EdsError err = EDS_ERR_OK;
  EdsDataType data_type = kEdsDataType_Unknown;
  EdsUInt32 data_size = 0;
  EdsUInt32 uint_data = 0;
  EdsChar string_data[EDS_MAX_NAME];
  
  if(prop == kEdsPropID_Unknown) {
    RX_VERBOSE("Unknown property");
    return false;
  }

  err = EdsGetPropertySize(canon->getCameraRef(), prop, 0, &data_type, &data_size);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    return false;
  }

  std::string dt = canon_data_type_to_string(data_type);

  switch(data_type) {

    case kEdsDataType_UInt32: {
      err = EdsGetPropertyData(canon->getCameraRef(), prop, 0, data_size, &uint_data);
      if(err != EDS_ERR_OK) {
        CANON_ERROR(err);
        return false;
      }
      if(!canon->getDevice()->setProperty(prop, uint_data)) {
        return false;
      }
      break;
    }

    case kEdsDataType_String: {
      err = EdsGetPropertyData(canon->getCameraRef(), prop, 0, data_size, &string_data);
      if(err != EDS_ERR_OK) {
        CANON_ERROR(err);
        return false;
      }
      if(!canon->getDevice()->setProperty(prop, string_data)) {
        return false;
      }
      break;
    }

    case kEdsDataType_FocusInfo: {
      break;
    }

    default: {
      RX_ERROR("Unhandled data type for property");
      return false;
    }
      
  }
  return err == EDS_ERR_OK;

}
