#include <videocapture/win/mediafoundation/VideoCaptureMediaFoundation.h>

// INIT + SHUTDOWN
// --------------------------------------------------------------------------------------
VideoCaptureMediaFoundation::VideoCaptureMediaFoundation() 
  :imf_media_source(NULL)
  ,imf_source_reader(NULL)
  ,mediaf_cb(NULL)
  ,is_capturing(false)
{

  HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
  if(FAILED(hr)) {
    RX_ERROR("Cannot intialize COM");
    ::exit(EXIT_FAILURE);
  }

  hr = MFStartup(MF_VERSION);
  if(FAILED(hr)) {
    RX_ERROR("Cannot startup the Media Foundation");
    ::exit(EXIT_FAILURE);
  }
}

VideoCaptureMediaFoundation::~VideoCaptureMediaFoundation() {
  closeDevice();
  MFShutdown();
  CoUninitialize();
}

// GENERAL GRABBER FUNCTIONS
// --------------------------------------------------------------------------------------
int VideoCaptureMediaFoundation::listDevices() {
  UINT32 count = 0;
  IMFAttributes* config = NULL;
  IMFActivate** devices = NULL;

  HRESULT hr = MFCreateAttributes(&config, 1);
  if(SUCCEEDED(hr)) {
    hr = config->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,  MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
  }
  
  if(SUCCEEDED(hr)) {
    hr = MFEnumDeviceSources(config, &devices, &count);
  }

  if(SUCCEEDED(hr) && count > 0) {
    printDeviceNames(devices, count);
  }

  for(DWORD i = 0; i < count; ++i) {
    devices[i]->Release();
  }

  CoTaskMemFree(devices);
  return count;
}

bool VideoCaptureMediaFoundation::openDevice(int device, VideoCaptureSettings cfg) {
  if(is_capturing) {
    RX_ERROR("Cannot open the device; first call close()");
    return false;
  }

  if(imf_media_source) {
    RX_ERROR("Cannot open device because we already have setup another one");
    return false;
  }

  if(!createVideoDeviceSource(device, &imf_media_source)) {
    RX_ERROR("Cannot open device because we couldn't make the device active");
    return false;
  }

  settings = cfg;

  // Set the media format, width and height on the capture device
  std::vector<AVCapability> caps;
  if(!getCapabilities(imf_media_source, caps)) {
    RX_ERROR("Cannot retrieve capabilities");
    safeReleaseMediaFoundation(&imf_media_source);
    return false;
  }

  AVCapability found_cap;
  for(std::vector<AVCapability>::iterator it = caps.begin(); it != caps.end(); ++it) {
    AVCapability& cap = *it;
    if(cap.size.width == cfg.width 
       && cap.size.height == cfg.height
       && cap.pixel_format == cfg.in_pixel_format) 
      {
        found_cap = cap;
        break;
      }
  }

  if(found_cap.pixel_format == AV_PIX_FMT_NONE) {
    RX_ERROR("The device does not support the capabilites defined in the VideoCaptureSettings");
    safeReleaseMediaFoundation(&imf_media_source);
    return false;
  }

  if(!setDeviceFormat(imf_media_source, found_cap.index)) {
    RX_ERROR("An error occured when trying to set the device format");
    safeReleaseMediaFoundation(&imf_media_source);
    return false;
  }

  // Create the source reader and set the media format 
  VideoCaptureMediaFoundationCB::createInstance(this, &mediaf_cb);
  if(!createSourceReader(imf_media_source, mediaf_cb, &imf_source_reader)) {
    RX_ERROR("Error while creating the IMFSourceReader");
    safeReleaseMediaFoundation(&mediaf_cb);
    safeReleaseMediaFoundation(&imf_media_source);
    return false;
  }

  if(!setReaderFormat(imf_source_reader, found_cap)) {
    RX_ERROR("Cannot set the format for the source reader");
    safeReleaseMediaFoundation(&mediaf_cb);
    safeReleaseMediaFoundation(&imf_media_source);
    return false;
  }
  
  return true;
}

