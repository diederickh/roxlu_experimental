#include <videocapture/edsdk/CanonTypes.h>
#include <videocapture/edsdk/CanonUtils.h>

// The live view can only be started when the EdsAEMode has a certain value
bool canon_can_start_live_view(EdsAEMode m) {
  if(m == kEdsAEMode_Movie 
     || m == kEdsAEMode_Green
     || m == kEdsAEMode_Program
     || m == kEdsAEMode_Tv
     || m == kEdsAEMode_Av
     || m == kEdsAEMode_Manual
     || m == kEdsAEMode_A_DEP)
    {
      return true;
    }
  return false;
}

std::string canon_data_type_to_string(EdsDataType dt) {
  switch(dt) {
    case kEdsDataType_Unknown: return "kEdsDataType_Unknown"; break;
    case kEdsDataType_Bool: return "kEdsDataType_Bool"; break;
    case kEdsDataType_String: return "kEdsDataType_String"; break;
    case kEdsDataType_Int8: return "kEdsDataType_Int8"; break;
    case kEdsDataType_UInt8: return "kEdsDataType_UInt8"; break;
    case kEdsDataType_Int16: return "kEdsDataType_Int16"; break;
    case kEdsDataType_UInt16: return "kEdsDataType_UInt16"; break;
    case kEdsDataType_Int32: return "kEdsDataType_Int32"; break;
    case kEdsDataType_UInt32: return "kEdsDataType_UInt32"; break;
    case kEdsDataType_Int64: return "kEdsDataType_Int64"; break;
    case kEdsDataType_UInt64: return "kEdsDataType_UInt64"; break;
    case kEdsDataType_Float: return "kEdsDataType_Float"; break;
    case kEdsDataType_Double: return "kEdsDataType_Double"; break;
    case kEdsDataType_ByteBlock: return "kEdsDataType_ByteBlock"; break;
    case kEdsDataType_Rational: return "kEdsDataType_Rational"; break;
    case kEdsDataType_Point: return "kEdsDataType_Point"; break;
    case kEdsDataType_Rect: return "kEdsDataType_Rect"; break;
    case kEdsDataType_Time: return "kEdsDataType_Time"; break;
    case kEdsDataType_Bool_Array: return "kEdsDataType_Bool_Array"; break;
    case kEdsDataType_Int8_Array: return "kEdsDataType_Int8_Array"; break;
    case kEdsDataType_Int16_Array: return "kEdsDataType_Int16_Array"; break;
    case kEdsDataType_Int32_Array: return "kEdsDataType_Int32_Array"; break;
    case kEdsDataType_UInt8_Array: return "kEdsDataType_UInt8_Array"; break;
    case kEdsDataType_UInt16_Array: return "kEdsDataType_UInt16_Array"; break;
    case kEdsDataType_UInt32_Array: return "kEdsDataType_UInt32_Array"; break;
    case kEdsDataType_Rational_Array: return "kEdsDataType_Rational_Array"; break;
    case kEdsDataType_FocusInfo: return "kEdsDataType_FocusInfo"; break;
    case kEdsDataType_PictureStyleDesc: return "kEdsDataType_PictureStyleDesc"; break;
    default: return "UNKNOWN TYPE"; break;
  }
}

