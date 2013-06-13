#import <videocapture/mac/VideoCaptureAVFoundation.h>
#include <videocapture/Utils.h>
#include <roxlu/core/Log.h>

@implementation VideoCaptureAVFoundation

- (id) init {
  
  self = [super init];
  if(self) {
    cb_frame = nil;
    cb_user = nil;
    session = nil;
    input = nil;
    output = nil;
  }
  RX_VERBOSE("ADD @autoreleasepool where necessary!");        
  return self;
}

- (int) listDevices {   
  int c = 0;
  NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
  for(AVCaptureDevice* device in devices) {
    RX_VERBOSE("[%d] : %s", c, [[device localizedName] UTF8String]);
    ++c;
  }
  return [devices count];
}

- (int) openDevice:(int) dev 
         withWidth:(int) w  
         andHeight:(int) h 
         andFormat:(enum AVPixelFormat) fmt 
      andFrameRate:(float) fps
{

  @autoreleasepool { // tmp

    // Get the device
    NSError* error;
    AVCaptureDevice* cap_device = [self getCaptureDevice: dev];
    if(!cap_device) {
      RX_ERROR(ERR_CM_DEVICE_NOT_FOUND);
      return 0;
    }


    input = [AVCaptureDeviceInput deviceInputWithDevice:cap_device error:&error];
    if(!input) {
      RX_ERROR(ERR_CM_CAPTURE_DEVICE);
      return 0;
    }
    [input retain];

    // Create session
    session = [[AVCaptureSession alloc]init];
    if(!session) {
      RX_ERROR(ERR_CM_ALLOC_SESSION);
      return 0;
    }

    [session beginConfiguration];

    // @todo do we really need this?
    NSString* const preset = [self widthHeightToCaptureSessionPreset: w andHeight: h];
    if([session canSetSessionPreset:preset]) {
      session.sessionPreset = preset; 
    }
    else {
      printf("ERROR: Cannot set high capture preset.\n");
      return 0;
    }


    // Check if we can use this input + add it.
    if(![session canAddInput:input]) {
      RX_ERROR(ERR_CM_SESSION_INPUT);
      return 0;
    }

    [session addInput:input];

    // Find format 
    AVFrameRateRange* found_fps;
    bool did_found_format = false;
    //CMFormatDescriptionRef format_ref;
    AVCaptureDeviceFormat* found_format;
    for(AVCaptureDeviceFormat* f in [cap_device formats]) {
      if(![[f mediaType] isEqualToString: AVMediaTypeVideo]) {
        continue;
      }

      CMFormatDescriptionRef fmt_description = [f formatDescription];
      CMPixelFormatType pixel_format_type = CMFormatDescriptionGetMediaSubType(fmt_description);
      int libav_format = [self avFoundationPixelFormatToLibavPixelFormat: pixel_format_type];    
    
      if(libav_format != fmt) {
        continue;
      }

      CMVideoDimensions dims = CMVideoFormatDescriptionGetDimensions([f formatDescription]);
      if(dims.width != w || dims.height != h) {
        continue;
      }
    
      for(AVFrameRateRange* fps_range in [f videoSupportedFrameRateRanges]) {
        AVRational rat;
        CMTime dur = [fps_range maxFrameDuration];
        rat.num = dur.value;
        rat.den = dur.timescale;
        double fps_raw = 1.0 / (double(rat.num) / double(rat.den));
        float fps_corrected = 0.0f;
        char buf[512];
        sprintf(buf, "%2.02f", fps_raw);
        sscanf(buf, "%f", &fps_corrected);
        if(fps_corrected == fps) {
          did_found_format = true;
          //format_ref = fmt_description;
          found_fps = fps_range;
          found_format = f;
          break;
        }
      }
    }

    if(!did_found_format) {
      RX_ERROR(ERR_CM_FORMAT_NOT_FOUND);
      return 0;
    }

    // Set the format type we found
    AVCaptureDevice* d = [input device];
    [d lockForConfiguration:nil];
    [d setActiveFormat: found_format]; /* sets pixel format */
    [d unlockForConfiguration];

    // Configure output
    output = [[AVCaptureVideoDataOutput alloc] init];
    if([self isLibavPixelFormatSupportedByCaptureVideoDataOutput: output pixelFormat: fmt] == 0) {
      RX_ERROR(ERR_CM_PIX_FMT);
      return 0;
    }
    [output retain];

    // Tell the AVCaptureVideoDataOutput to use the specified pixel format 
#if 1
    CMPixelFormatType pixel_format_type = [self libavPixelFormatToAVFoundationPixelFormat: fmt];
    [output setVideoSettings: [NSDictionary dictionaryWithObjectsAndKeys: 
                                                 [NSNumber numberWithInt:pixel_format_type], kCVPixelBufferPixelFormatTypeKey,
                                             [NSNumber numberWithInteger:w],  (id)kCVPixelBufferWidthKey,
                                             [NSNumber numberWithInteger:h], (id)kCVPixelBufferHeightKey,
                                            nil]];
#endif

    // Set FPS 
    [cap_device lockForConfiguration:nil];
    [cap_device setActiveVideoMinFrameDuration: [found_fps minFrameDuration]];
    [cap_device unlockForConfiguration];

    // Setup framegrabber callback
    dispatch_queue_t queue = dispatch_queue_create("Video Queue", DISPATCH_QUEUE_SERIAL);
    [output setSampleBufferDelegate:self queue:queue];
    if(![session canAddOutput:output]) {
      RX_ERROR(ERR_CM_OUTPUT);
      dispatch_release(queue);
      [session commitConfiguration];
      return 0;
    }
    [session addOutput:output];
    dispatch_release(queue);

    [session commitConfiguration];

  } // autorelease - tmp  
  return 1;
}

