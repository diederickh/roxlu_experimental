#import <videocapture/VideoCaptureAVFoundation.h>
@implementation VideoCaptureAVFoundation

- (id) init {
  self = [super init];
  frame_cb = nil;
  frame_user = nil;
  width = 0;
  height = 0;
  input = nil;
  output = nil;
        
  if (self) {
  }
  return self;
}

- (int) listDevices {   
  int c = 0;
  NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
  for(AVCaptureDevice* device in devices) {
    printf("[%d] :  %s\n", c, [[device localizedName] UTF8String]);
    ++c;
  }
  return [devices count];
}

- (int) printVerboseInfo {
  int c = 0;
  char* cptr;
  for(AVCaptureDeviceFormat* f in [[input device] formats]) {
     FourCharCode code = CMFormatDescriptionGetMediaSubType([f formatDescription]);
     cptr = (char*)&code;
     CMVideoDimensions dims = CMVideoFormatDescriptionGetDimensions([f formatDescription]);
     for(AVFrameRateRange* fps in [f videoSupportedFrameRateRanges]) {
         printf("%d x %d @ min: %f max: %f; code: %c%c%c%c\n", dims.width, dims.height, [fps minFrameRate], [fps maxFrameRate], cptr[0], cptr[1], cptr[2], cptr[3]);
     }
  }       
  return c;
}

- (int) isFormatSupported: (VideoCaptureFormat) fmt 
                          forWidth:(int) w 
                          andHeight:(int) h 
                          andApply:(int) applyFormat 
{
  if(input == nil) {
     printf("WARNING: you first need to open the capture device before you can check a format.\n");
     return 0;
  }       

  FourCharCode pixel_fmt_to_test = [self videoCaptureFormatTypeToCMPixelFormatType: fmt];
  if(pixel_fmt_to_test == 0) {
    printf("UNKNOWN FORMAT\n");
    return 0;
  }
 
  char* cptr;
  for(AVCaptureDeviceFormat* f in [[input device] formats]) {
     FourCharCode code = CMFormatDescriptionGetMediaSubType([f formatDescription]);
     if(code != pixel_fmt_to_test) {
        continue;
     }
     cptr = (char*)&code;
     CMVideoDimensions dims = CMVideoFormatDescriptionGetDimensions([f formatDescription]);
     if(dims.width == w && dims.height == h) {
        if(applyFormat) {
           AVCaptureDevice* d = [input device];
           [d lockForConfiguration:nil];
           [d setActiveFormat: f]; /* sets pixel format */
           [d unlockForConfiguration];
        }       
        return 1;
     }
  }
  printf("WARNING: we did not find a suitable format\n");
  return 0;
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

- (int) openDevice:(int)dev 
                   withWidth:(int) w  
                   andHeight:(int) h 
                   andFormat:(VideoCaptureFormat) fmt 
                   andApply:(int)applyFormat
{
  session = [[AVCaptureSession alloc]init];
  NSString* const preset = [self widthHeightToCaptureSessionPreset: w andHeight: h];
  if([session canSetSessionPreset:preset]) {
     session.sessionPreset = preset; 
  }
  else {
     printf("ERROR: Cannot set high capture preset.\n");
     return nil;
  }

  // Find the device
  int c = 0;
  NSError* error;
  NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
  for(AVCaptureDevice* device in devices) {
    if(c == dev) {
      input = [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];
      if(!input) { 
        printf("ERROR: Cannot connect with device.\n");
        return 0;
      }
    }
    ++c;
  }

  // Check if we can use this input + add it.
  if(![session canAddInput:input]) {
    printf("ERROR: Cannot add input to current session; session needs to be configured.\n");
    return 0;
  }
  [session addInput:input];

  // set the given format
  if([self isFormatSupported: fmt forWidth:w andHeight:h andApply:applyFormat] == 0) {
     printf("WARNING: we cannot set the format, continueing anyway.\n");
  }


  // get width / height
  CMVideoDimensions dims = CMVideoFormatDescriptionGetDimensions([[[input device] activeFormat] formatDescription]);
  width = dims.width;
  height = dims.height;

  // Configure output
  output = [[AVCaptureVideoDataOutput alloc] init];

#if 0
  // See  http://uri-labs.com/macosx_headers/AVCaptureOutput_h/Classes/AVCaptureVideoDataOutput/index.html
  // for an explanation of the different values you can set for the video settings member of the 
  // AVCaptureVideoDataOutput object. While testing I stumbled upon this wierd problem, as I posted on 
  // stackoverflow: http://stackoverflow.com/questions/16619068/capture-from-webcam-using-default-uncompressed-pixel-format-using-avfoundation
  NSDictionary* video_settings = [NSDictionary dictionaryWithObjectsAndKeys:
                                   [NSNumber numberWithInt:kCVPixelFormatType_422YpCbCr8_yuvs], (id)kCVPixelBufferPixelFormatTypeKey, 
                                   nil];
  //[output setVideoSettings:video_settings];             // use custom settings.
  //[output setVideoSettings:[NSDictionary dictionary]];  // use device native format
  [output setVideoSettings:nil];                          // use default uncompressed format
#endif

#if 0
  // This tiny snippet selects the frame rate for the device. This code works fine, but 
  // I need to create a better API for this. So for now I just disabled it. (May 2013)
   AVCaptureDevice* d = [input device];
   for (AVFrameRateRange *frame_rate_range in [[d activeFormat] videoSupportedFrameRateRanges]) {
     int max_rate = [frame_rate_range maxFrameRate];
     if(max_rate == 5) {
       [d lockForConfiguration:nil];
       [d setActiveVideoMinFrameDuration: [frame_rate_range minFrameDuration]];
       [d unlockForConfiguration];
       break;
     }
     //printf("> %f, %f\n", [frame_rate_range maxFrameRate], [frame_rate_range minFrameRate]);
   }
#endif

  dispatch_queue_t queue = dispatch_queue_create("Video Queue", DISPATCH_QUEUE_SERIAL);
  [output setSampleBufferDelegate:self queue:queue];
  if(![session canAddOutput:output]) {
     printf("ERROR: cannot add output to current session; sessions needs to be configured.\n");
     return 0;
  }
  [session addOutput:output];

  dispatch_release(queue);

#if 0
  // Debug, show active type
  FourCharCode active_video_type = CMFormatDescriptionGetMediaSubType([[[input device] activeFormat] formatDescription]);
  char* cptr = (char*)&active_video_type;
  printf("The format the capture device currently uses: %c%c%c%c \n", cptr[0], cptr[1], cptr[2], cptr[3]);
#endif

  return 1;
}

// thanks to: https://gist.github.com/zen4ever/2327666
- (void) printCaptureVideoDataOutputPixelFormats {
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
                                        nil];
 
  for (NSNumber *fmt in [output availableVideoCVPixelFormatTypes]) {
    printf("Supported pixel format for AVCaptureVideoDataOutput: %s\n", [[formats objectForKey:fmt] UTF8String]);
  }
}

