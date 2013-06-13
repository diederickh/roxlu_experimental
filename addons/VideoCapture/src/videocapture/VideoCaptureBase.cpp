#include <videocapture/VideoCaptureBase.h>
#include <videocapture/Utils.h>
#include <roxlu/core/Log.h>
#include <algorithm>


// TESTING FOR SUPPORTED CAPABILITES
// --------------------------------------------------------------------------------------
bool VideoCaptureBase::isPixelFormatSupported(int device, enum AVPixelFormat fmt) {
  std::vector<AVCapability> caps = getCapabilities(device);
  return std::find_if(caps.begin(), caps.end(), AVCapabilityFindPixelFormat(fmt)) != caps.end();
}

bool VideoCaptureBase::isSizeSupported(int device, int width, int height) {
  std::vector<AVCapability> caps = getCapabilities(device);
  return std::find_if(caps.begin(), caps.end(), AVCapabilityFindSize(width, height)) != caps.end();
}

bool VideoCaptureBase::isFrameRateSupported(int device, double fps) {
  std::vector<AVCapability> caps = getCapabilities(device);
  return std::find_if(caps.begin(), caps.end(), AVCapabilityFindFrameRate(fps)) != caps.end();
}

std::vector<AVSize> VideoCaptureBase::getSupportedSizes(int device) {
  std::vector<AVSize> result;
  std::vector<AVCapability> caps = getCapabilities(device);
 
  for(std::vector<AVCapability>::iterator it = caps.begin(); it != caps.end(); ++it) {
    AVCapability& cap = *it;
    std::vector<AVSize>::iterator it_found = std::find(result.begin(), result.end(), cap.size);
    if(it_found == result.end()) {
      result.push_back(cap.size);
    }
  }
 
  return result;
}

std::vector<AVPixelFormat> VideoCaptureBase::getSupportedPixelFormats(int device, int width, int height) {
  std::vector<enum AVPixelFormat> result;
  std::vector<AVCapability> caps = getCapabilities(device);
 
  for(std::vector<AVCapability>::iterator it = caps.begin(); it != caps.end(); ++it) {
    AVCapability& cap = *it;
    if(cap.size.width == width && cap.size.height == height) {
      std::vector<AVPixelFormat>::iterator it_found = std::find(result.begin(), result.end(), cap.pixel_format);
      if(it_found == result.end()) {
        result.push_back(cap.pixel_format);
      }
    }
  }
 
  return result;
}

std::vector<AVRational> VideoCaptureBase::getSupportedFrameRates(int device, int width, int height, enum AVPixelFormat fmt) {
  std::vector<AVCapability> caps;
  std::vector<AVRational> result;

  for(std::vector<AVCapability>::iterator it = caps.begin(); it != caps.end(); ++it) {
    AVCapability& cap = *it;
    if(cap.size.width == width && cap.size.height == height && cap.pixel_format == fmt) {
      result.push_back(cap.framerate);
    }
  }

  return result;  
}


bool VideoCaptureBase::getBestMatchingCapability(int device, 
                                                 VideoCaptureSettings cfg, 
                                                 AVCapability& result) 
{
  std::vector<AVCapability> caps = getCapabilities(device);
  for(std::vector<AVCapability>::iterator it = caps.begin(); it != caps.end(); ++it) {
    AVCapability& cap = *it;
    if(cfg.width == cap.size.width 
       && cfg.height == cap.size.height
       && cfg.in_pixel_format == cap.pixel_format) 
      {
        float cap_fps = rx_libav_rational_to_fps(cap.framerate);
        if(cap_fps == cfg.fps) {
          result = cap;
          return true;
        }
      }
  }
  return false;
}


// LOGGING SUPPORTED CAPABILITIES 
// --------------------------------------------------------------------------------------
void VideoCaptureBase::printSupportedFrameRates(int device, 
                                                int width, 
                                                int height,
                                                enum AVPixelFormat fmt) 
{

  std::vector<AVRational> rates = getSupportedFrameRates(device, width, height, fmt);
  if(!rates.size()) {
    RX_VERBOSE("No supported framerates for: %dx%d", width, height);
    return;
  }