- (int) closeDevice {
  if(!session) {
    RX_ERROR("Cannot close the device because it hasn't been opened yet.");
    return 0;
  }

  [session stopRunning];
  [output setSampleBufferDelegate:nil queue:dispatch_get_main_queue()];
  [session removeInput:input];
  [session removeOutput:output];
  [session release];
  [input release];
  [output release];
  session = nil;
  input = nil;
  output = nil;

  return 1;
}

- (void) dealloc {
  RX_VERBOSE("DEALLOC THE VIDEOCAPTUREAVFOUNDATION");
  return; // tmp

  if(session) {
    [session stopRunning];
    [session release];
   }

  [input release];
  [output release];

  input = nil;
  output = nil;
  session = nil; 
  cb_frame = nil;
  cb_user = nil;

  [super dealloc];
}

- (int) captureStart {
  [session startRunning];
  return 1;
}

- (int) captureStop {
  [session stopRunning];
  return 1;
}

- (void) captureOutput: (AVCaptureOutput*) captureOutput
 didOutputSampleBuffer: (CMSampleBufferRef) sampleBuffer
        fromConnection: (AVCaptureConnection*) connection 
{
  CVImageBufferRef img_ref = CMSampleBufferGetImageBuffer(sampleBuffer);
  CVPixelBufferLockBaseAddress(img_ref, 0);
  void* base_address = CVPixelBufferGetBaseAddress(img_ref);

  // get number of bytes in the image.
  size_t img_bytes_per_row = CVPixelBufferGetBytesPerRow(img_ref);
  size_t img_height = CVPixelBufferGetHeight(img_ref);
  size_t nbytes = img_bytes_per_row * img_height;

  cb_frame((void*)base_address, nbytes, cb_user);
  CVPixelBufferUnlockBaseAddress(img_ref, 0);

#if 0
  CMFormatDescriptionRef format_desc_ref = CMSampleBufferGetFormatDescription(sampleBuffer);
  FourCharCode active_video_type = CMFormatDescriptionGetMediaSubType(format_desc_ref);
  int av_fmt = CMFormatDescriptionGetMediaSubType(format_desc_ref);
  int libav_fmt = [self avFoundationPixelFormatToLibavPixelFormat: av_fmt];
  std::string libav_fmt_str = rx_libav_pixel_format_to_string((AVPixelFormat)libav_fmt);
  NSString* av_fmt_str = [self getPixelFormatString:av_fmt];
  RX_VERBOSE("Capturing in: %s, AVFoundation name: %s", libav_fmt_str.c_str(), [av_fmt_str UTF8String]);
#endif

}