- (int) captureStart {
  [session startRunning];
  return 1;
}

- (int) captureStop {
  [session stopRunning];
  return 1;
}


- (void) captureOutput:(AVCaptureOutput*)captureOutput
         didOutputSampleBuffer:(CMSampleBufferRef) sampleBuffer
         fromConnection:(AVCaptureConnection*) connection 
{
  CVImageBufferRef img_ref = CMSampleBufferGetImageBuffer(sampleBuffer);
  CVPixelBufferLockBaseAddress(img_ref, 0);
  size_t buffer_size = CVPixelBufferGetDataSize(img_ref);
  void* base_address = CVPixelBufferGetBaseAddress(img_ref);
  frame_cb((void*)base_address, buffer_size, frame_user);
  CVPixelBufferUnlockBaseAddress(img_ref, 0);

#if 0 
  CMFormatDescriptionRef format_desc_ref = CMSampleBufferGetFormatDescription(sampleBuffer);
  FourCharCode active_video_type = CMFormatDescriptionGetMediaSubType(format_desc_ref);
  char* cptr = (char*)&active_video_type;
  printf("____ The current buffer we receive from the webcam has (maybe converted) the subtype: %c%c%c%c ______\n", cptr[0], cptr[1], cptr[2], cptr[3]);
#endif
}

- (void) setFrameCallback:(rx_capture_frame_cb)frameCB user:(void*)frameUser {
  frame_cb = frameCB;
  frame_user = frameUser;
}

- (int) getWidth {
  return width;
}

- (int) getHeight {
  return height;
}

- (FourCharCode) videoCaptureFormatTypeToCMPixelFormatType:(VideoCaptureFormat) fmt {

  if(fmt == VC_FMT_RGB24) { return kCMPixelFormat_24RGB; }
  else if(fmt == VC_FMT_YUYV422) { return kCMPixelFormat_422YpCbCr8_yuvs; }  // "yuvs"
  else if(fmt == VC_FMT_UYVY422) { return kCMPixelFormat_422YpCbCr8; } // "yuv2", same a libav AV_PIX_FMT_UYVY422
  else {
   return 0;
  }
}

@end
