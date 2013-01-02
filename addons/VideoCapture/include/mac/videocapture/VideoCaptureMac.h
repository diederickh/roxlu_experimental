#undef DEBUG /* resulted in wierd errors with roxlu lib */
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CMBufferQueue.h>
//#include <CoreMediaIO/CMIOHardware.h>
//#include <CoreMediaIO/CMIOHardwareObject.h>

typedef void(*rx_capture_frame_cb)(void* pixels, size_t nbytes, void* user);

@interface rx_capture_t : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate> {
  AVCaptureSession* session;
  AVCaptureDeviceInput* input; 
  AVCaptureVideoDataOutput* output;
  rx_capture_frame_cb frame_cb;
  void* frame_user;
  int width;
  int height;
}

- (int) listDevices;
- (int) printVerboseInfo;
- (int) openDevice:(int)device;
- (void) captureOutput:(AVCaptureOutput*)captureOutput
         didOutputSampleBuffer:(CMSampleBufferRef) sampleBuffer
         fromConnection:(AVCaptureConnection*) connection;
- (int) start;
- (void) setFrameCallback:(rx_capture_frame_cb) frameCB user:(void*) frameUser;
- (int) getWidth;
- (int) getHeight;
@end
