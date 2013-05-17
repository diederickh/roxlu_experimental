/*
  
  VideoCaptureAVFoundation
  -------------------------
  Basic webcam grabber based on the examples provided by apple and their reference.
  This is still work in progress and we need to check if all memory management is 
  handled correctly. 

  Also, something silly happens when you ask for a grabber with a certain pixel format 
  and resolution which is not supported. The VideoCaptureInput device will try to get the
  best format you ask for, but this might be in a different format. 
  
  So if you want to capture in YUV, it might actually end up capturing using mpeg (code: 1bmd, or dmb1)
  but the AVCaptureVideoOutput seems to convert this format again. This is something I need to fix, or
  at least show some verbose info when this happens. (May 2013)


  TODO: - this code creates nice localized names for video types: 
          https://gist.github.com/roxlu/f2168a49cc5693193826
 
        - figure out this problem I posted on stackoverflow:
          http://stackoverflow.com/questions/16619068/capture-from-webcam-using-default-uncompressed-pixel-format-using-avfoundation
        

 */
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
- (void) printCaptureVideoDataOutputPixelFormats;
- (int) openDevice:(int)device withWidth:(int) w andHeight:(int) h andFormat:(VideoCaptureFormat) fmt andApply:(int) set;
- (void) captureOutput:(AVCaptureOutput*)captureOutput
         didOutputSampleBuffer:(CMSampleBufferRef) sampleBuffer
         fromConnection:(AVCaptureConnection*) connection;
- (int) captureStart;
- (int) captureStop;
- (void) setFrameCallback:(rx_capture_frame_cb) frameCB user:(void*) frameUser;
- (int) isFormatSupported:(VideoCaptureFormat) fmt forWidth:(int)w andHeight:(int)h andApply:(int)applyFormat;
- (FourCharCode) videoCaptureFormatTypeToCMPixelFormatType:(VideoCaptureFormat) fmt;
- (NSString* const) widthHeightToCaptureSessionPreset:(int) w andHeight:(int) h;
- (int) getWidth;
- (int) getHeight;

@end
