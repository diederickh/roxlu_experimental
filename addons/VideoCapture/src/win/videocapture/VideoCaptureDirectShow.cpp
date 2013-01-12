#include <videocapture/VideoCaptureDirectShow.h>



// ---------------------------------------------------------------------
// VideoCaptureDirectShoCallback
// ---------------------------------------------------------------------
VideoCaptureDirectShowCallback::VideoCaptureDirectShowCallback(VideoCaptureDirectShow* vidcap)
  :vidcap(vidcap)
{
  InitializeCriticalSection(&critical_section);
}

VideoCaptureDirectShowCallback::~VideoCaptureDirectShowCallback() {
  DeleteCriticalSection(&critical_section);
}

// For some reason sample is always NULL
HRESULT STDMETHODCALLTYPE VideoCaptureDirectShowCallback::SampleCB(
                                                                   double timestamp, 
                                                                   IMediaSample* sample)
{
  if(sample != NULL) {
    printf("callback: %f. %p.\n", timestamp, sample);
  }
  return S_OK;
}

HRESULT STDMETHODCALLTYPE VideoCaptureDirectShowCallback::BufferCB(
                                                      double timestamp, 
                                                      BYTE* buffer,
                                                      long size)
{
  printf("In BufferCB, this = %p, %f, %Iu\n", this, timestamp, size);
  vidcap->frame_cb((void*)buffer, size, vidcap->frame_user);
  return S_OK;
}

STDMETHODIMP VideoCaptureDirectShowCallback::QueryInterface(REFIID riid, void **obj) {
  if((riid == IID_ISampleGrabberCB) || (riid == IID_IUnknown)) {
    *obj = static_cast<ISampleGrabberCB*>(this);
    return S_OK;
  }
  else {
    return E_NOINTERFACE;
  }
}

STDMETHODIMP_(ULONG) VideoCaptureDirectShowCallback::AddRef() {
  return 1;
}

STDMETHODIMP_(ULONG) VideoCaptureDirectShowCallback::Release() {
  return 2;
}


// ---------------------------------------------------------------------
// VideoCaptureDirectShow 
// ---------------------------------------------------------------------

int VideoCaptureDirectShow::num_com_init = 0;

VideoCaptureDirectShow::VideoCaptureDirectShow(void)
  :width(0)
  ,height(0)
  ,is_graph_setup(false)
  ,graph(NULL)
  ,builder(NULL)
  ,capture_device_filter(NULL)
  ,media_control(NULL)
  ,sample_grabber_filter(NULL)
  ,sample_grabber_iface(NULL)
  ,frame_cb(NULL)
  ,frame_user(NULL)
  ,callback_type(BUFFERED_CALLBACK)
{
  if(!num_com_init) {
    HRESULT hr = NULL;
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if(hr == S_OK) {
      printf("VERBOSE: COM initialized.\n");
    }
    else if(hr == S_FALSE) {
      printf("WARNING: COM is already initailized on this thread.\n");
    }
    else if(hr == RPC_E_CHANGED_MODE) {
      printf("WARNING: COM concurrency model changed.\n");
    }
  }

  num_com_init++;
}


VideoCaptureDirectShow::~VideoCaptureDirectShow(void) {
  close();
}