- (NSString* const) widthHeightToCaptureSessionPreset:(int) w andHeight:(int) h {
  if(w == 320 && h == 240) { return AVCaptureSessionPreset320x240; }
  else if(w == 352 && h == 288) { return AVCaptureSessionPreset352x288; } 
  else if(w == 640 && h == 480) { return AVCaptureSessionPreset640x480; }
  else if(w == 960 && h == 540) { return AVCaptureSessionPreset960x540; }
  else if(w == 1280 && h == 720) { return AVCaptureSessionPreset1280x720; }
  else {
    return AVCaptureSessionPresetHigh; // if no preset matches we return the highest
  }
}

- (void) setFrameCallback:(videocapture_frame_cb)frameCB user:(void*)user {
  cb_frame = frameCB;
  cb_user = user;
}

- (AVCaptureDevice*) getCaptureDevice: (int) device {
  int c = 0;
  NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
  for(AVCaptureDevice* dev in devices) {
    if(c == device) {
      return dev;
    }
    ++c;
  }
  return nil;
}

- (int) getCapabilities: (std::vector<AVCapability>&) result
              forDevice: (int) device 
{
  AVCaptureDevice* d = [self getCaptureDevice:device];
  if(!d) {
    return 0;
  }

  int i = 0;
  for(AVCaptureDeviceFormat* f in [d formats]) {
    if(![[f mediaType] isEqualToString: AVMediaTypeVideo]) {
      ++i;
      continue;
    }
    
    CMFormatDescriptionRef fmt_description = [f formatDescription];
    CMPixelFormatType pixel_format_type = CMFormatDescriptionGetMediaSubType(fmt_description);
    int libav_format = [self avFoundationPixelFormatToLibavPixelFormat: pixel_format_type];    
    CMVideoDimensions dims = CMVideoFormatDescriptionGetDimensions([f formatDescription]);
    
    for(AVFrameRateRange* fps in [f videoSupportedFrameRateRanges]) {
      AVCapability cap;

      if ([fps minFrameRate] != [fps maxFrameRate]) {
        RX_VERBOSE("Need to handle a capability with different min/max framerates");
      }
      else {
        CMTime dur = [fps maxFrameDuration];
        cap.framerate.num = dur.value;
        cap.framerate.den = dur.timescale;
        cap.size.width = dims.width;
        cap.size.height = dims.height;
        cap.pixel_format = (AVPixelFormat)libav_format;
        cap.index = i;

        result.push_back(cap);
      }
    }
    ++i;
  }

  return result.size();
}

