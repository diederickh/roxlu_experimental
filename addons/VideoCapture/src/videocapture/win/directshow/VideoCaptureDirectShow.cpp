#include <videocapture/Utils.h>
#include <videocapture/win/directshow/VideoCaptureDirectShow.h>
#include <roxlu/core/Log.h>
#include <wmcodecdsp.h>
#include <iomanip>
#include <sstream>

#include <initguid.h>
#include <videocapture/win/directshow/VideoCaptureGUIDs.h>

VideoCaptureDirectShow2::VideoCaptureDirectShow2()
  :graph_builder(NULL)
  ,capture_graph_builder(NULL)
  ,device_filter(NULL)
  ,sample_grabber_filter(NULL)
  ,sample_grabber(NULL)
  ,null_renderer_filter(NULL)
  ,media_control(NULL)
  ,media_event(NULL)
  ,capture_cb(NULL)
{
  RX_VERBOSE("VideoCaptureDirectShow()");
  HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
  if(FAILED(hr)) {
    RX_ERROR("Cannot intialize COM");
    ::exit(EXIT_FAILURE);
  }

}

VideoCaptureDirectShow2::~VideoCaptureDirectShow2() {
  if(media_control) {
    closeDevice();
  }

  CoUninitialize();
}

// Capture control
// --------------------------------------------------------------------------------------
int VideoCaptureDirectShow2::listDevices() {
  // Get enumerator creator 
  int count = 0;

  IEnumMoniker* enum_moniker = NULL;
  if(!createDeviceMoniker(&enum_moniker)) {
    RX_ERROR("Cannot create IEnumMoniker so also not the device");
    goto done;
  }

  // Get information from devices
  HRESULT hr = S_OK;
  IMoniker* moniker = NULL;
  while(enum_moniker->Next(1, &moniker, NULL) == S_OK) {
    
    // Get the properties bag
    IPropertyBag* property_bag = NULL;
    hr = moniker->BindToStorage(0, 0, IID_PPV_ARGS(&property_bag));
    if(FAILED(hr)) {
      RX_ERROR("Cannot bind property bag to storage");
      moniker->Release();
      continue;
    }

    VARIANT var;
    VariantInit(&var);

    // Get the FriendlyName
    hr = property_bag->Read(L"FriendlyName", &var, 0);
    if(SUCCEEDED(hr)) {
      RX_VERBOSE("[%d] %S", count, var.bstrVal);
      VariantClear(&var);
    }
    else {
      RX_ERROR("Cannot retrieve the FriendlyName");
    }
    
    ++count;
    property_bag->Release();
    moniker->Release();
  }

 done:
  safeReleaseDirectShow(&enum_moniker);
  return count;
}

bool VideoCaptureDirectShow2::openDevice(int device, VideoCaptureSettings cfg) {

  if(capture_cb) {
    RX_ERROR("Cannot open the device because you've already opened one");
    return false;
  }

  // Get the best matching capability
  AVCapability best_cap;
  if(!getBestMatchingCapability(device, cfg, best_cap)) {
    RX_ERROR("Cannot find a matching capability for this device");
    return false;
  }

  // Create the IGraphBuilder/ICaptureGraphBuilder/IBaseFilter for the given device
  bool r = initCaptureGraphBuilderForDevice(device, &device_filter, &graph_builder, &capture_graph_builder);
  if(!r) {
    return false;
  }
  if(!setDeviceFilterMediaType(capture_graph_builder, device_filter, best_cap)) {
    goto error;
  }

  // Add a SampleGrabber to the filter.
  if(!createSampleGrabberFilter(&sample_grabber_filter)) {
    goto error;
  }
  if(!addSampleGrabberFilter(graph_builder, sample_grabber_filter)) {
    goto error;
  }
  if(!createSampleGrabber(sample_grabber_filter, &sample_grabber)) {
    goto error;
  }
  if(!setSampleGrabberMediaType(sample_grabber, cfg.in_pixel_format)) {
    goto error;
  }

  capture_cb = new VideoCaptureDirectShowCB(this);
  if(!setSampleGrabberCallback(sample_grabber, capture_cb)) {
    goto error;
  }

  // Create null renderer - endpoint of the filter graph
  if(!createNullRendererFilter(&null_renderer_filter)) {
    goto error;
  }
  if(!addNullRendererFilter(graph_builder, null_renderer_filter)) {
    goto error;
  }

  // Connect the filters
  HRESULT hr = capture_graph_builder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, device_filter, sample_grabber_filter, null_renderer_filter);
  if(FAILED(hr)) {
    RX_ERROR("Failed to conenct the filter graph");
    goto error;
  }

  printConnectedMediaTypeForSampleGrabber(sample_grabber);
  
  // Create media event/control 
  if(!createMediaControl(graph_builder, &media_control)) {
    goto error;
  }
  if(!createMediaEvent(graph_builder, &media_event)) {
    goto error;
  }

  return true;

 error:
  safeReleaseDirectShow(&device_filter);
  safeReleaseDirectShow(&graph_builder);
  safeReleaseDirectShow(&capture_graph_builder);
  safeReleaseDirectShow(&sample_grabber_filter);
  safeReleaseDirectShow(&null_renderer_filter);
  safeReleaseDirectShow(&sample_grabber);
  safeReleaseDirectShow(&media_control);
  safeReleaseDirectShow(&media_event);
  if(capture_cb) {
    delete capture_cb;
    capture_cb = NULL;
  }
  return false;
}