int VideoCaptureDirectShow::openDevice(int dev, int w, int h, VideoCaptureFormat fmt) {
  if(is_graph_setup) {
    printf("ERROR: graph is already created which means the device is opened already.\n");
    return 0;
  }

  if(!initCaptureGraphBuilder()) {
    printf("ERROR: cannot initialize capture graph builder.\n");
    return 0;
  }

  // -----------------------------------------------------
  // STEP 1: FIND THE DEVICE AND ADD IT TO THE GRAPH
  // -----------------------------------------------------
  // Get enumerator so we can find the device.
  ICreateDevEnum* dev_enum;
  HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, 
                                NULL, 
                                CLSCTX_INPROC_SERVER, 
                                IID_PPV_ARGS(&dev_enum)
                                );

  if(FAILED(hr)) {
    printf("ERROR: cannot create system device enumerator.\n");
    return 0;
  }

  IEnumMoniker* enum_moniker;
  hr = dev_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enum_moniker, 0);
  if(hr == S_FALSE) {
    printf("ERROR: cannot create video input device enumerator.\n");
    dev_enum->Release();
    dev_enum = NULL;
    return 0;
  }

  // Iterate over devices and select the one passed into this function.
  bool device_found = false;
  int dx = 0;
  IMoniker* moniker = NULL;
  while(enum_moniker->Next(1, &moniker, NULL) == S_OK) {

    if(dx == dev) {
      hr = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&capture_device_filter);
      if(FAILED(hr)) {
        printf("ERROR: Cannot bind moniker to base filter.\n");
        break;
      }

      hr = graph->AddFilter(capture_device_filter, L"Roxlu Capture Filter");
      if(FAILED(hr)) {
        printf("ERROR: cannot add filter.\n");
        break;
      }

      device_found = true;
      moniker->Release();
      moniker = NULL;
      break;
    }
    ++dx;
  }

  dev_enum->Release();
  dev_enum = NULL;
  
  if(!device_found) {
    printf("ERROR: Cannot find the device index to open.\n");
    return 0;
  }

  // -----------------------------------------------------
  // STEP 2: Check if we 're capable of using the given 
  // format and sizes.
  // -----------------------------------------------------
  if(!isFormatSupported(fmt, w, h, 1)) {
    printf("ERROR: the given format is not supported.\n");
    return 0;
  }

  // -----------------------------------------------------
  // STEP 3: CREATE A SAMPLE GRABBER HOOK TO GET FRAMES
  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd407288(v=vs.85).aspx
  // -----------------------------------------------------
  hr = graph->QueryInterface(IID_IMediaControl, (void**)&media_control);
  RETURN_IF_FAILED(hr, "ERROR: cannot query IID_MediaControl.\n", 0);

  hr = CoCreateInstance(CLSID_SampleGrabber, 
                        NULL, 
                        CLSCTX_INPROC_SERVER, 
                        IID_IBaseFilter, 
                        (void**)&sample_grabber_filter);
  RETURN_IF_FAILED(hr, "ERROR: failed to aquire a CLSID_SampleGrabber.\n", 0);


  hr = graph->AddFilter(sample_grabber_filter, L"SampleGrabber");
  RETURN_IF_FAILED(hr, "ERROR: cannot add sample grabber filter to the graph.\n", 0);

  hr = sample_grabber_filter->QueryInterface(IID_ISampleGrabber, (void**)&sample_grabber_iface);
  RETURN_IF_FAILED(hr, "ERROR: cannot query the IID_ISampleGrabber", 0);

  hr = sample_grabber_iface->SetBufferSamples(TRUE);
  RETURN_IF_FAILED(hr, "ERROR: cannot set buffer samples\n", 0);
  
  hr = sample_grabber_iface->SetOneShot(FALSE);
  RETURN_IF_FAILED(hr, "ERROR: cannot set the one shot feature.\n", 0);

  sample_grabber_listener = new VideoCaptureDirectShowCallback(this);
  hr = sample_grabber_iface->SetCallback(sample_grabber_listener, 1);
  RETURN_IF_FAILED(hr, "ERROR: cannot set the grabber callback.\n", 0);

  AM_MEDIA_TYPE mt;
  ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
  mt.majortype = MEDIATYPE_Video;
  //mt.subtype = MEDIASUBTYPE_RGB24;
  //mt.subtype = MEDIASUBTYPE_YUY2;
  mt.subtype = videoCaptureFormatToMediaCaptureFormat(fmt);
  mt.formattype = FORMAT_VideoInfo;

  hr = sample_grabber_iface->SetMediaType(&mt); // we could ask another type here (e.g. YUV)
  RETURN_IF_FAILED(hr, "ERROR: cannot set media type.\n", 0);

  //  AM_MEDIA_TYPE mt;
  sample_grabber_iface->GetConnectedMediaType(&mt);
  printAmMediaType(&mt);

  // @todo THIS COULD HELP THE PERFORMANCE:
  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd377588(v=vs.85).aspx
  // See "Run the graph" here:
  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd407288(v=vs.85).aspx

  // -------------------------------------------------------------
  // STEP 4: CREATE NULL RENDERER; THE VIDEO NEEDS AN 'END' POINT
  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd407288(v=vs.85).aspx
  // ------------------------------------------------------------- 
  hr = CoCreateInstance(CLSID_NullRenderer, 
                        NULL, 
                        CLSCTX_INPROC_SERVER, 
                        IID_IBaseFilter, 
                        (void**)&null_renderer_filter);
  RETURN_IF_FAILED(hr, "ERROR: cannot query for the null renderer.\n", 0);

  hr = graph->AddFilter(null_renderer_filter, L"NULLRenderer");
  RETURN_IF_FAILED(hr, "ERROR: cannot add the CLSID_NullRenderer to the graph.\n", 0);

  // -------------------------------------------------------------
  // STEP 4: Connect the filters (we're using an helper; but could
  // have done it manually following: 
  // - http://msdn.microsoft.com/en-us/library/windows/desktop/dd387915(v=vs.85).aspx
  // ------------------------------------------------------------- 
  hr = builder->RenderStream(&PIN_CATEGORY_PREVIEW, 
                        &MEDIATYPE_Video, 
                        capture_device_filter,
                        sample_grabber_filter,
                        null_renderer_filter);

  RETURN_IF_FAILED(hr, "ERROR: cannot connect filters in our graph.", 0);

  // From: https://github.com/ofTheo/videoInput/blob/master/videoInputSrcAndDemos/libs/videoInput/videoInput.cpp
  // We release some of the filters as in refered link it's mentioned this sovles freezes
  // capture_device_filter->Release(); // not releasing because we want to use this in "printVerboseInfo"
  // capture_device_filter = NULL;
  sample_grabber_filter->Release();
  sample_grabber_filter = NULL;
  null_renderer_filter->Release();
  null_renderer_filter = NULL;

  is_graph_setup = true;
  if(!saveGraphToFile()) {
    printf("ERROR: cannot save graph to file.\n");
    return 0;
  }
  return 1;
}