// PIXEL FORMAT CONVERSION + INFO
// ----------------------------------------------------------------------------------------------
- (NSString*) pixelFormatToString: (CMPixelFormatType) fmt {
  switch (fmt) {
    case kCMPixelFormat_32ARGB: return @"kCMPixelFormat_32ARGB"; break;
    case kCMPixelFormat_32BGRA: return @"kCMPixelFormat_32BGRA"; break;
    case kCMPixelFormat_24RGB: return @"kCMPixelFormat_24RGB"; break;
    case kCMPixelFormat_16BE555: return @"kCMPixelFormat_16BE555"; break;
    case kCMPixelFormat_16BE565: return @"kCMPixelFormat_16BE565"; break;
    case kCMPixelFormat_16LE555: return @"kCMPixelFormat_16LE555"; break;
    case kCMPixelFormat_16LE565: return @"kCMPixelFormat_16LE565"; break;
    case kCMPixelFormat_16LE5551: return @"kCMPixelFormat_16LE5551"; break;
    case kCMPixelFormat_422YpCbCr8: return @"kCMPixelFormat_422YpCbCr8"; break;
    case kCMPixelFormat_422YpCbCr8_yuvs: return @"kCMPixelFormat_422YpCbCr8_yuvs"; break;
    case kCMPixelFormat_444YpCbCr8: return @"kCMPixelFormat_444YpCbCr8"; break;
    case kCMPixelFormat_4444YpCbCrA8: return @"kCMPixelFormat_4444YpCbCrA8"; break;
    case kCMPixelFormat_422YpCbCr16: return @"kCMPixelFormat_422YpCbCr16"; break;
    case kCMPixelFormat_422YpCbCr10: return @"kCMPixelFormat_422YpCbCr10"; break;
    case kCMPixelFormat_444YpCbCr10: return @"kCMPixelFormat_444YpCbCr10"; break;
    case kCMPixelFormat_8IndexedGray_WhiteIsZero: return @"kCMPixelFormat_8IndexedGray_WhiteIsZero"; break;
    default: return @"UNKNOWN_PIXEL_FORMAT_TYPE";
  };
}

// returns -1 when format is unsupported
- (int) libavPixelFormatToAVFoundationPixelFormat: (enum AVPixelFormat) fmt {
  switch(fmt) {
    case AV_PIX_FMT_ARGB:           return kCMPixelFormat_32ARGB;                 break;
    case AV_PIX_FMT_BGRA:           return kCMPixelFormat_32BGRA;                 break;
    case AV_PIX_FMT_RGB24:          return kCMPixelFormat_24RGB;                  break;
    case AV_PIX_FMT_RGB555BE:       return kCMPixelFormat_16BE555;                break;
    case AV_PIX_FMT_RGB565BE:       return kCMPixelFormat_16BE565;                break;
    case AV_PIX_FMT_RGB555LE:       return kCMPixelFormat_16LE555;                break;
    case AV_PIX_FMT_RGB565LE:       return kCMPixelFormat_16LE565;                break;
    case AV_PIX_FMT_UYVY422:        return kCMPixelFormat_422YpCbCr8;             break;
    case AV_PIX_FMT_YUYV422:        return kCMPixelFormat_422YpCbCr8_yuvs;        break;
    case AV_PIX_FMT_YUV444P:        return kCMPixelFormat_444YpCbCr8;             break;
    case AV_PIX_FMT_YUV444P16LE:    return kCMPixelFormat_4444YpCbCrA8;           break; // @todo - not sure about this one
    case AV_PIX_FMT_YUV422P16LE:    return kCMPixelFormat_422YpCbCr16;            break; // @todo - not sure about this one 
    case AV_PIX_FMT_YUV422P10LE:    return kCMPixelFormat_422YpCbCr10;            break; // @todo - not sure about this one 
    case AV_PIX_FMT_YUV444P10LE:    return kCMPixelFormat_444YpCbCr10;            break; // @todo - not sure about this one 

    case kCMPixelFormat_8IndexedGray_WhiteIsZero:
    default: {
      return -1;
    }
  }
}