bool VideoCaptureDirectShow2::closeDevice() {

  if(!media_control) {
    RX_ERROR("Cannot close the device because it's not setup or is already closed");
    return false;
  }

  HRESULT hr = media_control->StopWhenReady();
  if(FAILED(hr)) {
    RX_ERROR("Failed to stop the capture stream");
    return false;
  }

  safeReleaseDirectShow(&null_renderer_filter);
  safeReleaseDirectShow(&device_filter);
  safeReleaseDirectShow(&sample_grabber);
  safeReleaseDirectShow(&sample_grabber_filter);
  safeReleaseDirectShow(&media_control);
  safeReleaseDirectShow(&media_event);
  safeReleaseDirectShow(&capture_graph_builder);
  safeReleaseDirectShow(&graph_builder);

  if(capture_cb) {
    delete capture_cb;
    capture_cb = NULL;
  }

  RX_VERBOSE("%p, %p, %p, %p", null_renderer_filter, device_filter, sample_grabber, sample_grabber_filter);
  RX_VERBOSE("%p, %p, %p, %p", media_control, media_event, graph_builder, capture_graph_builder);
  return true;
}

bool VideoCaptureDirectShow2::startCapture() {

  if(!media_control) {
    RX_ERROR("Cannot start capture because the capture filter hasn't been setup");
    return false;
  }

  HRESULT hr = media_control->Run();
  if(FAILED(hr)) {
    RX_ERROR("Error while trying to start the capture stream");
    return false;
  }

  return true;
}

bool VideoCaptureDirectShow2::stopCapture() {
  if(!media_control) {
    RX_ERROR("Cannot stop the capture because the capture filter hasn't been setup");
    return false;
  }

  HRESULT hr = media_control->Stop();
  if(FAILED(hr)) {
    RX_ERROR("Erro while trying to stop the capture");
    return false;
  }

  return true;
}

void VideoCaptureDirectShow2::update() {
}



// Capabilities
// --------------------------------------------------------------------------------------
std::vector<AVCapability> VideoCaptureDirectShow2::getCapabilities(int device) {
  std::vector<AVCapability> result;
  IAMStreamConfig* cap_stream_config = NULL;
  IGraphBuilder* cap_graph_builder = NULL;
  ICaptureGraphBuilder2* cap_capture_graph_builder = NULL;
  IBaseFilter* cap_device_filter = NULL;

  // Create a IGraphBuilder for the device
  if(!initCaptureGraphBuilderForDevice(device, &cap_device_filter, &cap_graph_builder, &cap_capture_graph_builder)) {
    RX_ERROR("Cannot setup the IGraphBuilder/ICaptureGraphBuilder/IBaseFilter (device)");
    goto done;
  }

  // Get the StreamConfig object for the capture device filter which contains info on this device
  if(!createStreamConfig(cap_capture_graph_builder, cap_device_filter, &cap_stream_config)) {
    RX_ERROR("Cannot create the IAMStreamConfig to query capabilities");
    goto done;
  }

  int count = 0;
  int size = 0;
  HRESULT hr = cap_stream_config->GetNumberOfCapabilities(&count, &size);
  if(SUCCEEDED(hr)) {
    for(int i = 0; i < count; ++i) {
 
      VIDEO_STREAM_CONFIG_CAPS caps;
      AM_MEDIA_TYPE* mt;
      hr = cap_stream_config->GetStreamCaps(i, &mt, (BYTE*)&caps);
      if(FAILED(hr)) {
        RX_ERROR("GetStreamCaps() failed");
        continue;
      }
 
      if(mt->majortype == MEDIATYPE_Video 
         && mt->formattype == FORMAT_VideoInfo
         && mt->cbFormat >= sizeof(VIDEOINFOHEADER) 
         && mt->pbFormat != NULL)
        {
          VIDEOINFOHEADER* info = (VIDEOINFOHEADER*)mt->pbFormat;
          AVPixelFormat av_pix_fmt = mediaSubTypeGUIDToLibavPixelFormat(mt->subtype);
          
          AVCapability cap;
          cap.size.width = info->bmiHeader.biWidth;
          cap.size.height = info->bmiHeader.biHeight;
          cap.pixel_format = av_pix_fmt;
          cap.framerate.num = info->AvgTimePerFrame;
          cap.framerate.den = 10000000; 
          cap.index = i;
          result.push_back(cap);
        }
      deleteMediaType(mt);
    }
  }

 done:
  safeReleaseDirectShow(&cap_graph_builder);
  safeReleaseDirectShow(&cap_capture_graph_builder);
  safeReleaseDirectShow(&cap_device_filter);
  safeReleaseDirectShow(&cap_stream_config);
  return result;
}

