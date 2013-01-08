# VideoCapture 
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt) # roxlu

set(videocapture_base_dir ${CMAKE_CURRENT_LIST_DIR})
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
)

# APPLE
# ------------------------
if(APPLE) 
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ObjC++")
  set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

  set(rxcapture_source_files
    ${videocapture_src_platform_dir}/videocapture/rx_capture_mac.mm
    ${videocapture_src_platform_dir}/videocapture/VideoCaptureMac.mm
  )

  set(roxlu_linker_flags  "${roxlu_linker_flags} -framework CoreFoundation -framework Cocoa -framework AVFoundation -framework CoreVideo -framework CoreMedia -framework CoreMediaIO" )

  add_library(rx_capture ${rxcapture_source_files})

endif(APPLE)

# LINUX
# ------------------------
if(UNIX AND NOT APPLE)

  set(rxcapture_source_files
    ${videocapture_src_platform_dir}/videocapture/rx_capture_linux.cpp
    ${videocapture_src_platform_dir}/videocapture/V4L2Capture.cpp
    )

  add_library(rx_capture ${rxcapture_source_files})
#  set(roxlu_linker_flags "${roxlu_linker_flags} -ludev")
  target_link_libraries(rx_capture "udev")

endif(UNIX AND NOT APPLE)


set(roxlu_addon_libs  ${roxlu_addon_libs} rx_capture )
 
set(roxlu_addon_sources  ${roxlu_addon_sources}  ${videocapture_source_files} )

add_library(roxlu_videocapture STATIC ${videocapture_source_files})
