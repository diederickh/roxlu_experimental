#import <videocapture/VideoCaptureMac.h>

@implementation rx_capture_t

- (id) init {
  self = [super init];
  frame_cb = nil;
  frame_user = nil;
  width = 0;
  height = 0;

  if (self) {
    session = [[AVCaptureSession alloc]init];

//    if([session canSetSessionPreset:AVCaptureSessionPresetHigh]) {
    if([session canSetSessionPreset:AVCaptureSessionPreset640x480]) {
      session.sessionPreset = AVCaptureSessionPreset640x480; //AVCaptureSessionPresetHigh;
      printf("Set high preset.\n");
    }
    else {
      printf("ERROR: Cannot set high caputre preset.\n");
      return nil;
    }
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

- (int) openDevice:(int)dev {
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

- (int) start {
  [session startRunning];
  return 1;
}

- (void) captureOutput:(AVCaptureOutput*)captureOutput
         didOutputSampleBuffer:(CMSampleBufferRef) sampleBuffer
         fromConnection:(AVCaptureConnection*) connection 
{
        

  CVImageBufferRef img_ref = CMSampleBufferGetImageBuffer(sampleBuffer);
  CVPixelBufferLockBaseAddress(img_ref, 0);
  size_t w = CVPixelBufferGetWidth(img_ref);
  size_t h = CVPixelBufferGetHeight(img_ref);                
  size_t bytes_per_row = CVPixelBufferGetBytesPerRow(img_ref);
  size_t buffer_size = CVPixelBufferGetDataSize(img_ref);
  void* base_address = CVPixelBufferGetBaseAddress(img_ref);
  //printf("w: %zu, h: %zu, bytes_per_row: %zu, total; %zu\n", w, h, bytes_per_row, buffer_size);        
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
@end