// HELPERS 
// --------------------------------------------------------------------------------------

// Creates an IEnumMoniker for video capture devices - caller must Release() the object
bool VideoCaptureDirectShow2::createDeviceMoniker(IEnumMoniker** moniker) {

  // Get enumerator creator 
  ICreateDevEnum* create_dev_enum = NULL;
  HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&create_dev_enum));
  if(FAILED(hr)) {
    RX_ERROR("Cannot create a CLSID_SystemDeviceEnum");
    return false;
  }

  // Get device enumerator
  hr = create_dev_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, moniker, 0);
  if(FAILED(hr)) {
    RX_ERROR("Cannot create a enum moniker to iterate over devices");
    safeReleaseDirectShow(&create_dev_enum);
    return false;
  }

  safeReleaseDirectShow(&create_dev_enum);
  return true;
}

// Returns the IMoniker* for the given device index. - caller must Release() the object
bool VideoCaptureDirectShow2::createDeviceFilter(int device, IBaseFilter** deviceFilter) {

  IEnumMoniker* enum_moniker = NULL;
  if(!createDeviceMoniker(&enum_moniker)) {
    RX_ERROR("Cannot create IEnumMoniker so also not the device");
    return false;
  }
  
  // Get information from devices
  IMoniker* moniker = NULL;
  int count = 0;
  bool found = false;
  while(enum_moniker->Next(1, &moniker, NULL) == S_OK) {

    if(count == device) {
      HRESULT hr = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)deviceFilter);
      if(FAILED(hr)) {
        RX_ERROR("Error while trying to bind the device to the IBaseFilter");
        break;
      }
      found = true;
      (*deviceFilter)->AddRef(); // @todo - do we really need this here?
      break;
    }

    ++count;
    moniker->Release();
  }

  safeReleaseDirectShow(&enum_moniker);
  safeReleaseDirectShow(&moniker);
  return true;
}

// Create + setup an device, IGraphBuilder and ICaptureGraphBuilder = user must realease all
bool VideoCaptureDirectShow2::initCaptureGraphBuilderForDevice(int device, IBaseFilter** deviceFilter, IGraphBuilder** graphBuilder, ICaptureGraphBuilder2** captureBuilder) {

  // Setup the ICaptureGraphBuilder2 + IGraphBuilder
  if(!initCaptureGraphBuilder(graphBuilder, captureBuilder)) {
    RX_ERROR("Cannot initialize the capture graph builder for a device");
    return false;
  }

  // Get the device filter that we add to the graph
  if(!createDeviceFilter(device, deviceFilter)) {
    RX_ERROR("Cannot find device to setup a graph");
    safeReleaseDirectShow(graphBuilder);
    safeReleaseDirectShow(captureBuilder);
    return false;
  }

  // Add the device to the graph.
  HRESULT hr = (*graphBuilder)->AddFilter(*deviceFilter, L"Video Capture Device");
  if(FAILED(hr)) {
    RX_ERROR("Error while trying to add the capture device to the graph");
    safeReleaseDirectShow(graphBuilder);
    safeReleaseDirectShow(captureBuilder);
    safeReleaseDirectShow(deviceFilter);
    return false;
  }

  return true;
}

// Create + setup an IGraphBuilder + ICaptureGraphBuilder2 - user must call release on both 
bool VideoCaptureDirectShow2::initCaptureGraphBuilder(IGraphBuilder** graphBuilder, ICaptureGraphBuilder2** captureGraphBuilder) {

  if(!createGraphBuilder(graphBuilder)) {
    goto error;
  }

  if(!createCaptureGraphBuilder(captureGraphBuilder)) {
    goto error;
  }
  
  HRESULT hr = (*captureGraphBuilder)->SetFiltergraph(*graphBuilder);
  if(FAILED(hr)) {
    RX_ERROR("Cannot set the IGraphBuilder on the ICaptureGraphBuilder2");
    goto error;
  }

  return true;

 error:
  safeReleaseDirectShow(graphBuilder);
  safeReleaseDirectShow(captureGraphBuilder);
  return false;
}

// Create a IGraphBuilder - user must call Release()
bool VideoCaptureDirectShow2::createGraphBuilder(IGraphBuilder** builder) {
  HRESULT hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**) builder);
  if(FAILED(hr)) {
    RX_ERROR("Error while creating the IGraphBuilder instance");
    return false;
  }
  return true;
}

// Create ICaptureGraphBuilder2 - user must call Release()
bool VideoCaptureDirectShow2::createCaptureGraphBuilder(ICaptureGraphBuilder2** captureBuilder) {
  HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)captureBuilder);
  if(FAILED(hr)) {
    RX_ERROR("Error while creating the ICaptureGraphBuilder2 instance");
    return false;
  }
  return true;
}