bool VideoCaptureMediaFoundation::startCapture() {
  if(!imf_source_reader) {
    RX_ERROR("Cannot start capture becuase it looks like the device hasn't been opened yet");
    return false;
  }
  if(is_capturing) {
    RX_ERROR("Cannot start capture because we are already capturing");
    return false;
  }
  is_capturing = true;

  HRESULT hr = imf_source_reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);
  if(FAILED(hr)) {
    RX_ERROR("Error while trying to ReadSample() on the imf_source_reader");
  }
}

bool VideoCaptureMediaFoundation::stopCapture() {
  if(!imf_source_reader) {
    RX_ERROR("Cannot stop capture because it seems that the device hasn't been opened yet");
    return false;
  }
  if(!is_capturing) {
    RX_ERROR("Cannot stop capture because we're not capturing yet");
    return false;
  }
  is_capturing = false;
}

void VideoCaptureMediaFoundation::update() {
}

bool VideoCaptureMediaFoundation::closeDevice() {
  if(!imf_source_reader) {
    RX_ERROR("Cannot close the device because it seems that is hasn't been opend yet. Did you call openDevice?");
    return false;
  }
  if(is_capturing) {
    stopCapture();
  }

  safeReleaseMediaFoundation(&imf_source_reader);
  safeReleaseMediaFoundation(&imf_media_source); 
  safeReleaseMediaFoundation(&mediaf_cb);
}

/**
 * Create a `IMGSourceReader` and set the `IMFSourceReaderCallback` as the 
 * callback object. 
 *
 */
bool VideoCaptureMediaFoundation::createSourceReader(IMFMediaSource* mediaSource, IMFSourceReaderCallback* callback, IMFSourceReader** sourceReader) {
  if(!mediaSource) {
    RX_ERROR("Cannot create a source readed because the IMFMediaSource passed into this function is not valid");
    return false;
  }

  HRESULT hr = S_OK;
  IMFAttributes* attrs = NULL;
  bool result = true;
  
  hr = MFCreateAttributes(&attrs, 1);
  if(FAILED(hr)) {
    RX_ERROR("Cannot create attributes for the media source reader");
    result = false;
    goto done;
  }

  hr = attrs->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, callback);
  if(FAILED(hr)) {
    RX_ERROR("SetUnknown() failed on the source reader");
    result = false;
    goto done;
  }

  // Create a source reader which sets up the pipeline for us so we get access to the pixels
   hr = MFCreateSourceReaderFromMediaSource(mediaSource, attrs, sourceReader);
  if(FAILED(hr)) {
    RX_ERROR("Error while creating a source reader.");
    result = false;
    goto done;
  }

 done:
  safeReleaseMediaFoundation(&attrs);
  return result;
}


/**
 * Sets the correct IMFMediaType which conforms the given AVCapability.
 *
 */
bool VideoCaptureMediaFoundation::setReaderFormat(IMFSourceReader* reader, AVCapability cap) {
 DWORD media_type_index = 0;
 bool format_set = false;
 HRESULT hr = S_OK;

  while(SUCCEEDED(hr)) {
    AVCapability match_cap;
    IMFMediaType* type = NULL;
    hr = imf_source_reader->GetNativeMediaType(0, media_type_index, &type);
    
    if(SUCCEEDED(hr)) {
      PROPVARIANT var;

      // PIXELFORMAT
      PropVariantInit(&var);
      {
        hr = type->GetItem(MF_MT_SUBTYPE, &var);
        if(SUCCEEDED(hr)) {
          match_cap.pixel_format = mediaFoundationVideoFormatToLibavPixelFormat(*var.puuid);
        }
      }
      PropVariantClear(&var);

      // SIZE
      PropVariantInit(&var);
      {
        hr = type->GetItem(MF_MT_FRAME_SIZE, &var);
        if(SUCCEEDED(hr)) {
          UINT32 high = 0;
          UINT32 low =  0;
          Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
          match_cap.size.width = high;
          match_cap.size.height = low;
        }
      }
      PropVariantClear(&var);
      
      // When the output media type of the source reader  matches our specs, set it!
      if(match_cap.size.width == cap.size.width
         && match_cap.size.height == cap.size.height
         && match_cap.pixel_format == cap.pixel_format) 
        {
          hr = imf_source_reader->SetCurrentMediaType(0, NULL, type);
          if(FAILED(hr)) {
            RX_ERROR("Failed to set the current media type for the given settings.");
          }
          else {
            hr = S_OK; // break;
            format_set = true;
          }
        }

      type->Release();
    }
    else {
      break;
    }
    ++media_type_index;
  }

  return format_set;
}


