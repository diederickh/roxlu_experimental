# VideoCapture 
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/build/cmake/CMakeLists.txt) # roxlu

set(videocapture_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(videocapture_src_dir ${videocapture_base_dir}/src/)
set(videocapture_src_platform_dir ${videocapture_src_dir}/${roxlu_platform}/)
set(videocapture_include_dir ${videocapture_base_dir}/include/)
set(videocapture_include_platform_dir ${videocapture_include_dir}/${roxlu_platform})

include_directories(
  ${videocapture_include_dir}
  ${videocapture_include_platform_dir}
)

set(videocapture_source_files
  ${videocapture_src_dir}/videocapture/VideoCapture.cpp
  ${videocapture_src_dir}/videocapture/VideoCaptureGLSurface.cpp
)

# APPLE
# ------------------------
if(APPLE) 
  set(rxcapture_source_files
    ${videocapture_src_platform_dir}/videocapture/rx_capture_mac.mm
    ${videocapture_src_platform_dir}/videocapture/VideoCaptureAVFoundation.mm
  )

  find_library(fr_core_foundation CoreFoundation)
  find_library(fr_cocoa Cocoa)
  find_library(fr_avfoundation AVFoundation)
  find_library(fr_core_video CoreVideo)
  find_library(fr_core_media CoreMedia)
  find_library(fr_core_media_io CoreMediaIO)
  
  set(roxlu_addon_libs
    ${roxlu_addon_libs}
    ${fr_core_foundation}
    ${fr_cocoa}
    ${fr_avfoundation}
    ${fr_core_video}
    ${fr_core_media}
    ${fr_core_media_io}
    )

  add_library(rx_capture ${rxcapture_source_files})

endif(APPLE)

# LINUX
# ------------------------
if(UNIX AND NOT APPLE)

  set(rxcapture_source_files
    ${videocapture_src_platform_dir}/videocapture/rx_capture_linux.cpp
    ${videocapture_src_platform_dir}/videocapture/VideoCaptureV4L2.cpp
    )

  add_library(rx_capture ${rxcapture_source_files})
  target_link_libraries(rx_capture "udev")

endif(UNIX AND NOT APPLE)


if(WIN32) 
  set(rxcapture_source_files
    ${videocapture_src_platform_dir}/videocapture/rx_capture_win.cpp
    ${videocapture_src_platform_dir}/videocapture/VideoCaptureDirectShow.cpp
    )
  set(roxlu_addon_libs
    ${roxlu_addon_libs}
    strmiids.lib
    )
  add_library(rx_capture ${rxcapture_source_files})
endif()


set(roxlu_addon_libs ${roxlu_addon_libs} rx_capture)
 
set(roxlu_addon_sources  ${roxlu_addon_sources}  ${videocapture_source_files} )

add_library(roxlu_videocapture STATIC ${videocapture_source_files})

if(APPLE)
  set_target_properties(roxlu_videocapture PROPERTIES COMPILE_FLAGS "-ObjC++")
  set_target_properties(rx_capture PROPERTIES COMPILE_FLAGS "-ObjC++")
endif()