// Create a IAMStreamConfig that you can use to configure the graph - user must call Release()
bool VideoCaptureDirectShow2::createStreamConfig(ICaptureGraphBuilder2* captureBuilder, IBaseFilter* startFilter, IAMStreamConfig** config) {
  if(!captureBuilder) {
    RX_ERROR("The given ICaptureGraphBuilder2 is invalid");
    return false;
  }
  if(!startFilter) {
    RX_ERROR("The given IBaseFilter (startFilter) is invalid");
    return false;
  }

  HRESULT hr = captureBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, startFilter, IID_IAMStreamConfig, (void**)config);
  if(FAILED(hr)) {
    RX_ERROR("Cannot find the IID_IAMStreamConfig interface");
    return false;
  }

  return true;
}


// Create IMediaControl - user must call Release()
bool VideoCaptureDirectShow2::createMediaControl(IGraphBuilder* graphBuilder, IMediaControl** mediaControl) {

  if(!graphBuilder) {
    RX_ERROR("Before calling createMediaControl, make sure you created the graphBuilder");
    return false;
  }

  HRESULT hr = graphBuilder->QueryInterface(IID_IMediaControl, (void**)mediaControl);
  if(FAILED(hr)) {
    RX_ERROR("Cannot create IMediaControl");
    return false;
  }
  return true;
}

// Create IMediaEventEx - user must call Release()
bool VideoCaptureDirectShow2::createMediaEvent(IGraphBuilder* graphBuilder, IMediaEventEx** mediaEvent) {

  if(!graphBuilder) {
    RX_ERROR("Before calling createMediaEvent, make sure you created the graphBuilder");
    return false;
  }

  HRESULT hr = graphBuilder->QueryInterface(IID_IMediaEvent, (void**)mediaEvent);
  if(FAILED(hr)) {
    RX_ERROR("Cannot create IMediaEVent");
    return false;
  }

  return true;
}


// Create a SampleGraberFilter - user must call Release()
bool VideoCaptureDirectShow2::createSampleGrabberFilter(IBaseFilter** grabberFilter) {
  HRESULT hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)grabberFilter);
  if(FAILED(hr)) {
    RX_ERROR("Cannot create the Sample Grabber Filter (IBaseFilter)");
    return false;
  }
  return true;
}

// Add a SampleGrabber filter to the graph - the user must call Release() on error (we do not allocate anything here)
bool VideoCaptureDirectShow2::addSampleGrabberFilter(IGraphBuilder* graphBuilder, IBaseFilter* grabberFilter) {
  if(!graphBuilder) {
    RX_ERROR("Cannot add a sample grabber filter to an uninialized IGraphBuilder");
    return false;
  }
  if(!grabberFilter) {
    RX_ERROR("Cannot add a SampleGrabber to the graph when it's not initialized");
    return false;
  }

  HRESULT hr = graphBuilder->AddFilter(grabberFilter, L"Sample Grabber");
  if(FAILED(hr)) {
    RX_ERROR("Error when trying to add the SampleGrabber to the graph");
    return false;
  }

  return true;
}

// Allocates the ISampleGrabber interface
bool VideoCaptureDirectShow2::createSampleGrabber(IBaseFilter* grabberFilter, ISampleGrabber** sampleGrabber) {
  if(!grabberFilter) {
    RX_ERROR("The given grabberFilter is invalid");
    return false;
  }

  HRESULT hr = grabberFilter->QueryInterface(IID_ISampleGrabber, (void**)sampleGrabber);
  if(FAILED(hr)) {
    RX_ERROR("Cannot find the IID_ISampleGrabber interface");
    return false;
  }
  return true;
}

bool VideoCaptureDirectShow2::setSampleGrabberCallback(ISampleGrabber* sampleGrabber, ISampleGrabberCB* sampleGrabberCB) {

  if(!sampleGrabber) {
    RX_ERROR("Cannot set a ISampleGrabberCB on a invalide ISampleGrabber*");
    return false;
  }
  if(!sampleGrabberCB) {
    RX_ERROR("Cannot set a ISampleGrabberCB when its invalid");
    return false;
  }
  
  HRESULT hr = sampleGrabber->SetBufferSamples(TRUE);
  if(FAILED(hr)) {
    RX_ERROR("SetBufferSampled() failed");
    return false;
  }

  hr = sampleGrabber->SetOneShot(FALSE);
  if(FAILED(hr)) {
    RX_ERROR("SetOneShot() failed");
    return false;
  }

  hr = sampleGrabber->SetCallback(sampleGrabberCB,1);
  if(FAILED(hr)) {
    RX_ERROR("setCallback() failed");
    return false;
  }

  return true;
}

// Create a CLSID_NullRenderer - user must call Release()
bool VideoCaptureDirectShow2::createNullRendererFilter(IBaseFilter** nullRendererFilter) {
  HRESULT hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)nullRendererFilter);
  if(FAILED(hr)) {
    RX_ERROR("Error creating the null renderer");
    return false;
  }
  return true;
}

bool VideoCaptureDirectShow2::addNullRendererFilter(IGraphBuilder* graphBuilder, IBaseFilter* nullRendererFilter) {

  if(!graphBuilder) {
    RX_ERROR("Cannot add a CLSID_NullRenderer to an invalid IGraphBuilder");
    return false;
  }

  HRESULT hr = graphBuilder->AddFilter(nullRendererFilter, L"NULLRenderer");
  if(FAILED(hr)) {
    RX_ERROR("Cannot add the CLSID_NullRenderer to the IGraphBuilder");
    return false;
  }

  return true;
}

