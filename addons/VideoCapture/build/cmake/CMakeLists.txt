# videocapture

roxlu_addon_begin("videocapture")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_include_dir(${roxlu_platform})
  roxlu_addon_add_source_file(videocapture/VideoCapture.cpp)
  roxlu_addon_add_source_file(videocapture/VideoCaptureBase.cpp)
  roxlu_addon_add_source_file(videocapture/VideoCaptureGLSurface.cpp)
  roxlu_addon_add_source_file(videocapture/Utils.cpp)
  roxlu_addon_add_source_file(videocapture/Types.cpp)

  if(APPLE) 
   #  roxlu_addon_add_source_file(videocapture/rx_capture_mac.mm)
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
  endif(APPLE)

  if(UNIX AND NOT APPLE)
    roxlu_addon_add_source_file(${roxlu_platform}/videocapture/rx_capture_linux.cpp)
    roxlu_addon_add_source_file(${roxlu_platform}/videocapture/VideoCaptureV4L2.cpp)
    roxlu_add_lib(udev)
  endif(UNIX AND NOT APPLE)


  if(WIN32) 
    find_package(WindowsSDK REQUIRED)
  #  roxlu_add_include_dir("${WINDOWSSDK_LATEST_DIR}/Include")
  file(TO_CMAKE_PATH ${WINDOWSSDK_PREFERRED_DIR} windows_sdk_dir)
  file(TO_CMAKE_PATH ${WINDOWSSDK_LATEST_DIR} windows_sdk_dir)
    message("+++++++++++++ ${windows_sdk_dir}")
    message("+++++++++++++ ${WINDOWSSDK_LATEST_DIR}")
    roxlu_add_include_dir(${windows_sdk_dir}/Include)
    roxlu_add_lib(${windows_sdk_dir}/Samples/multimedia/directshow/baseclasses/Release/strmbase.lib)
#    roxlu_add_lib("strmbase.lib")

   roxlu_addon_add_source_file(videocapture/win/directshow/VideoCaptureDirectShow.cpp)
   roxlu_addon_add_source_file(videocapture/win/directshow/VideoCaptureDirectShowCB.cpp)
   roxlu_addon_add_source_file(videocapture/win/mediafoundation/VideoCaptureMediaFoundation.cpp)
   roxlu_addon_add_source_file(videocapture/win/mediafoundation/VideoCaptureMediaFoundationCB.cpp)
#   roxlu_addon_add_source_file(videocapture/win/VideoCaptureWin.cpp)
#   roxlu_addon_add_source_file(${roxlu_platform}/videocapture/rx_capture_win.cpp)
#   roxlu_addon_add_source_file(${roxlu_platform}/videocapture/VideoCaptureDirectShow.cpp)

    roxlu_add_lib(strmiids.lib)
  endif()

  # --------------------------------------------------------------------------------------

roxlu_addon_end()

if(APPLE)
#  set_target_properties(roxlu_videocapture PROPERTIES COMPILE_FLAGS "-ObjC++")
#  set_target_properties(rx_capture PROPERTIES COMPILE_FLAGS "-ObjC++")
endif()