// negative values means that we didn't find the format
- (int) avFoundationPixelFormatToLibavPixelFormat: (int) fmt {

  switch(fmt) {
    case  kCMPixelFormat_32ARGB:           return AV_PIX_FMT_ARGB;         break;
    case  kCMPixelFormat_32BGRA:           return AV_PIX_FMT_BGRA;         break;
    case  kCMPixelFormat_24RGB:            return AV_PIX_FMT_RGB24;        break;
    case  kCMPixelFormat_16BE555:          return AV_PIX_FMT_RGB555BE;     break;
    case  kCMPixelFormat_16BE565:          return AV_PIX_FMT_RGB565BE;     break;
    case  kCMPixelFormat_16LE555:          return AV_PIX_FMT_RGB555LE;     break;
    case  kCMPixelFormat_16LE565:          return AV_PIX_FMT_RGB565LE;     break;
    case  kCMPixelFormat_422YpCbCr8:       return AV_PIX_FMT_UYVY422;      break;
    case  kCMPixelFormat_422YpCbCr8_yuvs:  return AV_PIX_FMT_YUYV422;      break;
    case  kCMPixelFormat_444YpCbCr8:       return AV_PIX_FMT_YUV444P;      break;
    case  kCMPixelFormat_4444YpCbCrA8:     return AV_PIX_FMT_YUV444P16LE;  break;
    case  kCMPixelFormat_422YpCbCr16:      return AV_PIX_FMT_YUV422P16LE;  break;
    case  kCMPixelFormat_422YpCbCr10:      return AV_PIX_FMT_YUV422P10LE;  break;
    case  kCMPixelFormat_444YpCbCr10:      return AV_PIX_FMT_YUV444P10LE;  break;

    case kCMVideoCodecType_JPEG_OpenDML:   return AV_PIX_FMT_RGB24;        break;

    case kCMPixelFormat_8IndexedGray_WhiteIsZero:
    default: {
      return -1;
    }
  }
}