bool VideoCaptureDirectShow2::setSampleGrabberMediaType(ISampleGrabber* sampleGrabber, AVPixelFormat pixelFormat) {
  if(!sampleGrabber) {
    RX_ERROR("Cannot set the media type for an uninitialize sample grabber");
    return false;
  }

  GUID subtype = libavPixelFormatToMediaSubTypeGUID(pixelFormat);
  if(subtype == GUID_NULL) {
    std::string str_fmt = rx_libav_pixel_format_to_string(pixelFormat);
    RX_ERROR("Cannot find a matching MediaSubType for AVPixelFormat: %s", str_fmt.c_str());
    return false;
  }

  AM_MEDIA_TYPE mt;
  ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
  mt.majortype = MEDIATYPE_Video;
  mt.subtype = subtype;
  mt.formattype = FORMAT_VideoInfo;

  HRESULT hr = sampleGrabber->SetMediaType(&mt);
  if(FAILED(hr)) {
    RX_ERROR("Error while trying to set the media type for the ISampleGrabber");
    return false;
  }

  return true;
}

bool VideoCaptureDirectShow2::setDeviceFilterMediaType(ICaptureGraphBuilder2* captureBuilder, IBaseFilter* deviceFilter, AVCapability cap) {

  if(!captureBuilder) {
    RX_ERROR("Cannot set device filter media type because the given ICaptureGraphBuilder* is invalid");
    return false;
  }

  if(!deviceFilter) {
    RX_ERROR("Cannot set the media type for the device filter because the device filter is invalid");
    return false;
  }
  
  if(cap.index < 0) {
    RX_ERROR("Cannot set the media type for the device filter because the given AVCapability has not index. Iterate over the stream caps to retrieve the caps index that we need");
    return false;
  }

  IAMStreamConfig* conf = NULL;
  HRESULT hr = captureBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, deviceFilter, IID_IAMStreamConfig, (void**)&conf);
  if(FAILED(hr)) {
    RX_ERROR("Failed to retrieve a IAMStreamConfig to set the device filter media type");
    return false;
  }

  bool result = true;
  AM_MEDIA_TYPE* mt;
  VIDEO_STREAM_CONFIG_CAPS caps;
  hr = conf->GetStreamCaps(cap.index, &mt, (BYTE*)&caps);
  if(FAILED(hr)) {
    RX_ERROR("Failed to retrieve the AM_MEDIA_TYPE for the AVCapabiltiy with stream caps index: %d", cap.index);
    result = false;
    goto done;
  }
  
  if(mt->majortype != MEDIATYPE_Video) {
    RX_ERROR("The AM_MEDIA_TYPE we found is not an Video type so we cannot use it to set the media format of the device filter");
    result = false;
    goto done;
  }
  if(mt->formattype != FORMAT_VideoInfo) {
    RX_ERROR("The AM_MEDIA_TYPE we found is not a Format_VideoInfo, so cannot set media type of device filter");
    result = false;
    goto done;
  }
  if(mt->cbFormat < sizeof(VIDEOINFOHEADER)) {
    RX_ERROR("The AMD_MEDIA_TYPE has an invalid cbFormat size");
    result = false;
    goto done;
  }
  if(mt->pbFormat == NULL) {
    RX_ERROR("The AM_MEDIA_TYPE.pbFormat is NULL; cannot set type of device filter");
    result = false;
    goto done;
  }

  GUID guid_pixfmt = libavPixelFormatToMediaSubTypeGUID(cap.pixel_format);
  if(mt->subtype != guid_pixfmt) {
    RX_ERROR("The AM_MEDIA_TYPE.subtype is not the same as the one we want..");
    result = false;
    goto done;
  }

  hr = conf->SetFormat(mt);
  if(FAILED(hr)) {
    RX_ERROR("Failed to set the AM_MEDIA_TYPE for the device filter");
    result = false;
    goto done;
  }

 done:
  deleteMediaType(mt);
  safeReleaseDirectShow(&conf);
  return result;
}


// UTILS 
// --------------------------------------------------------------------------------------
void VideoCaptureDirectShow2::printConnectedMediaTypeForSampleGrabber(ISampleGrabber* sampleGrabber) {
  if(!sampleGrabber) {
    RX_ERROR("Cannot print the connected media type because the given ISampleGrabber* is invalid");
    return;
  }

  AM_MEDIA_TYPE mt;
  HRESULT hr = sampleGrabber->GetConnectedMediaType(&mt);
  if(FAILED(hr)) {
    RX_ERROR("Error while trying to get the conneted media type from the ISampleGrabber* - did you call ICaptureGraphBuilder2->RenderStream()? make sure you did before calling this function");
    return;
  }

  printMediaType(&mt);
}