// CAPABILITIES
// --------------------------------------------------------------------------------------
bool VideoCaptureMediaFoundation::setDeviceFormat(IMFMediaSource* source, DWORD formatIndex) {
  IMFPresentationDescriptor* pres_desc = NULL;
  IMFStreamDescriptor* stream_desc = NULL;
  IMFMediaTypeHandler* handler = NULL;
  IMFMediaType* type = NULL;
  bool result = true;

  HRESULT hr = source->CreatePresentationDescriptor(&pres_desc);
  if(FAILED(hr)) {
    RX_ERROR("source->CreatePresentationDescriptor() failed");
    result = false;
    goto done;
  }

  BOOL selected;
  hr = pres_desc->GetStreamDescriptorByIndex(0, &selected, &stream_desc);
  if(FAILED(hr)) {
    RX_ERROR("pres_desc->GetStreamDescriptorByIndex failed");
    result = false;
    goto done;
  }

  hr = stream_desc->GetMediaTypeHandler(&handler);
  if(FAILED(hr)) {
    RX_ERROR("stream_desc->GetMediaTypehandler() failed");
    result = false;
    goto done;
  }

  hr = handler->GetMediaTypeByIndex(formatIndex, &type);
  if(FAILED(hr)) {
    RX_ERROR("hander->GetMediaTypeByIndex failed");
    result = false;
    goto done;
  }

  hr = handler->SetCurrentMediaType(type);
  if(FAILED(hr)) {
    RX_ERROR("handler->SetCurrentMediaType failed");
    result = false;
    goto done;
  }

 done:
  safeReleaseMediaFoundation(&pres_desc);
  safeReleaseMediaFoundation(&stream_desc);
  safeReleaseMediaFoundation(&handler);
  safeReleaseMediaFoundation(&type);
  return result;
}

// DEBUG + CONVERSION
// --------------------------------------------------------------------------------------
void VideoCaptureMediaFoundation::printDeviceNames(IMFActivate** devices, UINT count) {
  for(DWORD i = 0; i < count; ++i) {
    HRESULT hr = S_OK;
    WCHAR* friendly_name = NULL;
    UINT32 friendly_name_len = 0;

    hr = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,  &friendly_name, &friendly_name_len);
    if(SUCCEEDED(hr)) {
      wprintf(L"[%d] %s\n", int(i), friendly_name);
    }

    CoTaskMemFree(friendly_name);
  }
}


/**
 * Create and active the given `device`. 
 *
 * @param int device [in]   The device index for which you want to get an
 *                          activated IMFMediaSource object. This function 
 *                          allocates this object and increases the reference
 *                          count. When you're ready with this object, make sure
 *                          to call `safeReleaseMediaFoundation(&source)`
 *
 * @param IMFMediaSource** [out]  We allocate and activate the device for the 
 *                                given `device` parameter. When ready, call
 *                                `safeReleaseMediaFoundation(&source)` to free memory.
 */
bool VideoCaptureMediaFoundation::createVideoDeviceSource(int device, IMFMediaSource** source) {

  bool result = true;
  IMFAttributes* config = NULL;
  IMFActivate** devices = NULL;
  UINT32 count = 0;  

  HRESULT hr = MFCreateAttributes(&config, 1);
  if(FAILED(hr)) {
    result = false;
    goto done;
  }

  hr = config->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
  if(FAILED(hr)) {
    RX_ERROR("Cannot set the GUID on the IMFAttributes*");
    result = false;
    goto done;
  }

  hr = MFEnumDeviceSources(config, &devices, &count);
  if(FAILED(hr)) {
    RX_ERROR("Cannot get EnumDeviceSources");
    result = false;
    goto done;
  }
  if(count == 0 || device > count) {
    result = false;
    goto done;
  }

  safeReleaseMediaFoundation(source);
  hr = devices[device]->ActivateObject(IID_PPV_ARGS(source));
  if(FAILED(hr)) {
    RX_ERROR("Cannot activate the object");
    result = false;
    goto done;
  }

  result = true;

 done:

  safeReleaseMediaFoundation(&config);
  for(DWORD i = 0; i < count; ++i) {
    safeReleaseMediaFoundation(&devices[i]);
  }
  CoTaskMemFree(devices);

  return result;
}