- (NSString*) getPixelFormatString: (CMPixelFormatType) fmt {
  NSDictionary *formats = [NSDictionary dictionaryWithObjectsAndKeys:
                                        @"kCVPixelFormatType_1Monochrome", [NSNumber numberWithInt:kCVPixelFormatType_1Monochrome],
                                        @"kCVPixelFormatType_2Indexed", [NSNumber numberWithInt:kCVPixelFormatType_2Indexed],
                                        @"kCVPixelFormatType_4Indexed", [NSNumber numberWithInt:kCVPixelFormatType_4Indexed],
                                        @"kCVPixelFormatType_8Indexed", [NSNumber numberWithInt:kCVPixelFormatType_8Indexed],
                                        @"kCVPixelFormatType_1IndexedGray_WhiteIsZero", [NSNumber numberWithInt:kCVPixelFormatType_1IndexedGray_WhiteIsZero],
                                        @"kCVPixelFormatType_2IndexedGray_WhiteIsZero", [NSNumber numberWithInt:kCVPixelFormatType_2IndexedGray_WhiteIsZero],
                                        @"kCVPixelFormatType_4IndexedGray_WhiteIsZero", [NSNumber numberWithInt:kCVPixelFormatType_4IndexedGray_WhiteIsZero],
                                        @"kCVPixelFormatType_8IndexedGray_WhiteIsZero", [NSNumber numberWithInt:kCVPixelFormatType_8IndexedGray_WhiteIsZero],
                                        @"kCVPixelFormatType_16BE555", [NSNumber numberWithInt:kCVPixelFormatType_16BE555],
                                        @"kCVPixelFormatType_16LE555", [NSNumber numberWithInt:kCVPixelFormatType_16LE555],
                                        @"kCVPixelFormatType_16LE5551", [NSNumber numberWithInt:kCVPixelFormatType_16LE5551],
                                        @"kCVPixelFormatType_16BE565", [NSNumber numberWithInt:kCVPixelFormatType_16BE565],
                                        @"kCVPixelFormatType_16LE565", [NSNumber numberWithInt:kCVPixelFormatType_16LE565],
                                        @"kCVPixelFormatType_24RGB", [NSNumber numberWithInt:kCVPixelFormatType_24RGB],
                                        @"kCVPixelFormatType_24BGR", [NSNumber numberWithInt:kCVPixelFormatType_24BGR],
                                        @"kCVPixelFormatType_32ARGB", [NSNumber numberWithInt:kCVPixelFormatType_32ARGB],
                                        @"kCVPixelFormatType_32BGRA", [NSNumber numberWithInt:kCVPixelFormatType_32BGRA],
                                        @"kCVPixelFormatType_32ABGR", [NSNumber numberWithInt:kCVPixelFormatType_32ABGR],
                                        @"kCVPixelFormatType_32RGBA", [NSNumber numberWithInt:kCVPixelFormatType_32RGBA],
                                        @"kCVPixelFormatType_64ARGB", [NSNumber numberWithInt:kCVPixelFormatType_64ARGB],
                                        @"kCVPixelFormatType_48RGB", [NSNumber numberWithInt:kCVPixelFormatType_48RGB],
                                        @"kCVPixelFormatType_32AlphaGray", [NSNumber numberWithInt:kCVPixelFormatType_32AlphaGray],
                                        @"kCVPixelFormatType_16Gray", [NSNumber numberWithInt:kCVPixelFormatType_16Gray],
                                        @"kCVPixelFormatType_422YpCbCr8", [NSNumber numberWithInt:kCVPixelFormatType_422YpCbCr8],
                                        @"kCVPixelFormatType_4444YpCbCrA8", [NSNumber numberWithInt:kCVPixelFormatType_4444YpCbCrA8],
                                        @"kCVPixelFormatType_4444YpCbCrA8R", [NSNumber numberWithInt:kCVPixelFormatType_4444YpCbCrA8R],
                                        @"kCVPixelFormatType_444YpCbCr8", [NSNumber numberWithInt:kCVPixelFormatType_444YpCbCr8],
                                        @"kCVPixelFormatType_422YpCbCr16", [NSNumber numberWithInt:kCVPixelFormatType_422YpCbCr16],
                                        @"kCVPixelFormatType_422YpCbCr10", [NSNumber numberWithInt:kCVPixelFormatType_422YpCbCr10],
                                        @"kCVPixelFormatType_444YpCbCr10", [NSNumber numberWithInt:kCVPixelFormatType_444YpCbCr10],
                                        @"kCVPixelFormatType_420YpCbCr8Planar", [NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8Planar],
                                        @"kCVPixelFormatType_420YpCbCr8PlanarFullRange", [NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8PlanarFullRange],
                                        @"kCVPixelFormatType_422YpCbCr_4A_8BiPlanar", [NSNumber numberWithInt:kCVPixelFormatType_422YpCbCr_4A_8BiPlanar],
                                        @"kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange", [NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange],
                                        @"kCVPixelFormatType_420YpCbCr8BiPlanarFullRange", [NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarFullRange],
                                        @"kCVPixelFormatType_422YpCbCr8_yuvs", [NSNumber numberWithInt:kCVPixelFormatType_422YpCbCr8_yuvs],
                                        @"kCVPixelFormatType_422YpCbCr8FullRange", [NSNumber numberWithInt:kCVPixelFormatType_422YpCbCr8FullRange],

                                        @"kCMVideoCodecType_422YpCbCr8", [NSNumber numberWithInt:kCMVideoCodecType_422YpCbCr8],
                                        @"kCMVideoCodecType_Animation", [NSNumber numberWithInt:kCMVideoCodecType_Animation],
                                        @"kCMVideoCodecType_Cinepak", [NSNumber numberWithInt:kCMVideoCodecType_Cinepak],
                                        @"kCMVideoCodecType_JPEG", [NSNumber numberWithInt: kCMVideoCodecType_JPEG],
                                        @"kCMVideoCodecType_JPEG_OpenDML", [NSNumber numberWithInt:kCMVideoCodecType_JPEG_OpenDML],
                                        @"kCMVideoCodecType_SorensonVideo", [NSNumber numberWithInt:kCMVideoCodecType_SorensonVideo],
                                        @"kCMVideoCodecType_SorensonVideo3", [NSNumber numberWithInt:kCMVideoCodecType_SorensonVideo3],
                                        @"kCMVideoCodecType_H263", [NSNumber numberWithInt:kCMVideoCodecType_H263],
                                        @"kCMVideoCodecType_H264", [NSNumber numberWithInt:kCMVideoCodecType_H264],
                                        @"kCMVideoCodecType_MPEG4Video", [NSNumber numberWithInt:kCMVideoCodecType_MPEG4Video],
                                        @"kCMVideoCodecType_MPEG2Video", [NSNumber numberWithInt:kCMVideoCodecType_MPEG2Video],
                                        @"kCMVideoCodecType_MPEG1Video", [NSNumber numberWithInt:kCMVideoCodecType_MPEG1Video],
                                        @"kCMVideoCodecType_DVCNTSC", [NSNumber numberWithInt:kCMVideoCodecType_DVCNTSC],
                                        @"kCMVideoCodecType_DVCPAL", [NSNumber numberWithInt:kCMVideoCodecType_DVCPAL],
                                        @"kCMVideoCodecType_DVCProPAL", [NSNumber numberWithInt:kCMVideoCodecType_DVCProPAL],
                                        @"kCMVideoCodecType_DVCPro50NTSC", [NSNumber numberWithInt:kCMVideoCodecType_DVCPro50NTSC],
                                        @"kCMVideoCodecType_DVCPro50PAL", [NSNumber numberWithInt:kCMVideoCodecType_DVCPro50PAL],
                                        @"kCMVideoCodecType_DVCPROHD720p60", [NSNumber numberWithInt:kCMVideoCodecType_DVCPROHD720p60],
                                        @"kCMVideoCodecType_DVCPROHD720p50", [NSNumber numberWithInt:kCMVideoCodecType_DVCPROHD720p50],
                                        @"kCMVideoCodecType_DVCPROHD1080i60", [NSNumber numberWithInt:kCMVideoCodecType_DVCPROHD1080i60],
                                        @"kCMVideoCodecType_DVCPROHD1080i50", [NSNumber numberWithInt:kCMVideoCodecType_DVCPROHD1080i50],
                                        @"kCMVideoCodecType_DVCPROHD1080p30", [NSNumber numberWithInt:kCMVideoCodecType_DVCPROHD1080p30],
                                        @"kCMVideoCodecType_DVCPROHD1080p25", [NSNumber numberWithInt:kCMVideoCodecType_DVCPROHD1080p25],
                                        @"kCMVideoCodecType_AppleProRes4444", [NSNumber numberWithInt:kCMVideoCodecType_AppleProRes4444],
                                        @"kCMVideoCodecType_AppleProRes422HQ", [NSNumber numberWithInt:kCMVideoCodecType_AppleProRes422HQ],
                                        @"kCMVideoCodecType_AppleProRes422", [NSNumber numberWithInt:kCMVideoCodecType_AppleProRes422],
                                        @"kCMVideoCodecType_AppleProRes422LT", [NSNumber numberWithInt:kCMVideoCodecType_AppleProRes422LT],
                                        @"kCMVideoCodecType_AppleProRes422Proxy", [NSNumber numberWithInt:kCMVideoCodecType_AppleProRes422Proxy],
                                        nil];


  NSNumber* nfmt = [NSNumber numberWithInt:fmt];
  return [formats objectForKey:nfmt];
}