// int VideoCaptureDirectShow::getNumBytesPerFrame(VideoCaptureFormat fmt, int w, int h) {
  
// }

int VideoCaptureDirectShow::startCapture() {
  if(!is_graph_setup) {
    printf("ERROR: first open a device before you start the capture.\n");
    return 0;
  }
  HRESULT hr = media_control->Run();
  
  RETURN_IF_FAILED(hr, "ERROR: call Run() on the MediaControl fails.\n", 0);
  return 1;
}

int VideoCaptureDirectShow::stopCapture() {
  if(!is_graph_setup) {
    printf("ERROR: cannot stopCapture() when the graph hasn't been setup, open the device first\n");
    return 0;
  }
  HRESULT hr = media_control->Stop();
  RETURN_IF_FAILED(hr, "ERROR: cannot stop, media_control->Stop() return an error.\n", 0);
  return 1;
}

// We use the CaptureGraphBuilder2 Interface to create a capture graph
bool VideoCaptureDirectShow::initCaptureGraphBuilder() {
  if(builder != NULL) {
    return true;
  }

  HRESULT hr = NULL;
  hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, 
                        NULL,
                        CLSCTX_INPROC_SERVER,
                        IID_ICaptureGraphBuilder2,
                        (void**)&builder);
  
  if(!SUCCEEDED(hr)) {
    printf("ERROR: cannot initialize capture graph builder.\n");
    return false;
  }

  hr = CoCreateInstance(CLSID_FilterGraph,
                        0, 
                        CLSCTX_INPROC_SERVER, 
                        IID_IGraphBuilder, 
                        (void**)&graph);
  if(!SUCCEEDED(hr)) {
    printf("ERROR: cannot initialize filter graph manager.\n");
    builder->Release();
    builder = NULL;
    return false;
  }

  builder->SetFiltergraph(graph);
  return true;
}

int VideoCaptureDirectShow::listDevices() {
  ICreateDevEnum* dev_enum;
  HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, 
                                NULL, 
                                CLSCTX_INPROC_SERVER, 
                                IID_PPV_ARGS(&dev_enum)
                                );

  if(SUCCEEDED(hr)) {
    IEnumMoniker* moniker;
    
    hr = dev_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &moniker, 0);
    if(hr == S_FALSE) {
      dev_enum->Release();
      return 0;
    }

    fillDeviceInformationList(moniker);
    if(devices.size()) {
      printf("--------------------------------------------------\n");
      for(size_t i = 0; i < devices.size(); ++i) {
        printf("[%d] %s\n", int(i), devices[i].name.c_str());
      }
      printf("--------------------------------------------------\n");
    }
    return 1;
  }
  else {
    printf("ERROR: cannot list devices; system device enumerator failed.\n");
    return 0;
  }
}

// Test if a format is supported and if 'set != 0' we will also try to set it.
int VideoCaptureDirectShow::isFormatSupported(VideoCaptureFormat fmt, int w, int h, int set) {
  if(builder == NULL || capture_device_filter == NULL) {
    printf("ERROR: cannot check if format is supported because device filter is still NULL.\n");
    return 0;
  }

  IAMStreamConfig* conf = NULL;
  HRESULT hr = builder->FindInterface(&PIN_CATEGORY_CAPTURE,
                                      &MEDIATYPE_Video,
                                      capture_device_filter,
                                      IID_IAMStreamConfig,
                                      (void**)&conf);

  if(hr == E_NOINTERFACE) {  printf("ERROR: cannot find IID_IAMStreamConfig interface\n"); return 0; }
  if(hr == E_FAIL) { printf("ERROR: IID_IAMStreamConfig fail\n");  return 0; }
  if(hr == E_POINTER) { printf("ERROR: IID_IAMStreamConfig passed null\n"); return 0; } 

  int count = 0;
  int size = 0;
  hr = conf->GetNumberOfCapabilities(&count, &size);
  if(FAILED(hr)) {
    printf("ERROR: cannot get the number of capabilities.\n");
    conf->Release();
    conf = NULL;
    return 0;
  }

  if(size != sizeof(VIDEO_STREAM_CONFIG_CAPS)) {
    printf("ERROR: returned size from GetNumberOfCapabilities is not the same as VIDEO_STREAM_CONFIG_CAPS.\n");
    conf->Release();
    conf = NULL;
    return 0;
  }
  int found = 0;
  for(int i = 0; i < count ; ++i) {
    VIDEO_STREAM_CONFIG_CAPS caps;
    AM_MEDIA_TYPE* mt; // @todo how do we delete/free this?
    hr = conf->GetStreamCaps(i, &mt, (BYTE*)&caps);
    if(FAILED(hr)) {
      printf("ERROR: GetStreamCaps() failed in IsFormatSupported().\n");
      continue;
    }

    if(mt->majortype == MEDIATYPE_Video && 
       mt->subtype == videoCaptureFormatToMediaCaptureFormat(fmt) &&
       mt->formattype == FORMAT_VideoInfo && 
       mt->cbFormat >= sizeof(VIDEOINFOHEADER) && 
       mt->pbFormat != NULL)
    {
      VIDEOINFOHEADER* info = (VIDEOINFOHEADER*)mt->pbFormat;
      found = 1;

      if(set) {

        info->bmiHeader.biWidth = w;
        info->bmiHeader.biHeight = h;
        info->bmiHeader.biSizeImage = DIBSIZE(info->bmiHeader);

        hr = conf->SetFormat(mt);
        if(FAILED(hr)) {
          printf("ERROR: cannot set the format in isFormatSupported! \n");
          found = 0;
        }

        width = w;
        height = h;
        deleteMediaType(mt);
        break;
      }
      //printf(">>>>>>>> %d, %d\n", info->bmiHeader.biWidth, info->bmiHeader.biHeight);
    }
    deleteMediaType(mt);
  }
  return found;
}


