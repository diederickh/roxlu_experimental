# VideoCapture

# TODO

- Most system SDKs have their own pixel format conversion routines. 
  In short these SDKs work with a filter graph that roughly works like this: 
  ````
  [capture device] --> [ grabber ] --> [ null renderer]
                           ^
                           |_ this is where you attach an callback function to.
  ````                           

  The [ grabber ] can sometimes convert the incoming pixels to another format for you.
  So you can define the pixel format on the [capture device] and on the [grabber] (or some other 
  intermedia object). For now, the VideoCaptureSettings.in_pixel_format sets the [capture device]
  pixel format and the VideoCaptureSettings.out_pixel_format stes the SWSContext (libswcale)
  output format. But, we need to add a feature that first tests if the [grabber] can convert
  the pixel format, because these are often implemented by the OS and one can assume these
  are faster/HW-accelerated (<-- need to check that first.).
- The `VideoCaptureMediaFoundation` has a memory leak. 
  When you repeatedly call `openDevice()` and `closeDevice()` it leaks about 80kb.
- The `VideoCaptureDirectShow2` has a memory leak. 
  When you repeatedly call `openDevice()` and `closeDevice()` it leaks about 10kb. 
  Probably something with COM objects not being released
- Add an API for device detection/loss