std::string canon_error_to_string(EdsError err) {
  switch(err) {
    // General errors
    case EDS_ERR_UNIMPLEMENTED: return "Not implemented"; break;
    case EDS_ERR_INTERNAL_ERROR: return "Internal error"; break;
    case EDS_ERR_MEM_ALLOC_FAILED: return "Memory allocation error"; break;
    case EDS_ERR_MEM_FREE_FAILED: return "Memory release error"; break;
    case EDS_ERR_OPERATION_CANCELLED: return "Operation canceled"; break;
    case EDS_ERR_INCOMPATIBLE_VERSION: return "Version error"; break;
    case EDS_ERR_NOT_SUPPORTED: return "Not supported"; break;
    case EDS_ERR_UNEXPECTED_EXCEPTION: return "Unexpected exception"; break;
    case EDS_ERR_PROTECTION_VIOLATION: return "Protection violation"; break;
    case EDS_ERR_MISSING_SUBCOMPONENT: return "Missing subcomponent"; break;
    case EDS_ERR_SELECTION_UNAVAILABLE: return "Selection unavailable"; break;

      // File access errors
    case EDS_ERR_FILE_IO_ERROR: return "IO error"; break;
    case EDS_ERR_FILE_TOO_MANY_OPEN: return "Too many files open"; break;
    case EDS_ERR_FILE_NOT_FOUND: return "File does not exist"; break;
    case EDS_ERR_FILE_OPEN_ERROR: return "Open error"; break;
    case EDS_ERR_FILE_CLOSE_ERROR: return "Close error"; break;
    case EDS_ERR_FILE_SEEK_ERROR: return "Seek error"; break;
    case EDS_ERR_FILE_TELL_ERROR: return "Tell error"; break;
    case EDS_ERR_FILE_READ_ERROR: return "Read error"; break;
    case EDS_ERR_FILE_WRITE_ERROR: return "Write error"; break;
    case EDS_ERR_FILE_PERMISSION_ERROR: return "Permission error"; break;
    case EDS_ERR_FILE_DISK_FULL_ERROR: return "Disk full"; break;
    case EDS_ERR_FILE_ALREADY_EXISTS: return "File already exists"; break;
    case EDS_ERR_FILE_FORMAT_UNRECOGNIZED: return "Format error"; break;
    case EDS_ERR_FILE_DATA_CORRUPT: return "Invalid data"; break;
    case EDS_ERR_FILE_NAMING_NA: return "File naming error"; break;

      // Directory errors
    case EDS_ERR_DIR_NOT_FOUND: return "￼Directory does not exist"; break;
    case EDS_ERR_DIR_IO_ERROR: return "I/O error"; break;
    case EDS_ERR_DIR_ENTRY_NOT_FOUND: return "No file in directory"; break;
    case EDS_ERR_DIR_ENTRY_EXISTS: return "File in directory"; break;
    case EDS_ERR_DIR_NOT_EMPTY: return "Directory full"; break;
    case EDS_ERR_PROPERTIES_UNAVAILABLE: return "Property (and additional property information) unavailable"; break;
    case EDS_ERR_PROPERTIES_MISMATCH: return "Property mismatch"; break;
    case EDS_ERR_PROPERTIES_NOT_LOADED: return "Property not loaded"; break;

      // Function parameter errors
    case EDS_ERR_INVALID_PARAMETER: return "Invalid function parameter"; break;
    case EDS_ERR_INVALID_HANDLE: return "Handle error"; break;
    case EDS_ERR_INVALID_POINTER: return "Pointer error"; break;
    case EDS_ERR_INVALID_INDEX: return "Index error"; break;
    case EDS_ERR_INVALID_LENGTH: return "Length error"; break;
    case EDS_ERR_INVALID_FN_POINTER: return "FN pointer error"; break;
    case EDS_ERR_INVALID_SORT_FN: return "Sort FN error"; break;

      // Device errors
    case EDS_ERR_DEVICE_NOT_FOUND: return "Device not found"; break;
    case EDS_ERR_DEVICE_BUSY: return "Device busy Note: If a device busy error occurs, reissue the command after a while. The camera will become unstable."; break;
    case EDS_ERR_DEVICE_INVALID: return "Device error"; break;
    case EDS_ERR_DEVICE_EMERGENCY: return "Device emergency"; break;
    case EDS_ERR_DEVICE_MEMORY_FULL: return "Device memory full"; break;
    case EDS_ERR_DEVICE_INTERNAL_ERROR: return "Internal device error"; break;
    case EDS_ERR_DEVICE_INVALID_PARAMETER: return "Device parameter invalid"; break;
    case EDS_ERR_DEVICE_NO_DISK: return "No disk"; break;
    case EDS_ERR_DEVICE_DISK_ERROR: return "Disk error"; break;
    case EDS_ERR_DEVICE_CF_GATE_CHANGED: return "The CF gate has been changed"; break;
    case EDS_ERR_DEVICE_DIAL_CHANGED: return "The dial has been changed"; break;
    case EDS_ERR_DEVICE_NOT_INSTALLED: return "Device not installed"; break;
    case EDS_ERR_DEVICE_STAY_AWAKE: return "Device connected in awake mode"; break;
    case EDS_ERR_DEVICE_NOT_RELEASED: return "Device not released"; break;

      // Stream errors
    case EDS_ERR_STREAM_IO_ERROR: return "Stream I/O error"; break;
    case EDS_ERR_STREAM_NOT_OPEN: return "Stream open error"; break;
    case EDS_ERR_STREAM_ALREADY_OPEN: return "Stream already open"; break;
    case EDS_ERR_STREAM_OPEN_ERROR: return "Failed to open stream"; break;
    case EDS_ERR_STREAM_CLOSE_ERROR: return "Failed to close stream"; break;
    case EDS_ERR_STREAM_SEEK_ERROR: return "Stream seek error"; break;
    case EDS_ERR_STREAM_TELL_ERROR: return "Stream tell error"; break;
    case EDS_ERR_STREAM_READ_ERROR: return "Failed to read stream"; break;
    case EDS_ERR_STREAM_WRITE_ERROR: return "Failed to write stream"; break;
    case EDS_ERR_STREAM_PERMISSION_ERROR: return "Permission error"; break;
    case EDS_ERR_STREAM_COULDNT_BEGIN_THREAD: return "Could not start reading thumbnail"; break;
    case EDS_ERR_STREAM_BAD_OPTIONS: return "Invalid stream option"; break;
    case EDS_ERR_STREAM_END_OF_STREAM: return "Invalid stream termination"; break;

      // Communication errors
    case EDS_ERR_COMM_PORT_IS_IN_USE: return "Port in use"; break;
    case EDS_ERR_COMM_DISCONNECTED: return "Port disconnected"; break;
    case EDS_ERR_COMM_DEVICE_INCOMPATIBLE: return "Incompatible device"; break;
    case EDS_ERR_COMM_BUFFER_FULL: return "Buffer full"; break;
    case EDS_ERR_COMM_USB_BUS_ERR: return "USB bus error"; break;

      // Camera lock ui errors
    case EDS_ERR_USB_DEVICE_LOCK_ERROR: return "Failed to lock the UI"; break;
    case EDS_ERR_USB_DEVICE_UNLOCK_ERROR: return "Failed to unlock the UI"; break;

      // STI - WIA errors
    case EDS_ERR_STI_UNKNOWN_ERROR: return "Unknown STI"; break;
    case EDS_ERR_STI_INTERNAL_ERROR: return "Internal STI error"; break;
    case EDS_ERR_STI_DEVICE_CREATE_ERROR: return "Device creation error"; break;
    case EDS_ERR_STI_DEVICE_RELEASE_ERROR: return "Device release error"; break;
    case EDS_ERR_DEVICE_NOT_LAUNCHED: return "Device startup failed"; break;

      // Other general errors
    case EDS_ERR_ENUM_NA: return "Enumeration terminated (there was no suitable enumeration item)"; break;
    case EDS_ERR_INVALID_FN_CALL: return "Called in a mode when the function could not be used"; break;
    case EDS_ERR_HANDLE_NOT_FOUND: return "Handle not found"; break;
    case EDS_ERR_INVALID_ID: return "Invalid ID"; break;
    case EDS_ERR_WAIT_TIMEOUT_ERROR: return "Timeout"; break;
    case EDS_ERR_LAST_GENERIC_ERROR_PLUS_ONE: return "Not used."; break;

      // PTP errors
    case EDS_ERR_SESSION_NOT_OPEN: return "Session open error"; break;
    case EDS_ERR_INVALID_TRANSACTIONID: return "Invalid transaction ID"; break;
    case EDS_ERR_INCOMPLETE_TRANSFER: return "Transfer problem"; break;
    case EDS_ERR_INVALID_STRAGEID: return "Storage error"; break;
    case EDS_ERR_DEVICEPROP_NOT_SUPPORTED: return "Unsupported device property"; break;
    case EDS_ERR_INVALID_OBJECTFORMATCODE: return "Invalid object format code"; break;
    case EDS_ERR_SELF_TEST_FAILED: return "Failed self-diagnosis"; break;
    case EDS_ERR_PARTIAL_DELETION: return "Failed in partial deletion"; break;
    case EDS_ERR_SPECIFICATION_BY_FORMAT_UNSUPPORTED: return "Unsupported format specification"; break;
    case EDS_ERR_NO_VALID_OBJECTINFO: return "Invalid object information"; break;
    case EDS_ERR_INVALID_CODE_FORMAT: return "Invalid code format"; break;
    // case EDS_ERR_UNKNOWN_VENDER_CODE: return "Unknown vendor code"; break; // not defined in headers
    case EDS_ERR_CAPTURE_ALREADY_TERMINATED: return "Capture already terminated"; break;
    case EDS_ERR_INVALID_PARENTOBJECT: return "Invalid parent object"; break;
    case EDS_ERR_INVALID_DEVICEPROP_FORMAT: return "Invalid property format"; break;
    case EDS_ERR_INVALID_DEVICEPROP_VALUE: return "Invalid property value"; break;
    case EDS_ERR_SESSION_ALREADY_OPEN: return "Session already open"; break;
    case EDS_ERR_TRANSACTION_CANCELLED: return "Transaction canceled"; break;
    case EDS_ERR_SPECIFICATION_OF_DESTINATION_UNSUPPORTED: return "Unsupported destination specification"; break;
    case EDS_ERR_UNKNOWN_COMMAND: return "Unknown command"; break;
    case EDS_ERR_OPERATION_REFUSED: return "Operation refused"; break;
    case EDS_ERR_LENS_COVER_CLOSE: return "Lens cover closed"; break;
    case EDS_ERR_OBJECT_NOTREADY: return "Image data set not ready for live view"; break;

      // Take picture errors
    case EDS_ERR_TAKE_PICTURE_AF_NG: return "Focus failed"; break;
    case EDS_ERR_TAKE_PICTURE_RESERVED: return "Reserved"; break;
    case EDS_ERR_TAKE_PICTURE_MIRROR_UP_NG: return "Currently configuring mirror up"; break;
    case EDS_ERR_TAKE_PICTURE_SENSOR_CLEANING_NG: return "Currently cleaning sensor"; break;
    case EDS_ERR_TAKE_PICTURE_SILENCE_NG: return "Currently performing silent operations"; break;
    case EDS_ERR_TAKE_PICTURE_NO_CARD_NG: return "Card not installed"; break;
    case EDS_ERR_TAKE_PICTURE_CARD_NG: return "Error writing to card"; break;
    case EDS_ERR_TAKE_PICTURE_CARD_PROTECT_NG: return "Card write protected"; break;

    default: return "UNKNOWN ERROR"; break;

  }
  return "unknown";
}