int VideoCaptureDirectShow::printVerboseInfo() {
  if(!is_graph_setup) {
    printf("ERROR: you need to open a device before calling printVerboseInfo()\n");
    return 0;
  }
  printf("> %p\n", capture_device_filter);
  IAMStreamConfig* conf;
  HRESULT hr = NULL;
  hr = builder->FindInterface(&PIN_CATEGORY_CAPTURE, // we use capture here because webcams don't have a preview pin
                            &MEDIATYPE_Video, 
                            capture_device_filter, 
                            IID_IAMStreamConfig,
                            (void**)&conf);
  if(hr == E_NOINTERFACE) {  printf("ERROR: cannot find IID_IAMStreamConfig interface\n"); return 0; }
  if(hr == E_FAIL) { printf("ERROR: IID_IAMStreamConfig fail\n");  return 0; }
  if(hr == E_POINTER) { printf("ERROR: IID_IAMStreamConfig passed null\n"); return 0; } 

  // Get preferred format
  AM_MEDIA_TYPE* mt;
  hr = conf->GetFormat(&mt);
  if(FAILED(hr)) {
    printf("ERROR: cannot get format for current device.\n");
    conf->Release();
    conf = NULL;
    return 0;
  }

  printf("--------------------------------------------------\n");
  printf("Preferred media type:\n");
  printAmMediaType(mt);
  printf("--------------------------------------------------\n");
  
  int count = 0;
  int size = 0;
  hr = conf->GetNumberOfCapabilities(&count, &size);
  if(FAILED(hr)) {
    printf("ERROR: cannot GetNumberOfCapabilities()\n");
    conf->Release();
    conf = NULL;
    return 0;
  }

  if(size != sizeof(VIDEO_STREAM_CONFIG_CAPS)) {
    printf("ERROR: capabilities size incorrect.\n");
    conf->Release();
    conf = NULL;
  }

  for(int i = 0; i < count; ++i) {
    VIDEO_STREAM_CONFIG_CAPS cap; 
    hr = conf->GetStreamCaps(i, &mt, (BYTE*)&cap);
    if(FAILED(hr)) {
      printf("WARNING: GetStreamCaps for index: %d failed.\n", i);
      continue;
    }

    // if this AM_MEDIA_TYPE suits your needs you need to set it 
    // see here for an example: http://www.microsoftfaqs.com/msg/14713491.aspx
    // backup: https://gist.github.com/bd9e4982ce2e7da2fcca
    printAmMediaType(mt);
    deleteMediaType(mt);

  }
  conf->Release();
  conf = NULL;
  return 1;
}

