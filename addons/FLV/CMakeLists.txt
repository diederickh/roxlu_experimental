# FLV
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt) # roxlu

set(flv_base_dir ${CMAKE_CURRENT_LIST_DIR})
set(flv_src_dir ${flv_base_dir}/src/)
set(flv_include_dir ${flv_base_dir}/include/)

include_directories(
  ${flv_include_dir}
  ${flv_include_platform_dir}
)

set(flv_source_files
  ${flv_src_dir}/flv/AV.cpp
  ${flv_src_dir}/flv/FLV.cpp
)
 
set(roxlu_addon_sources
   ${roxlu_addon_sources}
   ${flv_source_files}
)

if(ROXLU_PLATFORM_WIN)
  #if(ROXLU_WIN_LIB_STATIC) 
  #elseif(ROXLU_WIN_LIB_DYNAMIC)
  #endif()
#elseif(ROXLU_PLATFORM_LINUX)
elseif(ROXLU_PLATFORM_MAC)
  set(roxlu_addon_libs
    ${roxlu_addon_libs}
    ${roxlu_extern_lib_dir}/libx264.a
    ${roxlu_extern_lib_dir}/libmp3lame.a
    ${roxlu_extern_lib_dir}/libavutil.a
    ${roxlu_extern_lib_dir}/libswscale.a
    )
endif()

add_library(roxlu_flv STATIC ${flv_source_files})