std::string canon_event_to_string(EdsUInt32 ev) {
  switch(ev) {
    case kEdsStateEvent_Shutdown: return "kEdsStateEvent_Shutdown"; break;
    case kEdsPropertyEvent_PropertyChanged: return "kEdsPropertyEvent_PropertyChanged"; break;;
    case kEdsPropertyEvent_PropertyDescChanged: return "kEdsPropertyEvent_PropertyDescChanged"; break;
    case kEdsObjectEvent_DirItemCreated: return "kEdsObjectEvent_DirItemCreated"; break;
    case kEdsObjectEvent_DirItemRemoved: return "kEdsObjectEvent_DirItemRemoved"; break;
    case kEdsObjectEvent_DirItemInfoChanged: return "kEdsObjectEvent_DirItemInfoChanged"; break;
    case kEdsObjectEvent_DirItemContentChanged: return "kEdsObjectEvent_DirItemContentChanged"; break;
    case kEdsObjectEvent_VolumeInfoChanged: return "kEdsObjectEvent_VolumeInfoChanged"; break;
    case kEdsObjectEvent_VolumeUpdateItems: return "kEdsObjectEvent_VolumeUpdateItems"; break;
    case kEdsObjectEvent_FolderUpdateItems: return "kEdsObjectEvent_FolderUpdateItems"; break;
    case kEdsStateEvent_JobStatusChanged: return "kEdsStateEvent_JobStatusChanged"; break;
    case kEdsObjectEvent_DirItemRequestTransfer: return "kEdsObjectEvent_DirItemRequestTransfer"; break;
    case kEdsObjectEvent_DirItemRequestTransferDT: return "kEdsObjectEvent_DirItemRequestTransferDT"; break;
    case kEdsObjectEvent_DirItemCancelTransferDT: return "kEdsObjectEvent_DirItemCancelTransferDT"; break;
    case kEdsStateEvent_WillSoonShutDown: return "kEdsStateEvent_WillSoonShutDown"; break;
    case kEdsStateEvent_ShutDownTimerUpdate: return "kEdsStateEvent_ShutDownTimerUpdate"; break;
    case kEdsStateEvent_CaptureError: return "kEdsStateEvent_CaptureError"; break;
    case kEdsStateEvent_BulbExposureTime: return "kEdsStateEvent_BulbExposureTime"; break;
    case kEdsStateEvent_InternalError: return "kEdsStateEvent_InternalError"; break;
    default: return "UNKNOWN EVENT"; break;
  }
}

