# videocapture

roxlu_addon_begin("videocapture")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_include_dir(${roxlu_platform})
  roxlu_addon_add_source_file(videocapture/VideoCapture.cpp)
  roxlu_addon_add_source_file(videocapture/VideoCaptureGLSurface.cpp)

  if(APPLE) 
     roxlu_addon_add_source_file(${roxlu_platform}/videocapture/rx_capture_mac.mm)
     roxlu_addon_add_source_file(${roxlu_platform}/videocapture/VideoCaptureAVFoundation.mm)

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
     roxlu_add_lib(rx_capture)
  endif(APPLE)

  if(UNIX AND NOT APPLE)
    roxlu_addon_add_source_file(${roxlu_platform}/videocapture/rx_capture_linux.cpp)
    roxlu_addon_add_source_file(${roxlu_platform}/videocapture/VideoCaptureV4L2.cpp)
    roxlu_add_lib(udev)
  endif(UNIX AND NOT APPLE)


  if(WIN32) 
    roxlu_addon_add_source_file(${roxlu_platform}/videocapture/rx_capture_win.cpp)
    roxlu_addon_add_source_file(${roxlu_platform}/videocapture/VideoCaptureDirectShow.cpp)
    roxlu_add_lib(strmiids.lib)
    roxlu_add_lib(rx_capture)
  endif()

  if(APPLE)
     set_target_properties(roxlu_videocapture PROPERTIES COMPILE_FLAGS "-ObjC++")
     set_target_properties(rx_capture PROPERTIES COMPILE_FLAGS "-ObjC++")
  endif()
  # --------------------------------------------------------------------------------------

roxlu_addon_end()