void VideoCaptureDirectShow::printAmMediaType(AM_MEDIA_TYPE* mt) {
  /*
  printf("AM_MEDIA_TYPE.majortype: %s\n", mediaFormatMajorTypeToString(mt->majortype).c_str());
  printf("AM_MEDIA_TYPE.subtype: %s\n", mediaFormatSubTypeToString(mt->subtype).c_str());
  printf("AM_MEDIA_TYPE.bFixedSizeSamples: %c\n", (mt->bFixedSizeSamples == TRUE) ? 'y' : 'n');
  printf("AM_MEDIA_TYPE.bTemporalCompression: %c\n", (mt->bTemporalCompression == TRUE) ? 'y' : 'n');
  printf("AM_MEDIA_TYPE.lSampleSize: %Iu\n", mt->lSampleSize);
  printf("AM_MEDIA_TYPE.formattype: %s\n", mediaFormatFormatTypeToString(mt->formattype));
  printf("AM_MEDIA_TYPE.cbFormat: %Iu\n", mt->cbFormat);
  if(mt->formattype == FORMAT_VideoInfo && mt->cbFormat >= sizeof(VIDEOINFOHEADER)) {
    VIDEOINFOHEADER* ih = (VIDEOINFOHEADER*)mt->pbFormat;
    printf("VIDEOINFOHEADER - width: %Iu\n", LONG(ih->bmiHeader.biWidth));
    printf("VIDEOINFOHEADER - height: %Iu\n", LONG(ih->bmiHeader.biHeight));
    // from fps in seconds to nano example:  (1/30) * 10,000,000
    // from fps in 100 nano units to seconds:
    double fps = 1.0 / (double(ih->AvgTimePerFrame) / 10000000.0f); 
    printf("VIDEOINFOHEADER - fps: %f\n", fps);
  }
  */
  //  printf("majortype: %s, ", mediaFormatMajorTypeToString(mt->majortype).c_str());
  printf("subtype: %s, ", mediaFormatSubTypeToString(mt->subtype).c_str());
  //printf("bFixedSizeSamples: %c, ", (mt->bFixedSizeSamples == TRUE) ? 'y' : 'n');
  //printf("bTemporalCompression: %c, ", (mt->bTemporalCompression == TRUE) ? 'y' : 'n');
  printf("lSampleSize: %Iu, ", mt->lSampleSize);
  //printf("formattype: %s, ", mediaFormatFormatTypeToString(mt->formattype));
  // printf("cbFormat: %Iu, ", mt->cbFormat);
  if(mt->formattype == FORMAT_VideoInfo && mt->cbFormat >= sizeof(VIDEOINFOHEADER)) {
    VIDEOINFOHEADER* ih = (VIDEOINFOHEADER*)mt->pbFormat;
    printf("width: %Iu, ", LONG(ih->bmiHeader.biWidth));
    printf("height: %Iu, ", LONG(ih->bmiHeader.biHeight));
    // from fps in seconds to nano example:  (1/30) * 10,000,000
    // from fps in 100 nano units to seconds:
    double fps = 1.0 / (double(ih->AvgTimePerFrame) / 10000000.0f); 
    printf("fps: %f\n", fps);
  }
  else {
    printf("\n");
  }
}

void VideoCaptureDirectShow::fillDeviceInformationList(IEnumMoniker* enumerator) {
  IMoniker* moniker = NULL;

  while(enumerator->Next(1, &moniker, NULL) == S_OK) {

    IPropertyBag* bag;
    HRESULT hr = moniker->BindToStorage(0, 0, IID_PPV_ARGS(&bag));
    if(FAILED(hr)) {
      moniker->Release();
      continue;
    }

    VideoCaptureDirectShowDevice device_info;
    VARIANT var;
    VariantInit(&var);
    
    hr = bag->Read(L"Description", &var, 0);
    if(FAILED(hr)) {
      hr = bag->Read(L"FriendlyName", &var, 0);
    }
    if(SUCCEEDED(hr)) {
      int count = 0;
      while(var.bstrVal[count] != 0x00 && count < 1024) {
        device_info.name.push_back(var.bstrVal[count]);
        count++;
      }
    }
    else {
      device_info.name = "UNKNOWN";
    }

    bag->Release();
    bag = NULL;

    moniker->Release();
    moniker = NULL;

    devices.push_back(device_info);
  }
}

void VideoCaptureDirectShow::close() {
  if(!is_graph_setup) {
    return;
  }

  if(media_control != NULL) {
    HRESULT hr = media_control->Stop();
    if(FAILED(hr)) {
      printf("ERROR: IMediaControl::Stop() fails.\n");
    }
  }

  if(capture_device_filter) {
    nukeDownStream(capture_device_filter);
  }

  if(null_renderer_filter != NULL) {
    null_renderer_filter->Release();
    null_renderer_filter = NULL;
  }

  if(capture_device_filter != NULL) {
    capture_device_filter->Release();
    capture_device_filter = NULL;
  }

  if(sample_grabber_iface != NULL) {
    sample_grabber_iface->SetCallback(NULL, 0); 
    sample_grabber_iface->SetCallback(NULL, 1); 
    sample_grabber_iface->Release();
    sample_grabber_iface = NULL;
  }

  if(sample_grabber_filter != NULL) {
    sample_grabber_filter->Release();
    sample_grabber_filter = NULL;
  }

  if(sample_grabber_listener != NULL) {
    delete sample_grabber_listener;
    sample_grabber_listener = NULL;
  }

  if(builder != NULL) {
    builder->Release();
    builder = NULL;
  }

  if(media_control != NULL) {
    media_control->Release();
    media_control = NULL;
  }

  if(graph != NULL) {
    destroyGraph();
    graph->Release();
    graph = NULL;
  }
  
  delete null_renderer_filter;
  delete sample_grabber_filter;
  delete sample_grabber_iface;
  delete media_control;
  delete capture_device_filter;
  delete builder;
  delete graph;
  is_graph_setup = false;
  
  if(num_com_init > 0) {
    num_com_init--;
  }
  if(num_com_init == 0) {
    CoUninitialize();
  }
}