void VideoCaptureDirectShow2::printMediaType(AM_MEDIA_TYPE* mt) {
  RX_VERBOSE("AM_MEDIA_TYPE.majortype: %s",              mediaFormatMajorTypeToString(mt->majortype).c_str());
  RX_VERBOSE("AM_MEDIA_TYPE.subtype: %s",                mediaFormatSubTypeToString(mt->subtype).c_str());
  RX_VERBOSE("AM_MEDIA_TYPE.bFixedSizeSamples: %c",      (mt->bFixedSizeSamples == TRUE) ? 'y' : 'n');
  RX_VERBOSE("AM_MEDIA_TYPE.bTemporalCompression: %c",   (mt->bTemporalCompression == TRUE) ? 'y' : 'n');
  RX_VERBOSE("AM_MEDIA_TYPE.lSampleSize: %Iu",           mt->lSampleSize);
  RX_VERBOSE("AM_MEDIA_TYPE.formattype: %s",             mediaFormatFormatTypeToString(mt->formattype).c_str());
  RX_VERBOSE("AM_MEDIA_TYPE.cbFormat: %Iu",              mt->cbFormat);

  if(mt->formattype == FORMAT_VideoInfo && mt->cbFormat >= sizeof(VIDEOINFOHEADER)) {
    VIDEOINFOHEADER* ih = (VIDEOINFOHEADER*)mt->pbFormat;
    double fps = 1.0 / (double(ih->AvgTimePerFrame) / (1000000.0f * 10)); 

    RX_VERBOSE("VIDEOINFOHEADER - width: %Iu",        LONG(ih->bmiHeader.biWidth));
    RX_VERBOSE("VIDEOINFOHEADER - height: %Iu",       LONG(ih->bmiHeader.biHeight));
    RX_VERBOSE("VIDEOINFOHEADER - fps: %f", fps);
  }
}

std::string VideoCaptureDirectShow2::mediaFormatMajorTypeToString(GUID type) {
  if(type == MEDIATYPE_AnalogAudio)        { return "MEDIATYPE_AnalogAudio";       }
  else if(type == MEDIATYPE_AnalogVideo)   { return "MEDIATYPE_AnalogVideo";       }
  else if(type == MEDIATYPE_Audio)         { return "MEDIATYPE_Audio";             }
  else if(type == MEDIATYPE_AUXLine21Data) { return "MEDIATYPE_AUXLine21Data";     } 
  else if(type == MEDIATYPE_File)          { return "MEDIATYPE_File";              }
  else if(type == MEDIATYPE_Interleaved)   { return "MEDIATYPE_Interleaved";       } 
  else if(type == MEDIATYPE_LMRT)          { return "MEDIATYPE_LMRT";              } 
  else if(type == MEDIATYPE_Midi)          { return "MEDIATYPE_Midi";              } 
  else if(type == MEDIATYPE_MPEG2_PES)     { return "MEDIATYPE_MPEG2_PES";         } 
  else if(type == MEDIATYPE_MPEG2_SECTIONS){ return "MEDIATYPE_MPEG2_SECTIONS";    } 
  else if(type == MEDIATYPE_ScriptCommand) { return "MEDIATYPE_ScriptCommand";     }
  else if(type == MEDIATYPE_Stream)        { return "MEDIATYPE_Stream";            } 
  else if(type == MEDIATYPE_Text)          { return "MEDIATYPE_Text";              }
  else if(type == MEDIATYPE_Timecode)      { return "MEDIATYPE_Timecode";          } 
  else if(type == MEDIATYPE_URL_STREAM)    { return "MEDIATYPE_URL_STREAM";        } 
  else if(type == MEDIATYPE_VBI)           { return "MEDIATYPE_VBI";               } 
  else if(type == MEDIATYPE_Video)         { return "MEDIATYPE_Video";             } 

  std::string str = GUIDtoString(type);
  return "OTHER SUBTYPE WITH GUID " +str;  
}