std::vector<AVCapability> VideoCaptureMediaFoundation::getCapabilities(int device) {

  std::vector<AVCapability> result;
  IMFMediaSource* source = NULL;
  if(createVideoDeviceSource(0, &source)) {
    getCapabilities(source, result);
    safeReleaseMediaFoundation(&source);
  }

  return result;
}


/** 
 * Get capabilities for the given IMFMediaSource which represents 
 * a video capture device.
 *
 * @param IMFMediaSource* source [in]               Pointer to the video capture source.
 * @param std::vector<AVCapability>& caps [out]     This will be filled with capabilites 
 */
bool VideoCaptureMediaFoundation::getCapabilities(IMFMediaSource* source, std::vector<AVCapability>& caps) {
  IMFPresentationDescriptor* presentation_desc = NULL;
  IMFStreamDescriptor* stream_desc = NULL;
  IMFMediaTypeHandler* media_handler = NULL;
  IMFMediaType* type = NULL;
  bool result = true;

  HRESULT hr = source->CreatePresentationDescriptor(&presentation_desc);
  if(FAILED(hr)) {
    RX_ERROR("Cannot get presentation descriptor");
    result = false;
    goto done;
  }

  BOOL selected;
  hr = presentation_desc->GetStreamDescriptorByIndex(0, &selected, &stream_desc);
  if(FAILED(hr)) {
    RX_ERROR("Cannot get stream descriptor");
    result = false;
    goto done;
  }

  hr = stream_desc->GetMediaTypeHandler(&media_handler);
  if(FAILED(hr)) {
    RX_ERROR("Cannot get media type handler");
    result = false;
    goto done;
  }

  DWORD types_count = 0;
  hr = media_handler->GetMediaTypeCount(&types_count);
  if(FAILED(hr)) {
    RX_ERROR("Cannot get media type count");
    result = false;
    goto done;
  }

  // Loop over all the types
  PROPVARIANT var;
  for(DWORD i = 0; i < types_count; ++i) {

    AVCapability cap;

    hr = media_handler->GetMediaTypeByIndex(i, &type);

    if(FAILED(hr)) {
      RX_ERROR("Cannot get media type by index");
      result = false;
      goto done;
    }
    
    UINT32 attr_count = 0;
    hr = type->GetCount(&attr_count);
    if(FAILED(hr)) {
      RX_ERROR("Cannot type param count");
      result = false;
      goto done;
    }

    if(attr_count > 0) {
      for(UINT32 j = 0; j < attr_count; ++j) {
        GUID guid = { 0 };

        PropVariantInit(&var);
        hr = type->GetItemByIndex(j, &guid, &var);
        if(FAILED(hr)) {
          RX_ERROR("Cannot get item by index");
          result = false;
          goto done;
        }

        if(guid == MF_MT_SUBTYPE && var.vt == VT_CLSID) {
          cap.pixel_format = mediaFoundationVideoFormatToLibavPixelFormat(*var.puuid);
        }
        else if(guid == MF_MT_FRAME_SIZE) {
          UINT32 high = 0;
          UINT32 low =  0;
          Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
          cap.size.width = high;
          cap.size.height = low;
        }
        else if(guid == MF_MT_FRAME_RATE_RANGE_MIN 
                || guid == MF_MT_FRAME_RATE_RANGE_MAX 
                || guid == MF_MT_FRAME_RATE)
          {
            UINT32 high = 0;
            UINT32 low =  0;
            Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
            cap.framerate.num = low;
            cap.framerate.den = high;
        }

        PropVariantClear(&var);
      }

      cap.index = i;
      caps.push_back(cap);
    }

    safeReleaseMediaFoundation(&type);
  }

 done: 
  safeReleaseMediaFoundation(&presentation_desc);
  safeReleaseMediaFoundation(&stream_desc);
  safeReleaseMediaFoundation(&media_handler);
  safeReleaseMediaFoundation(&type);
  PropVariantClear(&var);
  return result;
}

