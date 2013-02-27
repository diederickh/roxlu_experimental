# Webm addon
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/build/cmake/CMakeLists.txt) # roxlu

set(webm_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(webm_src_dir ${webm_base_dir}src/)
set(webm_include_dir ${webm_base_dir}include/)

include_directories(
  ${webm_include_dir}
  ${roxlu_extern_include_dir}/
)

if(APPLE)
  find_library(fr_foundation CoreFoundation)
  find_library(fr_services CoreServices)

  roxlu_add_extern_lib(libswscale.a)
  roxlu_add_extern_lib(libavutil.a)
  roxlu_add_extern_lib(libavcodec.a)
  roxlu_add_extern_lib(libuv.a)
  roxlu_add_extern_lib(libvpx.a)
  roxlu_add_lib(${fr_foundation})
  roxlu_add_lib(${fr_services})
	
endif()

if(LINUX AND NOT APPLE)
	
endif()

if(WIN32)
  message(FATAL_ERROR Webm addon is not yet ported to windows)
endif()

set(webm_source_files
  ${webm_src_dir}/webm/VPXEncoder.cpp
  ${webm_src_dir}/webm/VPXDecoder.cpp
  ${webm_src_dir}/webm/VPXStream.cpp
  ${webm_src_dir}/webm/EBML.cpp
  ${webm_src_dir}/webm/Webm.cpp
  ${webm_src_dir}/webm/WebmScreenRecorder.cpp
)

add_library(roxlu_webm STATIC ${webm_source_files})
roxlu_add_lib(roxlu_webm)


