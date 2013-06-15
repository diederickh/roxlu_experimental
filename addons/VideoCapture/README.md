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
-  Check if avframe/picture is used correctly; when you maintain your own buffer, you only use avpicture_fill() and NOT avpicture_alloc/avpicture_free!


### EDSDK
##### What version
  
  It seems that the latest version is not really the latest one; 
  Version 2.13 crashes with a autorelease error and 2.12 seems to work fine (correct: EDSDKv2120M.dmg.zip, wrong: EDSDKv2130M.dmg.zip)

##### Fixing @executable_paths for the EDSK frameworks:

- https://gist.github.com/roxlu/fb6f343d6a69c0ee43e7

```sh
$ cd EDSDK.framework/Versions/Current
$ install_name_tool -id @executable_path/lib/EDSDK.framework/Versions/A/EDSDK EDSDK

$ cd EDSDK.framework/Versions/Current/DPPLibCom.bundle/Contents/MacOS
$ install_name_tool -change  @executable_path/../Frameworks/DPP.framework/Versions/A/DPP @executable_path/lib/DPP.framework/Versions/A/DPP DPPLibCom

$ cd DPP.framework
$ install_name_tool -id @executable_path/lib/DPP.framework/Versions/Current/DPP DPP
````