std::string canon_property_to_string(EdsUInt32 prop) {
  switch(prop) {

    case kEdsPropID_Unknown: return "kEdsPropID_Unknown"; break;

    case kEdsPropID_ProductName: return "kEdsPropID_ProductName"; break;
    case kEdsPropID_OwnerName: return "kEdsPropID_OwnerName"; break;
    case kEdsPropID_MakerName: return "kEdsPropID_MakerName"; break;
    case kEdsPropID_DateTime: return "kEdsPropID_DateTime"; break;
    case kEdsPropID_FirmwareVersion: return "kEdsPropID_FirmwareVersion"; break;
    case kEdsPropID_BatteryLevel: return "kEdsPropID_BatteryLevel"; break;
    case kEdsPropID_CFn: return "kEdsPropID_CFn"; break;
    case kEdsPropID_SaveTo: return "kEdsPropID_SaveTo"; break;
    case kEdsPropID_CurrentStorage: return "kEdsPropID_CurrentStorage"; break;
    case kEdsPropID_CurrentFolder: return "kEdsPropID_CurrentFolder"; break;
    case kEdsPropID_MyMenu: return "kEdsPropID_MyMenu"; break;

    case kEdsPropID_BatteryQuality: return "kEdsPropID_BatteryQuality"; break;

    case kEdsPropID_BodyIDEx: return "kEdsPropID_BodyIDEx"; break;
    case kEdsPropID_HDDirectoryStructure: return "kEdsPropID_HDDirectoryStructure"; break;

      /*----------------------------------
        Image Properties
        ----------------------------------*/
    case kEdsPropID_ImageQuality: return "kEdsPropID_ImageQuality"; break;
    case kEdsPropID_JpegQuality: return "kEdsPropID_JpegQuality"; break;
    case kEdsPropID_Orientation: return "kEdsPropID_Orientation"; break;
    case kEdsPropID_ICCProfile: return "kEdsPropID_ICCProfile"; break;
    case kEdsPropID_FocusInfo: return "kEdsPropID_FocusInfo"; break;
    case kEdsPropID_DigitalExposure: return "kEdsPropID_DigitalExposure"; break;
    case kEdsPropID_WhiteBalance: return "kEdsPropID_WhiteBalance"; break;
    case kEdsPropID_ColorTemperature: return "kEdsPropID_ColorTemperature"; break;
    case kEdsPropID_WhiteBalanceShift: return "kEdsPropID_WhiteBalanceShift"; break;
    case kEdsPropID_Contrast: return "kEdsPropID_Contrast"; break;
    case kEdsPropID_ColorSaturation: return "kEdsPropID_ColorSaturation"; break;
    case kEdsPropID_ColorTone: return "kEdsPropID_ColorTone"; break;
    case kEdsPropID_Sharpness: return "kEdsPropID_Sharpness"; break;
    case kEdsPropID_ColorSpace: return "kEdsPropID_ColorSpace"; break;
    case kEdsPropID_ToneCurve: return "kEdsPropID_ToneCurve"; break;
    case kEdsPropID_PhotoEffect: return "kEdsPropID_PhotoEffect"; break;
    case kEdsPropID_FilterEffect: return "kEdsPropID_FilterEffect"; break;
    case kEdsPropID_ToningEffect: return "kEdsPropID_ToningEffect"; break;
    case kEdsPropID_ParameterSet: return "kEdsPropID_ParameterSet"; break;
    case kEdsPropID_ColorMatrix: return "kEdsPropID_ColorMatrix"; break;
    case kEdsPropID_PictureStyle: return "kEdsPropID_PictureStyle"; break;
    case kEdsPropID_PictureStyleDesc: return "kEdsPropID_PictureStyleDesc"; break;
    case kEdsPropID_PictureStyleCaption: return "kEdsPropID_PictureStyleCaption"; break;

      /*----------------------------------
        Image Processing Properties
        ----------------------------------*/
    case kEdsPropID_Linear: return "kEdsPropID_Linear"; break;
    case kEdsPropID_ClickWBPoint: return "kEdsPropID_ClickWBPoint"; break;
    case kEdsPropID_WBCoeffs: return "kEdsPropID_WBCoeffs"; break;


      /*----------------------------------
        Image GPS Properties
        ----------------------------------*/
    case kEdsPropID_GPSVersionID: return "kEdsPropID_GPSVersionID"; break;
    case kEdsPropID_GPSLatitudeRef: return "kEdsPropID_GPSLatitudeRef"; break;
    case kEdsPropID_GPSLatitude: return "kEdsPropID_GPSLatitude"; break;
    case kEdsPropID_GPSLongitudeRef: return "kEdsPropID_GPSLongitudeRef"; break;
    case kEdsPropID_GPSLongitude: return "kEdsPropID_GPSLongitude"; break;
    case kEdsPropID_GPSAltitudeRef: return "kEdsPropID_GPSAltitudeRef"; break;
    case kEdsPropID_GPSAltitude: return "kEdsPropID_GPSAltitude"; break;
    case kEdsPropID_GPSTimeStamp: return "kEdsPropID_GPSTimeStamp"; break;
    case kEdsPropID_GPSSatellites: return "kEdsPropID_GPSSatellites"; break;
    case kEdsPropID_GPSStatus: return "kEdsPropID_GPSStatus"; break;
    case kEdsPropID_GPSMapDatum: return "kEdsPropID_GPSMapDatum"; break;
    case kEdsPropID_GPSDateStamp: return "kEdsPropID_GPSDateStamp"; break;

      /*----------------------------------
        Property Mask
        ----------------------------------*/
    case kEdsPropID_AtCapture_Flag: return "kEdsPropID_AtCapture_Flag"; break;

      /*----------------------------------
        Capture Properties
        ----------------------------------*/
    case kEdsPropID_AEMode: return "kEdsPropID_AEMode"; break;
    case kEdsPropID_DriveMode: return "kEdsPropID_DriveMode"; break;
    case kEdsPropID_ISOSpeed: return "kEdsPropID_ISOSpeed"; break;
    case kEdsPropID_MeteringMode: return "kEdsPropID_MeteringMode"; break;
    case kEdsPropID_AFMode: return "kEdsPropID_AFMode"; break;
    case kEdsPropID_Av: return "kEdsPropID_Av"; break;
    case kEdsPropID_Tv: return "kEdsPropID_Tv"; break;
    case kEdsPropID_ExposureCompensation: return "kEdsPropID_ExposureCompensation"; break;
    case kEdsPropID_FlashCompensation: return "kEdsPropID_FlashCompensation"; break;
    case kEdsPropID_FocalLength: return "kEdsPropID_FocalLength"; break;
    case kEdsPropID_AvailableShots: return "kEdsPropID_AvailableShots"; break;
    case kEdsPropID_Bracket: return "kEdsPropID_Bracket"; break;
    case kEdsPropID_WhiteBalanceBracket: return "kEdsPropID_WhiteBalanceBracket"; break;
    case kEdsPropID_LensName: return "kEdsPropID_LensName"; break;
    case kEdsPropID_AEBracket: return "kEdsPropID_AEBracket"; break;
    case kEdsPropID_FEBracket: return "kEdsPropID_FEBracket"; break;
    case kEdsPropID_ISOBracket: return "kEdsPropID_ISOBracket"; break;
    case kEdsPropID_NoiseReduction: return "kEdsPropID_NoiseReduction"; break;
    case kEdsPropID_FlashOn: return "kEdsPropID_FlashOn"; break;
    case kEdsPropID_RedEye: return "kEdsPropID_RedEye"; break;
    case kEdsPropID_FlashMode: return "kEdsPropID_FlashMode"; break;
    case kEdsPropID_LensStatus: return "kEdsPropID_LensStatus"; break;
    case kEdsPropID_Artist: return "kEdsPropID_Artist"; break;
    case kEdsPropID_Copyright: return "kEdsPropID_Copyright"; break;
    case kEdsPropID_DepthOfField: return "kEdsPropID_DepthOfField"; break;
    case kEdsPropID_EFCompensation: return "kEdsPropID_EFCompensation"; break;
    case kEdsPropID_AEModeSelect: return "kEdsPropID_AEModeSelect"; break;

      /*----------------------------------
        EVF Properties
        ----------------------------------*/
    case kEdsPropID_Evf_OutputDevice: return "kEdsPropID_Evf_OutputDevice"; break;
    case kEdsPropID_Evf_Mode: return "kEdsPropID_Evf_Mode"; break;
    case kEdsPropID_Evf_WhiteBalance: return "kEdsPropID_Evf_WhiteBalance"; break;
    case kEdsPropID_Evf_ColorTemperature: return "kEdsPropID_Evf_ColorTemperature"; break;
    case kEdsPropID_Evf_DepthOfFieldPreview: return "kEdsPropID_Evf_DepthOfFieldPreview"; break;

      // EVF IMAGE DATA Properties
    case kEdsPropID_Evf_Zoom: return "kEdsPropID_Evf_Zoom"; break;
    case kEdsPropID_Evf_ZoomPosition: return "kEdsPropID_Evf_ZoomPosition"; break;
    case kEdsPropID_Evf_FocusAid: return "kEdsPropID_Evf_FocusAid"; break;
    case kEdsPropID_Evf_Histogram: return "kEdsPropID_Evf_Histogram"; break;
    case kEdsPropID_Evf_ImagePosition: return "kEdsPropID_Evf_ImagePosition"; break;
    case kEdsPropID_Evf_HistogramStatus: return "kEdsPropID_Evf_HistogramStatus"; break;
    case kEdsPropID_Evf_AFMode: return "kEdsPropID_Evf_AFMode"; break;

    case kEdsPropID_Record: return "kEdsPropID_Record"; break;

    case kEdsPropID_Evf_HistogramY: return "kEdsPropID_Evf_HistogramY"; break;
    case kEdsPropID_Evf_HistogramR: return "kEdsPropID_Evf_HistogramR"; break;
    case kEdsPropID_Evf_HistogramG: return "kEdsPropID_Evf_HistogramG"; break;
    case kEdsPropID_Evf_HistogramB: return "kEdsPropID_Evf_HistogramB"; break;

    case kEdsPropID_Evf_CoordinateSystem: return "kEdsPropID_Evf_CoordinateSystem"; break;
    case kEdsPropID_Evf_ZoomRect: return "kEdsPropID_Evf_ZoomRect"; break;
    case kEdsPropID_Evf_ImageClipRect: return "kEdsPropID_Evf_ImageClipRect"; break;
      

    default: return "UNKNOWN PROPERTY"; break;
  };
}

