# videocapture
roxlu_add_addon(UV)  # we need the uv addon for the canon wrapper
roxlu_add_addon(Image) # we use the jpeg to decode the live view stream
roxlu_add_addon(GPUImage) 

roxlu_addon_begin("videocapture")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_include_dir(${roxlu_platform})
  roxlu_addon_add_source_file(videocapture/VideoCapture.cpp)
  roxlu_addon_add_source_file(videocapture/VideoCaptureBase.cpp)
  roxlu_addon_add_source_file(videocapture/VideoCaptureGLSurface.cpp)
  roxlu_addon_add_source_file(videocapture/Utils.cpp)
  roxlu_addon_add_source_file(videocapture/Types.cpp)
  roxlu_addon_add_source_file(videocapture/Webcam.cpp)

  if(APPLE) 
     roxlu_addon_add_source_file(videocapture/mac/VideoCaptureAVFoundation.mm)
     roxlu_addon_add_source_file(videocapture/mac/VideoCaptureMac.cpp)

     find_library(fr_core_foundation CoreFoundation)
     find_library(fr_cocoa Cocoa)
     find_library(fr_avfoundation AVFoundation)
     find_library(fr_core_video CoreVideo)
     find_library(fr_core_media CoreMedia)
     find_library(fr_core_media_io CoreMediaIO)
     
     roxlu_add_lib(${fr_core_foundation})
     roxlu_add_lib(${fr_cocoa})
     roxlu_add_lib(${fr_avfoundation})
     roxlu_add_lib(${fr_core_video})
     roxlu_add_lib(${fr_core_media})
     roxlu_add_lib(${fr_core_media_io})

     # EDSDK
     if(USE_EDSDK)
       add_definitions(-D__MACOS__)

       roxlu_addon_add_source_file(videocapture/edsdk/Canon.cpp)
       roxlu_addon_add_source_file(videocapture/edsdk/CanonTypes.cpp)
       roxlu_addon_add_source_file(videocapture/edsdk/CanonTaskQueue.cpp)
       roxlu_addon_add_source_file(videocapture/edsdk/CanonTaskOpenSession.cpp)
       roxlu_addon_add_source_file(videocapture/edsdk/CanonTaskCloseSession.cpp)
       roxlu_addon_add_source_file(videocapture/edsdk/CanonTaskTakePicture.cpp)
       roxlu_addon_add_source_file(videocapture/edsdk/CanonTaskProperty.cpp)
       roxlu_addon_add_source_file(videocapture/edsdk/CanonTaskDownload.cpp)
       roxlu_addon_add_source_file(videocapture/edsdk/CanonTaskEvfStart.cpp)
       roxlu_addon_add_source_file(videocapture/edsdk/CanonTaskEvfEnd.cpp)
       roxlu_addon_add_source_file(videocapture/edsdk/CanonTaskEvfDownload.cpp)
       roxlu_addon_add_source_file(videocapture/edsdk/CanonTaskStop.cpp)
       roxlu_addon_add_source_file(videocapture/edsdk/CanonUtils.cpp)
       roxlu_add_extern_framework(EDSDK)
       roxlu_add_extern_framework(DPP)
    endif()

  endif(APPLE)

  if(UNIX AND NOT APPLE)
    roxlu_add_extern_include_dir(videocapture/linux/)
    roxlu_addon_add_source_file(videocapture/linux/v4l2/VideoCaptureV4L2.cpp)
    roxlu_addon_add_source_file(videocapture/linux/v4l2/VideoCaptureV4L2Types.cpp)
    roxlu_addon_add_source_file(videocapture/linux/v4l2/VideoCaptureV4L2Utils.cpp) 

    roxlu_add_lib(udev)
    add_definitions(-D__STDC_CONSTANT_MACROS)

    # dependencies
    roxlu_add_extern_lib(libavformat.a)
    roxlu_add_extern_lib(libavfilter.a)
    roxlu_add_extern_lib(libavcodec.a)
    roxlu_add_extern_lib(libavresample.a)
    roxlu_add_extern_lib(libswscale.a)
    roxlu_add_extern_lib(libavutil.a)
    roxlu_add_extern_lib(libx264.a)
    roxlu_add_extern_lib(libspeex.a)
    roxlu_add_extern_lib(libvorbis.a)
    roxlu_add_extern_lib(libvorbisenc.a)
    roxlu_add_extern_lib(libtheoraenc.a)
    roxlu_add_extern_lib(libtheoradec.a)
    roxlu_add_extern_lib(libtheora.a)
    roxlu_add_extern_lib(libogg.a)
    roxlu_add_extern_lib(libssl.a)
    roxlu_add_extern_lib(libcrypto.a)
    roxlu_add_extern_lib(libuv.a)
    roxlu_add_extern_lib(libvpx.a)
    roxlu_add_extern_lib(libmp3lame.a)
    roxlu_add_lib(z)
    roxlu_add_lib(bz2)

  endif(UNIX AND NOT APPLE)


  if(WIN32) 
    find_package(WindowsSDK REQUIRED)
    file(TO_CMAKE_PATH ${WINDOWSSDK_PREFERRED_DIR} windows_sdk_dir)
    file(TO_CMAKE_PATH ${WINDOWSSDK_LATEST_DIR} windows_sdk_dir)
  
    roxlu_add_include_dir(${windows_sdk_dir}/Include)
    roxlu_add_lib(${windows_sdk_dir}/Samples/multimedia/directshow/baseclasses/Release/strmbase.lib)
    #roxlu_add_lib("strmbase.lib")

   roxlu_addon_add_source_file(videocapture/win/directshow/VideoCaptureDirectShow.cpp)
   roxlu_addon_add_source_file(videocapture/win/directshow/VideoCaptureDirectShowCB.cpp)
   roxlu_addon_add_source_file(videocapture/win/mediafoundation/VideoCaptureMediaFoundation.cpp)
   roxlu_addon_add_source_file(videocapture/win/mediafoundation/VideoCaptureMediaFoundationCB.cpp)
   roxlu_add_lib(strmiids.lib)

  endif()

  # --------------------------------------------------------------------------------------

roxlu_addon_end()