// Mac uses an AVCaptureVideoDataOutput object which also has it's pixel format 
// capabilities. The same as the AVCaptureDeviceInput which really represents camera.
- (void) printSupportedPixelFormatsByVideoDataOutput: (AVCaptureVideoDataOutput*) o {
  RX_VERBOSE("");
  RX_VERBOSE("Supported pixel formats by the AVCaptureVideoDataOutput* (not same as AVCaptureDeviceInput!)");
  RX_VERBOSE("--------------------------------------------------------------------");
  NSArray* supported_pix_fmts = o.availableVideoCVPixelFormatTypes;
  int i = 0;
  for(NSNumber *curr_pix_fmt in supported_pix_fmts){
    int libav_fmt = [self avFoundationPixelFormatToLibavPixelFormat: [curr_pix_fmt intValue]];
    std::string libav_fmt_str = rx_libav_pixel_format_to_string((AVPixelFormat)libav_fmt);
    if(!i) {
      RX_VERBOSE("\t%s (%s) [most efficient]", libav_fmt_str.c_str(), [[self pixelFormatToString: [curr_pix_fmt intValue]] UTF8String]);
    }
    else {
      RX_VERBOSE("\t%s (%s)", libav_fmt_str.c_str(), [[self pixelFormatToString: [curr_pix_fmt intValue]] UTF8String]);
    }
    ++i;
  }
}