std::string canon_ae_mode_to_string(EdsAEMode mode) {
  switch(mode) {
    case kEdsAEMode_Program: return "kEdsAEMode_Program"; break; 
    case kEdsAEMode_Tv: return "kEdsAEMode_Tv"; break; 
    case kEdsAEMode_Av: return "kEdsAEMode_Av"; break; 
    case kEdsAEMode_Manual: return "kEdsAEMode_Manual"; break; 
    case kEdsAEMode_Bulb: return "kEdsAEMode_Bulb"; break; 
    case kEdsAEMode_A_DEP: return "kEdsAEMode_A_DEP"; break; 
    case kEdsAEMode_Custom: return "kEdsAEMode_Custom"; break; 
    case kEdsAEMode_Lock: return "kEdsAEMode_Lock"; break; 
    case kEdsAEMode_Green: return "kEdsAEMode_Green"; break; 
    case kEdsAEMode_NightPortrait: return "kEdsAEMode_NightPortrait"; break; 
    case kEdsAEMode_Sports: return "kEdsAEMode_Sports"; break; 
    case kEdsAEMode_Portrait: return "kEdsAEMode_Portrait"; break; 
    case kEdsAEMode_Landscape: return "kEdsAEMode_Landscape"; break; 
    case kEdsAEMode_Closeup: return "kEdsAEMode_Closeup"; break; 
    case kEdsAEMode_FlashOff: return "kEdsAEMode_FlashOff"; break; 
    case kEdsAEMode_CreativeAuto: return "kEdsAEMode_CreativeAuto"; break; 
    case kEdsAEMode_Movie: return "kEdsAEMode_Movie"; break; 
    case kEdsAEMode_PhotoInMovie: return "kEdsAEMode_PhotoInMovie"; break; 
    case kEdsAEMode_SceneIntelligentAuto: return "kEdsAEMode_SceneIntelligentAuto"; break; 
    case kEdsAEMode_Unknown: return "kEdsAEMode_Unknown"; break; 
    default: return "UNKNOWN AE MODE"; break;
  }
}