std::string VideoCaptureDirectShow2::mediaFormatSubTypeToString(GUID type) {
  if(type == MEDIASUBTYPE_RGB1)                   { return "MEDIASUBTYPE_RGB1";        } 
  else if(type == MEDIASUBTYPE_RGB4)              { return "MEDIASUBTYPE_RGB4";        }
  else if(type == MEDIASUBTYPE_RGB8)              { return "MEDIASUBTYPE_RGB8";        }
  else if(type == MEDIASUBTYPE_RGB555)            { return "MEDIASUBTYPE_RGB555";      }
  else if(type == MEDIASUBTYPE_RGB565)            { return "MEDIASUBTYPE_RGB565";      }
  else if(type == MEDIASUBTYPE_RGB24)             { return "MEDIASUBTYPE_RGB24";       }
  else if(type == MEDIASUBTYPE_RGB32)             { return "MEDIASUBTYPE_RGB32";       }
  else if(type == MEDIASUBTYPE_ARGB1555)          { return "MEDIASUBTYPE_ARGB1555";    }
  else if(type == MEDIASUBTYPE_ARGB32)            { return "MEDIASUBTYPE_ARGB32";      }
  else if(type == MEDIASUBTYPE_ARGB4444)          { return "MEDIASUBTYPE_ARGB4444";    } 
  else if(type == MEDIASUBTYPE_A2R10G10B10)       { return "MEDIASUBTYPE_A2R10G10B10"; }
  else if(type == MEDIASUBTYPE_A2B10G10R10)       { return "MEDIASUBTYPE_A2B10G10R10"; }
 
  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd391027(v=vs.85).aspx
  else if(type == MEDIASUBTYPE_AYUV)              { return "MEDIASUBTYPE_AYUV";        } 
  else if(type == MEDIASUBTYPE_YUY2)              { return "MEDIASUBTYPE_YUY2";        }
  else if(type == MEDIASUBTYPE_UYVY)              { return "MEDIASUBTYPE_UYVY";        } 
  else if(type == MEDIASUBTYPE_IMC1)              { return "MEDIASUBTYPE_IMC1";        } 
  else if(type == MEDIASUBTYPE_IMC3)              { return "MEDIASUBTYPE_IMC3";        }
  else if(type == MEDIASUBTYPE_IMC2)              { return "MEDIASUBTYPE_IMC2";        } 
  else if(type == MEDIASUBTYPE_IMC4)              { return "MEDIASUBTYPE_IMC4";        } 
  else if(type == MEDIASUBTYPE_YV12)              { return "MEDIASUBTYPE_YV12";        }
  else if(type == MEDIASUBTYPE_NV12)              { return "MEDIASUBTYPE_NV12";        } 
  else if(type == MEDIASUBTYPE_IF09)              { return "MEDIASUBTYPE_IF09";        }
  else if(type == MEDIASUBTYPE_IYUV)              { return "MEDIASUBTYPE_IYUV";        } 
  else if(type == MEDIASUBTYPE_I420)              { return "MEDIASUBTYPE_I420";        }  
  else if(type == MEDIASUBTYPE_Y211)              { return "MEDIASUBTYPE_Y211";        }
  else if(type == MEDIASUBTYPE_Y411)              { return "MEDIASUBTYPE_Y411";        }
  else if(type == MEDIASUBTYPE_Y41P)              { return "MEDIASUBTYPE_Y41P";        }
  else if(type == MEDIASUBTYPE_YVU9)              { return "MEDIASUBTYPE_YVU9";        } 
  else if(type == MEDIASUBTYPE_YVYU)              { return "MEDIASUBTYPE_YVYU";        }
  
 
  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd757808(v=vs.85).aspx,  wmcodecdsp.h * defined in SDK 7.1
  else if(type == MEDIASUBTYPE_H264)              { return "MEDIASUBTYPE_H264";        }
 
  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd390688(v=vs.85).aspx
  else if(type == MEDIASUBTYPE_CFCC)              { return "MEDIASUBTYPE_CFCC";        }
  else if(type == MEDIASUBTYPE_CLJR)              { return "MEDIASUBTYPE_CLJR";        } 
  else if(type == MEDIASUBTYPE_CPLA)              { return "MEDIASUBTYPE_CPLA";        } 
  else if(type == MEDIASUBTYPE_CLPL)              { return "MEDIASUBTYPE_CLPL";        } 
  else if(type == MEDIASUBTYPE_IJPG)              { return "MEDIASUBTYPE_IJPG";        }
  else if(type == MEDIASUBTYPE_MDVF)              { return "MEDIASUBTYPE_MDVF";        }
  else if(type == MEDIASUBTYPE_MJPG)              { return "MEDIASUBTYPE_MJPG";        } 
  else if(type == MEDIASUBTYPE_Overlay)           { return "MEDIASUBTYPE_Overlay";     } 
  else if(type == MEDIASUBTYPE_Plum)              { return "MEDIASUBTYPE_Plum";        } 
  else if(type == MEDIASUBTYPE_QTJpeg)            { return "MEDIASUBTYPE_QTJpeg";      } 
  else if(type == MEDIASUBTYPE_QTMovie)           { return "MEDIASUBTYPE_QTMovie";     }
  else if(type == MEDIASUBTYPE_QTRle)             { return "MEDIASUBTYPE_QTRle";       }
  else if(type == MEDIASUBTYPE_QTRpza)            { return "MEDIASUBTYPE_QTRpza";      } 
  else if(type == MEDIASUBTYPE_QTSmc)             { return "MEDIASUBTYPE_QTSmc";       } 
  else if(type == MEDIASUBTYPE_TVMJ)              { return "MEDIASUBTYPE_TVMJ";        } 
  else if(type == MEDIASUBTYPE_VPVBI)             { return "MEDIASUBTYPE_VPVBI";       } 
  else if(type == MEDIASUBTYPE_VPVideo)           { return "MEDIASUBTYPE_VPVideo";     } 
  else if(type == MEDIASUBTYPE_WAKE)              { return "MEDIASUBTYPE_WAKE";        }
 
  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd390712(v=vs.85).aspx
  else if(type == MEDIASUBTYPE_MPEG1System)       { return "MEDIASUBTYPE_MPEG1System";   } 
  else if(type == MEDIASUBTYPE_MPEG1VideoCD)      { return "MEDIASUBTYPE_MPEG1VideoCD";  }
  else if(type == MEDIASUBTYPE_MPEG1Packet)       { return "MEDIASUBTYPE_MPEG1Packet";   }
  else if(type == MEDIASUBTYPE_MPEG1Payload)      { return "MEDIASUBTYPE_MPEG1Payload";  } 
  else if(type == MEDIASUBTYPE_MPEG1Packet)       { return "MEDIASUBTYPE_MPEG1Packet";   } 
  else if(type == MEDIASUBTYPE_MPEG1Video)        { return "MEDIASUBTYPE_ MPEG1Video";   }
  else if(type == MEDIASUBTYPE_MPEG1Audio)        { return "MEDIASUBTYPE_ MPEG1Audio";   } 
 
  else if(type == MEDIASUBTYPE_MPEG2_VIDEO)       { return "MEDIASUBTYPE_MPEG2_VIDEO";   } 
  else if(type == MEDIASUBTYPE_DOLBY_AC3)         { return "MEDIASUBTYPE_DOLBY_AC3";     } 
  else if(type == MEDIASUBTYPE_MPEG2_AUDIO)       { return "MEDIASUBTYPE_MPEG2_AUDIO";   } 
  else { 
    std::string str = GUIDtoString(type);
    return "OTHER SUBTYPE WITH GUID " +str;  
  }
}