  RX_VERBOSE("Supported framerate for device: %d and size: %dx%d", device, width, height); 
  RX_VERBOSE("--------------------------------------------------------------------");
  for(std::vector<AVRational>::iterator it = rates.begin(); it != rates.end(); ++it) {
    AVRational r = *it;
    double fps = 1.0 / (double(r.num) / double(r.den));
    RX_VERBOSE("\t %02.02f fps", fps); 
  }

}

void VideoCaptureBase::printSupportedPixelFormats(int device, int width, int height) {

  std::vector<enum AVPixelFormat> formats = getSupportedPixelFormats(device, width, height);
  if(!formats.size()) {
    RX_VERBOSE("No supported pixel formats for: %dx%d", width, height);
    return;
  }

  RX_VERBOSE("Supported pixel formats for device: %d and size: %dx%d", device, width, height)
  RX_VERBOSE("--------------------------------------------------------------------");
  for(std::vector<enum AVPixelFormat>::iterator it = formats.begin(); it != formats.end(); ++it) {
    enum AVPixelFormat f = *it;
    std::string fmt_name = rx_libav_pixel_format_to_string(f);
    RX_VERBOSE("\t%s", fmt_name.c_str());
  }
}

void VideoCaptureBase::printSupportedSizes(int device) { 

  std::vector<AVSize> sizes = getSupportedSizes(device);
  if(!sizes.size()) {
    RX_VERBOSE("No supported sizes found");
    return;
  }

  RX_VERBOSE("Supported sizes for device: %d", device);
  RX_VERBOSE("--------------------------------------------------------------------");
  for(std::vector<AVSize>::iterator it = sizes.begin(); it != sizes.end(); ++it) {
    AVSize& s = *it;
    RX_VERBOSE("\t%dx%d", s.width, s.height);
  }
}

void VideoCaptureBase::printSupportedPixelFormats(int device) {

  std::vector<AVCapability> caps = getCapabilities(device);
  if(!caps.size()) {
    RX_VERBOSE("No capabilities found for device: %d", device);
    return;
  }

  RX_VERBOSE("Supported pixel formats for device: %d", device);
  RX_VERBOSE("--------------------------------------------------------------------");

  std::set<enum AVPixelFormat> unique_fmts;
  for(std::vector<AVCapability>::iterator it = caps.begin();
      it != caps.end();
      ++it)
    {
      AVCapability cap = *it;
      unique_fmts.insert(cap.pixel_format);
    }

  for(std::set<enum AVPixelFormat>::iterator it = unique_fmts.begin();
      it != unique_fmts.end();
      ++it) 
    {
     
      enum AVPixelFormat pix_fmt = *it;
      RX_VERBOSE("\t%s", rx_libav_pixel_format_to_string(pix_fmt).c_str());
    }
}

void VideoCaptureBase::printCapabilities(int device) {

  std::vector<AVCapability> caps = getCapabilities(device);
  if(!caps.size()) {
    RX_VERBOSE("No capabilities found for device: %d", device);
    return;
  }

  int last_width = 0;
  int last_height = 0;
  enum AVPixelFormat last_pix_fmt = AV_PIX_FMT_NONE;
  
  for(std::vector<AVCapability>::iterator it = caps.begin();
      it != caps.end();
      ++it)
    {
      AVCapability cap = *it;
      if(cap.size.width != last_width 
         || cap.size.height != last_height 
         || cap.pixel_format != last_pix_fmt) 
        {
          RX_VERBOSE("");
          RX_VERBOSE("%s, %dx%d", 
                     rx_libav_pixel_format_to_string(cap.pixel_format).c_str(),
                     cap.size.width,
                     cap.size.height);
          RX_VERBOSE("--------------------------------------------------------------------");
          last_width = cap.size.width;
          last_height = cap.size.height;
          last_pix_fmt = cap.pixel_format;
      }
      
      AVRational r = cap.framerate;
      double fps = 1.0 / (double(r.num) / double(r.den));
      RX_VERBOSE("\tFPS %2.02f ", fps);
    }
}
