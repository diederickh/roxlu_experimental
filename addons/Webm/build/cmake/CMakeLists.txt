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

  set(webm_libs
    ${roxlu_extern_lib_dir}/libswscale.a
    ${roxlu_extern_lib_dir}/libavutil.a
    ${roxlu_extern_lib_dir}/libavcodec.a
    ${roxlu_extern_lib_dir}/libuv.a
    ${roxlu_extern_lib_dir}/libvpx.a
		${fr_foundation}
		${fr_services}
    )
	
endif()

if(LINUX AND NOT APPLE)
	
endif()

if(WIN32)
endif()

set(webm_source_files
  ${webm_src_dir}/webm/VPXEncoder.cpp
  ${webm_src_dir}/webm/VPXDecoder.cpp
  ${webm_src_dir}/webm/VPXStream.cpp
  ${webm_src_dir}/webm/EBML.cpp
  ${webm_src_dir}/webm/Webm.cpp
  ${webm_src_dir}/webm/WebmScreenRecorder.cpp
)

set(roxlu_addon_sources
  ${roxlu_addon_sources}
  ${webm_source_files}
)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${webm_libs}
)

add_library(roxlu_webm STATIC ${webm_source_files})