// From SDK
void VideoCaptureDirectShow::nukeDownStream(IBaseFilter* filter) {
  IPin *p, *to;
  ULONG u;
  IEnumPins *pins = NULL;
  PIN_INFO pininfo;
  HRESULT hr = filter->EnumPins(&pins);
  pins->Reset();
  while(hr == NOERROR) {
    hr = pins->Next(1, &p, &u);
    if(hr == S_OK && p)  {
      p->ConnectedTo(&to);
      if(to) {
        hr = to->QueryPinInfo(&pininfo);
        if(hr == NOERROR){
          if(pininfo.dir == PINDIR_INPUT) {
            nukeDownStream(pininfo.pFilter);
            graph->Disconnect(to);
            graph->Disconnect(p);
            graph->RemoveFilter(pininfo.pFilter);
          }
          pininfo.pFilter->Release();
          pininfo.pFilter = NULL;
        }
        to->Release();
      }
      p->Release();
    }
  } 

  if(pins) {
    pins->Release();
  }
}

// From SDK and 
void VideoCaptureDirectShow::destroyGraph() {
  HRESULT hr = NULL;
  int r = 0;
  int num_filters=0;

  while(hr == NOERROR) {
    IEnumFilters* filter_enumerator = NULL;
    ULONG num_fetched;

    hr = graph->EnumFilters(&filter_enumerator);
    if(FAILED(hr)) { 
      printf("WARNING: failed to enumerate filter.\n");
      return;
    }

    IBaseFilter* filter = NULL;
    if (filter_enumerator->Next(1, &filter, &num_fetched) == S_OK) {
      FILTER_INFO filter_info = {0};
      hr = filter->QueryFilterInfo(&filter_info);
      filter_info.pGraph->Release();
          
      // get the name of the filter
      int count = 0;
      char buffer[255];
      memset(buffer, 0, 255 * sizeof(char));
      while(filter_info.achName[count] != 0x00) {
        buffer[count] = filter_info.achName[count];
        count++;
      }
      printf("VERBOSE: removing filter %s.\n", buffer);

      hr = graph->RemoveFilter(filter);
      if(FAILED(hr)) { 
        printf("ERROR: graph->RemoveFilter() failed.\n");
        return;
      }

      filter->Release();
      filter = NULL;
    }
    else { 
      break;
    }
    filter_enumerator->Release();
    filter_enumerator= NULL;
  }
}

// Debugging / verbosity
// http://msdn.microsoft.com/en-us/library/windows/desktop/dd390660(v=vs.85).aspx
std::string VideoCaptureDirectShow::mediaFormatMajorTypeToString(GUID type) {
  if(type == MEDIATYPE_AnalogAudio) { return "MEDIATYPE_AnalogAudio"; }
  else if(type == MEDIATYPE_AnalogVideo) { return "MEDIATYPE_AnalogVideo";  }
  else if(type == MEDIATYPE_Audio) { return "MEDIATYPE_Audio"; }
  else if(type == MEDIATYPE_AUXLine21Data) { return "MEDIATYPE_AUXLine21Data"; } 
  else if(type == MEDIATYPE_File) { return "MEDIATYPE_File"; }
  else if(type == MEDIATYPE_Interleaved) { return "MEDIATYPE_Interleaved"; } 
  else if(type == MEDIATYPE_LMRT) { return "MEDIATYPE_LMRT"; } 
  else if(type == MEDIATYPE_Midi) { return "MEDIATYPE_Midi"; } 
  else if(type == MEDIATYPE_MPEG2_PES) { return "MEDIATYPE_MPEG2_PES"; } 
  else if(type == MEDIATYPE_MPEG2_SECTIONS) { return "MEDIATYPE_MPEG2_SECTIONS"; } 
  else if(type == MEDIATYPE_ScriptCommand) { return "MEDIATYPE_ScriptCommand"; }
  else if(type == MEDIATYPE_Stream) { return "MEDIATYPE_Stream"; } 
  else if(type == MEDIATYPE_Text) { return "MEDIATYPE_Text"; }
  else if(type == MEDIATYPE_Timecode) { return "MEDIATYPE_Timecode"; } 
  else if(type == MEDIATYPE_URL_STREAM) { return "MEDIATYPE_URL_STREAM"; } 
  else if(type == MEDIATYPE_VBI) { return "MEDIATYPE_VBI"; } 
  else if(type == MEDIATYPE_Video) { return "MEDIATYPE_Video"; } 
  return "UNKNOWN - NOT ADDED TO LIST";
}