// CONVERSIONS, DEBUG
// ----------------------------------------------

AVPixelFormat VideoCaptureMediaFoundation::mediaFoundationVideoFormatToLibavPixelFormat(GUID guid) {
  if(IsEqualGUID(guid,MFVideoFormat_RGB24))     { return AV_PIX_FMT_RGB24;   }
  else if(IsEqualGUID(guid,MFVideoFormat_I420)) { return AV_PIX_FMT_YUV420P; } 
  else {
    return AV_PIX_FMT_NONE;
  }
}

GUID VideoCaptureMediaFoundation::libavPixelFormatToMediaFoundationVideoFormat(AVPixelFormat fmt) {
  switch(fmt) {
    case AV_PIX_FMT_RGB24:               return MFVideoFormat_RGB24;    break;
    case AV_PIX_FMT_YUV420P:             return MFVideoFormat_I420;     break;
    default:                             return MF_MT_MAJOR_TYPE;       break;
  }
}


#define MEDIAFOUNDATION_CHECK_VIDEOFORMAT(param, val) if (param == val) return #val
std::string VideoCaptureMediaFoundation::mediaFoundationVideoFormatToString(const GUID& guid) {
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_MAJOR_TYPE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_SUBTYPE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_ALL_SAMPLES_INDEPENDENT);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_FIXED_SIZE_SAMPLES);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_COMPRESSED);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_SAMPLE_SIZE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_WRAPPED_TYPE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_NUM_CHANNELS);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_SAMPLES_PER_SECOND);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_AVG_BYTES_PER_SECOND);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_BLOCK_ALIGNMENT);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_BITS_PER_SAMPLE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_VALID_BITS_PER_SAMPLE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_SAMPLES_PER_BLOCK);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_CHANNEL_MASK);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_FOLDDOWN_MATRIX);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_WMADRC_PEAKREF);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_WMADRC_PEAKTARGET);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_WMADRC_AVGREF);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_WMADRC_AVGTARGET);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AUDIO_PREFER_WAVEFORMATEX);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AAC_PAYLOAD_TYPE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_FRAME_SIZE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_FRAME_RATE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_FRAME_RATE_RANGE_MAX);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_FRAME_RATE_RANGE_MIN);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_PIXEL_ASPECT_RATIO);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_DRM_FLAGS);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_PAD_CONTROL_FLAGS);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_SOURCE_CONTENT_HINT);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_VIDEO_CHROMA_SITING);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_INTERLACE_MODE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_TRANSFER_FUNCTION);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_VIDEO_PRIMARIES);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_CUSTOM_VIDEO_PRIMARIES);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_YUV_MATRIX);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_VIDEO_LIGHTING);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_VIDEO_NOMINAL_RANGE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_GEOMETRIC_APERTURE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_MINIMUM_DISPLAY_APERTURE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_PAN_SCAN_APERTURE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_PAN_SCAN_ENABLED);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AVG_BITRATE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AVG_BIT_ERROR_RATE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_MAX_KEYFRAME_SPACING);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_DEFAULT_STRIDE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_PALETTE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_USER_DATA);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_AM_FORMAT_TYPE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_MPEG_START_TIME_CODE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_MPEG2_PROFILE);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_MPEG2_LEVEL);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_MPEG2_FLAGS);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_MPEG_SEQUENCE_HEADER);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_DV_AAUX_SRC_PACK_0);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_DV_AAUX_CTRL_PACK_0);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_DV_AAUX_SRC_PACK_1);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_DV_AAUX_CTRL_PACK_1);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_DV_VAUX_SRC_PACK);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_DV_VAUX_CTRL_PACK);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_ARBITRARY_HEADER);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_ARBITRARY_FORMAT);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_IMAGE_LOSS_TOLERANT); 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_MPEG4_SAMPLE_DESCRIPTION);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_ORIGINAL_4CC); 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MF_MT_ORIGINAL_WAVE_FORMAT_TAG);
    
  // Media types
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFMediaType_Audio);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFMediaType_Video);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFMediaType_Protected);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFMediaType_SAMI);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFMediaType_Script);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFMediaType_Image);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFMediaType_HTML);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFMediaType_Binary);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFMediaType_FileTransfer);

  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_AI44);            //     FCC('AI44')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_ARGB32);          //     D3DFMT_A8R8G8B8 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_AYUV);            //     FCC('AYUV')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_DV25);            //     FCC('dv25')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_DV50);            //     FCC('dv50')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_DVH1);            //     FCC('dvh1')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_DVSD);            //     FCC('dvsd')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_DVSL);            //     FCC('dvsl')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_H264);            //     FCC('H264')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_I420);            //     FCC('I420')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_IYUV);            //     FCC('IYUV')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_M4S2);            //     FCC('M4S2')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_MJPG);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_MP43);            //     FCC('MP43')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_MP4S);            //     FCC('MP4S')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_MP4V);            //     FCC('MP4V')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_MPG1);            //     FCC('MPG1')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_MSS1);            //     FCC('MSS1')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_MSS2);            //     FCC('MSS2')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_NV11);            //     FCC('NV11')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_NV12);            //     FCC('NV12')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_P010);            //     FCC('P010')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_P016);            //     FCC('P016')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_P210);            //     FCC('P210')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_P216);            //     FCC('P216')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_RGB24);           //     D3DFMT_R8G8B8 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_RGB32);           //     D3DFMT_X8R8G8B8 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_RGB555);          //     D3DFMT_X1R5G5B5 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_RGB565);          //     D3DFMT_R5G6B5 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_RGB8);
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_UYVY);            //     FCC('UYVY')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_v210);            //     FCC('v210')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_v410);            //     FCC('v410')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_WMV1);            //     FCC('WMV1')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_WMV2);            //     FCC('WMV2')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_WMV3);            //     FCC('WMV3')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_WVC1);            //     FCC('WVC1')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_Y210);            //     FCC('Y210')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_Y216);            //     FCC('Y216')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_Y410);            //     FCC('Y410')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_Y416);            //     FCC('Y416')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_Y41P);            
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_Y41T);            
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_YUY2);            //     FCC('YUY2')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_YV12);            //     FCC('YV12')
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFVideoFormat_YVYU);

  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_PCM);              //     WAVE_FORMAT_PCM 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_Float);            //     WAVE_FORMAT_IEEE_FLOAT 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_DTS);              //     WAVE_FORMAT_DTS 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_Dolby_AC3_SPDIF);  //     WAVE_FORMAT_DOLBY_AC3_SPDIF 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_DRM);              //     WAVE_FORMAT_DRM 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_WMAudioV8);        //     WAVE_FORMAT_WMAUDIO2 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_WMAudioV9);        //     WAVE_FORMAT_WMAUDIO3 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_WMAudio_Lossless); //     WAVE_FORMAT_WMAUDIO_LOSSLESS 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_WMASPDIF);         //     WAVE_FORMAT_WMASPDIF 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_MSP1);             //     WAVE_FORMAT_WMAVOICE9 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_MP3);              //     WAVE_FORMAT_MPEGLAYER3 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_MPEG);             //     WAVE_FORMAT_MPEG 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_AAC);              //     WAVE_FORMAT_MPEG_HEAAC 
  MEDIAFOUNDATION_CHECK_VIDEOFORMAT(guid, MFAudioFormat_ADTS);             //     WAVE_FORMAT_MPEG_ADTS_AAC 
  return "UNKOWN";
}

// ----------------------------------------------

template <class T> void safeReleaseMediaFoundation(T **t) {
  if(*t) {
    int c = int((*t)->Release());
    *t = NULL;
  }
}