std::string canon_metering_mode_to_string(EdsUInt32 mode) {
  switch(mode) {
    case 1: return "Spot metering"; break;
    case 3: return "Evaluative metering"; break;
    case 4: return "Partial metering"; break;
    case 5: return "Center-weighted averaging metering"; break;
    default: return "UNKNOWN METERING MODE"; break;
  }
}

std::string canon_evf_output_device_to_string(EdsUInt32 d) {
  switch(d) {
    case kEdsEvfOutputDevice_TFT: return "KEdsEvfOutputDevice_TFT"; break;
    case kEdsEvfOutputDevice_PC: return "KEdsEvfOutputDevice_PC"; break;
    case (kEdsEvfOutputDevice_PC | kEdsEvfOutputDevice_TFT): return "KEdsEvfOutputDevice_PC && KEdsEvfOutputDevice_TFT"; break;
    default: return "UNKNOWN EVF OUTPUT DEVICE"; break;
  }
}

std::string canon_evf_af_mode_to_string(EdsUInt32 m) {
  switch(m) {
    case Evf_AFMode_Quick: return "Evf_AFMode_Quick"; break;
    case Evf_AFMode_Live: return "Evf_AFMode_Live"; break;
    case Evf_AFMode_LiveFace: return "Evf_AFMode_LiveFace"; break;
    default: return "UNKNOWN EVF AF MODE"; break;
  }
}

std::string canon_evf_zoom_to_string(EdsUInt32 z) {
  switch(z) {
    case kEdsEvfZoom_Fit: return "kEdsEvfZoom_Fit"; break;
    case kEdsEvfZoom_x5: return "kEdsEvfZoom_x5"; break;
    case kEdsEvfZoom_x10: return "kEdsEvfZoom_x10"; break;
    default: return "UNKNOWN EVF ZOOM"; break;
  }
}