// Debugging / verbosity 
// http://msdn.microsoft.com/en-us/library/windows/desktop/dd407253(v=vs.85).aspx
std::string VideoCaptureDirectShow::mediaFormatSubTypeToString(GUID type) {
  if(type == MEDIASUBTYPE_RGB1) { return "MEDIASUBTYPE_RGB1"; } 
  else if(type == MEDIASUBTYPE_RGB4) { return "MEDIASUBTYPE_RGB4"; }
  else if(type == MEDIASUBTYPE_RGB8) { return "MEDIASUBTYPE_RGB8"; }
  else if(type == MEDIASUBTYPE_RGB555) { return "MEDIASUBTYPE_RGB555"; }
  else if(type == MEDIASUBTYPE_RGB565) { return "MEDIASUBTYPE_RGB565"; }
  else if(type == MEDIASUBTYPE_RGB24) { return "MEDIASUBTYPE_RGB24"; }
  else if(type == MEDIASUBTYPE_RGB32) { return "MEDIASUBTYPE_RGB32"; }
  else if(type == MEDIASUBTYPE_ARGB1555) { return "MEDIASUBTYPE_ARGB1555"; }
  else if(type == MEDIASUBTYPE_ARGB32) { return "MEDIASUBTYPE_ARGB32"; }
  else if(type == MEDIASUBTYPE_ARGB4444) { return "MEDIASUBTYPE_ARGB4444"; } 
  else if(type == MEDIASUBTYPE_A2R10G10B10) { return "MEDIASUBTYPE_A2R10G10B10"; }
  else if(type == MEDIASUBTYPE_A2B10G10R10) { return "MEDIASUBTYPE_A2B10G10R10"; }

  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd391027(v=vs.85).aspx
  else if(type == MEDIASUBTYPE_AYUV) { return "MEDIASUBTYPE_AYUV"; } 
  else if(type == MEDIASUBTYPE_YUY2) { return "MEDIASUBTYPE_YUY2"; }
  else if(type == MEDIASUBTYPE_UYVY) { return "MEDIASUBTYPE_UYVY"; } 
  else if(type == MEDIASUBTYPE_IMC1) { return "MEDIASUBTYPE_IMC1"; } 
  else if(type == MEDIASUBTYPE_IMC3) { return "MEDIASUBTYPE_IMC3"; }
  else if(type == MEDIASUBTYPE_IMC2) { return "MEDIASUBTYPE_IMC2"; } 
  else if(type == MEDIASUBTYPE_IMC4) { return "MEDIASUBTYPE_IMC4"; } 
  else if(type == MEDIASUBTYPE_YV12) { return "MEDIASUBTYPE_YV12"; }
  else if(type == MEDIASUBTYPE_NV12) { return "MEDIASUBTYPE_NV12"; } 
  else if(type == MEDIASUBTYPE_IF09) { return "MEDIASUBTYPE_IF09"; }
  else if(type == MEDIASUBTYPE_IYUV) { return "MEDIASUBTYPE_IYUV"; }  // same as MEDIASUBTYPE_I420
  else if(type == MEDIASUBTYPE_Y211) { return "MEDIASUBTYPE_Y211"; }
  else if(type == MEDIASUBTYPE_Y411) { return "MEDIASUBTYPE_Y411"; }
  else if(type == MEDIASUBTYPE_Y41P) { return "MEDIASUBTYPE_Y41P"; }
  else if(type == MEDIASUBTYPE_YVU9) { return "MEDIASUBTYPE_YVU9"; } 
  else if(type == MEDIASUBTYPE_YVYU) { return "MEDIASUBTYPE_YVYU"; }
  

  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd757808(v=vs.85).aspx
  // wmcodecdsp.h
  /*
  else if(type == MEDIASUBTYPE_AVC1) { return "MEDIASUBTYPE_AVC1"; } 
  else if(type == MEDIASUBTYPE_H264) { return "MEDIASUBTYPE_H264"; }
  else if(type == MEDIASUBTYPE_h264) { return "MEDIASUBTYPE_h264"; }
  else if(type == MEDIASUBTYPE_X264) { return "MEDIASUBTYPE_X264"; }
  else if(type == MEDIASUBTYPE_x264) { return "MEDIASUBTYPE_x264"; }
  */

  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd390688(v=vs.85).aspx
  else if(type == MEDIASUBTYPE_CFCC) { return "MEDIASUBTYPE_CFCC"; }
  else if(type == MEDIASUBTYPE_CLJR) { return "MEDIASUBTYPE_CLJR"; } 
  else if(type == MEDIASUBTYPE_CPLA) { return "MEDIASUBTYPE_CPLA"; } 
  else if(type == MEDIASUBTYPE_CLPL) { return "MEDIASUBTYPE_CLPL"; } 
  else if(type == MEDIASUBTYPE_IJPG) { return "MEDIASUBTYPE_IJPG"; }
  else if(type == MEDIASUBTYPE_MDVF) { return "MEDIASUBTYPE_MDVF"; }
  else if(type == MEDIASUBTYPE_MJPG) { return "MEDIASUBTYPE_MJPG"; } 
  else if(type == MEDIASUBTYPE_Overlay) { return "MEDIASUBTYPE_Overlay"; } 
  else if(type == MEDIASUBTYPE_Plum) { return "MEDIASUBTYPE_Plum"; } 
  else if(type == MEDIASUBTYPE_QTJpeg) { return "MEDIASUBTYPE_QTJpeg"; } 
  else if(type == MEDIASUBTYPE_QTMovie) { return "MEDIASUBTYPE_QTMovie"; }
  else if(type == MEDIASUBTYPE_QTRle) { return "MEDIASUBTYPE_QTRle"; }
  else if(type == MEDIASUBTYPE_QTRpza) { return "MEDIASUBTYPE_QTRpza"; } 
  else if(type == MEDIASUBTYPE_QTSmc) { return "MEDIASUBTYPE_QTSmc"; } 
  else if(type == MEDIASUBTYPE_TVMJ) { return "MEDIASUBTYPE_TVMJ"; } 
  else if(type == MEDIASUBTYPE_VPVBI) { return "MEDIASUBTYPE_VPVBI"; } 
  else if(type == MEDIASUBTYPE_VPVideo) { return "MEDIASUBTYPE_VPVideo"; } 
  else if(type == MEDIASUBTYPE_WAKE) { return "MEDIASUBTYPE_WAKE"; }

  // http://msdn.microsoft.com/en-us/library/windows/desktop/dd390712(v=vs.85).aspx
  else if(type == MEDIASUBTYPE_MPEG1System) { return "MEDIASUBTYPE_MPEG1System"; } 
  else if(type == MEDIASUBTYPE_MPEG1VideoCD) { return "MEDIASUBTYPE_MPEG1VideoCD"; }
  else if(type == MEDIASUBTYPE_MPEG1Packet) { return "MEDIASUBTYPE_MPEG1Packet"; }
  else if(type == MEDIASUBTYPE_MPEG1Payload) { return "MEDIASUBTYPE_MPEG1Payload"; } 
  else if(type == MEDIASUBTYPE_MPEG1Packet) { return "MEDIASUBTYPE_MPEG1Packet"; } 
  else if(type == MEDIASUBTYPE_MPEG1Video) { return "MEDIASUBTYPE_ MPEG1Video"; }
  else if(type == MEDIASUBTYPE_MPEG1Audio) { return "MEDIASUBTYPE_ MPEG1Audio"; } 

  else if(type == MEDIASUBTYPE_MPEG2_VIDEO) { return "MEDIASUBTYPE_MPEG2_VIDEO"; } 
  else if(type == MEDIASUBTYPE_DOLBY_AC3) { return "MEDIASUBTYPE_DOLBY_AC3"; } 
  else if(type == MEDIASUBTYPE_MPEG2_AUDIO) { return "MEDIASUBTYPE_MPEG2_AUDIO";} 
  else {  return "UNKOWN NOT ADDED TO LIST.\n";  }
}


