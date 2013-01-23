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
     printf("ERROR: Cannot set high caputre preset.\n");
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
   dispatch_queue_t queue = dispatch_queue_create("Video Queue", DISPATCH_QUEUE_SERIAL);
  [output setSampleBufferDelegate:self queue:queue];
  if(![session canAddOutput:output]) {
     printf("ERROR: cannot add output to current session; sessions needs to be configured.\n");
     return 0;
  }
  [session addOutput:output];
  dispatch_release(queue);
  return 1;
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
  else if(fmt == VC_FMT_YUYV422) { return kCMPixelFormat_422YpCbCr8_yuvs; }
  else if(fmt == VC_FMT_UYVY422) { return kCMPixelFormat_422YpCbCr8; }
  else {
   return 0;
  }
}

@end