std::string VideoCaptureDirectShow2::mediaFormatFormatTypeToString(GUID type) {
  if(type == FORMAT_DvInfo)               { return "FORMAT_DvInfo"; } 
  else if(type == FORMAT_MPEG2Video)      { return "FORMAT_MPEG2Video"; } 
  else if(type == FORMAT_MPEGStreams)     { return "FORMAT_MPEGStreams"; } 
  else if(type == FORMAT_MPEGVideo)       { return "FORMAT_MPEGVideo"; } 
  else if(type == FORMAT_None)            { return "FORMAT_None"; } 
  else if(type == FORMAT_VideoInfo)       { return "FORMAT_VideoInfo"; } 
  else if(type == FORMAT_VideoInfo2)      { return "FORMAT_VideoInfo2"; } 
  else if(type == FORMAT_WaveFormatEx)    { return "FORMAT_WaveFormatEx"; }
  else if(type == GUID_NULL)              { return "GUID_NULL"; } 
  else {  
    std::string str = GUIDtoString(type);
    return "OTHER FORMAT WITH GUID " +str;  
  }
}

AVPixelFormat VideoCaptureDirectShow2::mediaSubTypeGUIDToLibavPixelFormat(GUID guid) {
  if(guid == MEDIASUBTYPE_RGB24) { return AV_PIX_FMT_RGB24; } 
  else if(guid == MEDIASUBTYPE_I420) { return AV_PIX_FMT_YUV420P; }
  else {
    return AV_PIX_FMT_NONE;
  }
}
 
GUID VideoCaptureDirectShow2::libavPixelFormatToMediaSubTypeGUID(AVPixelFormat fmt) {
  switch(fmt) {
    case AV_PIX_FMT_RGB24:       return MEDIASUBTYPE_RGB24;   break;
    case AV_PIX_FMT_YUV420P:     return MEDIASUBTYPE_I420;    break;
    default:                     return GUID_NULL;            break;
  };
}

// Based on: http://msdn.microsoft.com/nl-nl/library/windows/desktop/dd375432(v=vs.85).aspx
void VideoCaptureDirectShow2::deleteMediaType(AM_MEDIA_TYPE* mt) {
 
  if(mt == NULL) {
    return;
  }
 
  if(mt->cbFormat != 0) {
    CoTaskMemFree((PVOID)mt->pbFormat);
    mt->cbFormat = 0;
    mt->pbFormat = NULL;
  }
 
  if(mt->pUnk != NULL) {
    mt->pUnk->Release();
    mt->pUnk = NULL;
  }   
 
  CoTaskMemFree(mt);
}

template <class T> void safeReleaseDirectShow(T **v) {
  if(*v) {
    (*v)->Release();
    *v = NULL;
  }
}


template <typename T>
std::string directShowToHex(const T& in, int width) {
  std::ostringstream strstr;
  strstr << std::hex << std::setfill('0') << std::setw(width) << in;
  return strstr.str();
}
 
std::string GUIDtoString(const GUID& id) {
    return
      directShowToHex(id.Data1)+"-"+directShowToHex(id.Data2)+"-"+directShowToHex(id.Data3)+"-"
      +directShowToHex((int)id.Data4[0],2)+directShowToHex((int)id.Data4[1],2)+"-"
      +directShowToHex((int)id.Data4[2],2)+directShowToHex((int)id.Data4[3],2)
      +directShowToHex((int)id.Data4[4],2)+directShowToHex((int)id.Data4[5],2)
      +directShowToHex((int)id.Data4[6],2)+directShowToHex((int)id.Data4[7],2);
} 