std::string VideoCaptureDirectShow::mediaFormatFormatTypeToString(GUID type) {
  if(type == FORMAT_DvInfo) { return "FORMAT_DvInfo"; } 
  else if(type == FORMAT_MPEG2Video) { return "FORMAT_MPEG2Video"; } 
  else if(type == FORMAT_MPEGStreams) { return "FORMAT_MPEGStreams"; } 
  else if(type == FORMAT_MPEGVideo) { return "FORMAT_MPEGVideo"; } 
  else if(type == FORMAT_None) { return "FORMAT_None"; } 
  else if(type == FORMAT_VideoInfo) { return "FORMAT_VideoInfo"; } 
  else if(type == FORMAT_VideoInfo2) { return "FORMAT_VideoInfo2"; } 
  else if(type == FORMAT_WaveFormatEx) { return "FORMAT_WaveFormatEx"; }
  else if(type == GUID_NULL) { return "GUID_NULL"; } 
  else {  return "UNKOWN NOT ADDED TO LIST.\n";  }
}

// http://msdn.microsoft.com/en-us/library/windows/desktop/dd377551(v=vs.85).aspx
int VideoCaptureDirectShow::saveGraphToFile() {
  const WCHAR stream_name[] = L"ActiveMovieGraph"; 
  const WCHAR path[] = L"data0.grf";
  HRESULT hr;
    
  IStorage *storage = NULL;
  hr = StgCreateDocfile(path,
                        STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
                        0, &storage);
  if(FAILED(hr)){
    return 0;
  }

  IStream *stream;
  hr = storage->CreateStream(stream_name,
                              STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
                              0, 0, &stream);
  if(FAILED(hr)) {
    storage->Release();    
    return 0;
  }

  IPersistStream *persist = NULL;
  graph->QueryInterface(IID_IPersistStream, (void**)&persist);
  hr = persist->Save(stream, TRUE);
  stream->Release();
  persist->Release();

  if(SUCCEEDED(hr)) {
    hr = storage->Commit(STGC_DEFAULT);
  }
  storage->Release();
  return 1;
}

GUID VideoCaptureDirectShow::videoCaptureFormatToMediaCaptureFormat(VideoCaptureFormat fmt) {
  switch(fmt) {
    case VC_FMT_RGB24: return MEDIASUBTYPE_RGB24; break;
    case VC_FMT_YUY2: return MEDIASUBTYPE_YUY2; break;
    case VC_FMT_I420: return MEDIASUBTYPE_IYUV; break;
    default: {
      printf("ERROR: we have not yet added a conversion from %d to a DirectShow format.\n", fmt);
      ::exit(EXIT_FAILURE);
    }
  };
}

void VideoCaptureDirectShow::deleteMediaType(AM_MEDIA_TYPE* mt) {
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
}