- (void) printSupportedVideoCodecFormatsByVideoDataOutput: (AVCaptureVideoDataOutput*) o {
  RX_VERBOSE("");
  RX_VERBOSE("Supported video codec formats by the AVCaptureVideoDataOutput* (not same as AVCaptureDeviceInput!)");
  RX_VERBOSE("--------------------------------------------------------------------");
  NSArray* supported_codecs = o.availableVideoCodecTypes;
  for(NSString *curr_codec in supported_codecs) {
    RX_VERBOSE("\t%s", [curr_codec UTF8String]);
  }
}


- (int) isLibavPixelFormatSupportedByCaptureVideoDataOutput: (AVCaptureVideoDataOutput*) o
                                                pixelFormat: (enum AVPixelFormat) fmt
{

  NSArray* supported_pix_fmts = o.availableVideoCVPixelFormatTypes;
  for(NSNumber *curr_pix_fmt in supported_pix_fmts){
    int libav_fmt = [self avFoundationPixelFormatToLibavPixelFormat: [curr_pix_fmt intValue]];
    if(libav_fmt == (int)fmt) {
      return 1;
    }
  }
  return 0;
}

// C INTERFACE
// ----------------------------------------------------------------------------------------------
void* avf_alloc() {
  return [[VideoCaptureAVFoundation alloc] init];
}

void avf_dealloc(void* cap) {
  if(cap) {
    [(id)cap dealloc];
  }
}

int avf_list_devices(void* cap) {
  return [(id)cap listDevices];
}

int avf_open_device(void* cap, int device, VideoCaptureSettings cfg) {
  return [(id)cap openDevice: device 
                   withWidth: cfg.width 
                   andHeight: cfg.height 
                   andFormat: cfg.in_pixel_format 
                andFrameRate: cfg.fps];
}

int avf_close_device(void* cap) { 
  return [(id)cap closeDevice];
}

int avf_start_capture(void* cap) {
  return [(id)cap captureStart];
}

int avf_stop_capture(void* cap) {
  return [(id)cap captureStop];
}

void avf_update(void* cap) {
}

void avf_set_frame_callback(void* cap, videocapture_frame_cb frameCB, void* user) {
  [(id)cap setFrameCallback: frameCB user:user];
}

/*
std::vector<AVRational> avf_get_framerates(void *cap, int device, int width, int height, enum AVPixelFormat fmt) {
  std::vector<AVRational> result;
  [(id)cap getFrameRates:result forWidth:width andHeight:height andFormat:fmt andDevice:device];
  return result;
}

std::vector<enum AVPixelFormat> avf_get_pixel_formats(void* cap, int device, int width, int height) {
  std::vector<enum AVPixelFormat> result;
  [(id)cap getPixelFormats:result forWidth:width andHeight:height andDevice:device];
  return result;
}

std::vector<AVSize> avf_get_sizes(void* cap, int device) {
  std::vector<AVSize> result;
  [(id)cap getSizes:result forDevice:device];
  return result;
}
*/

std::vector<AVCapability> avf_get_capabilities(void* cap, int device) {
  std::vector<AVCapability> result;
  [(id)cap getCapabilities:result forDevice:device];
  return result;
}

@end
