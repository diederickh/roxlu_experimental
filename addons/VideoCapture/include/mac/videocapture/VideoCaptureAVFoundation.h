#undef DEBUG /* resulted in wierd errors with roxlu lib */
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CMBufferQueue.h>
#include <videocapture/Types.h>

typedef void(*rx_capture_frame_cb)(void* pixels, size_t nbytes, void* user);

@interface VideoCaptureAVFoundation : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate> {
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
- (int) openDevice:(int)device withWidth:(int) w andHeight:(int) h andFormat:(VideoCaptureFormat) fmt andApply:(int) set;
- (void) captureOutput:(AVCaptureOutput*)captureOutput
         didOutputSampleBuffer:(CMSampleBufferRef) sampleBuffer
         fromConnection:(AVCaptureConnection*) connection;
- (int) captureStart;
- (void) setFrameCallback:(rx_capture_frame_cb) frameCB user:(void*) frameUser;
- (int) isFormatSupported:(VideoCaptureFormat) fmt forWidth:(int)w andHeight:(int)h andApply:(int)applyFormat;
- (FourCharCode) videoCaptureFormatTypeToCMPixelFormatType:(VideoCaptureFormat) fmt;
- (NSString* const) widthHeightToCaptureSessionPreset:(int) w andHeight:(int) h;
- (int) getWidth;
- (int) getHeight;
@end
