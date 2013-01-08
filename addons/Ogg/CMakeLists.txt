# Ogg
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt) # roxlu

set(ogg_base_dir ${CMAKE_CURRENT_LIST_DIR})
set(ogg_src_dir ${ogg_base_dir}/src/)
set(ogg_include_dir ${ogg_base_dir}/include/)

include_directories(
  ${ogg_include_dir}
  ${ogg_include_platform_dir}
)

set(ogg_source_files
  ${ogg_src_dir}/ogg/CaptureBuffer.cpp
  ${ogg_src_dir}/ogg/OggMaker.cpp
  ${ogg_src_dir}/ogg/VorbisWriter.cpp
)
 
set(roxlu_addon_sources
   ${roxlu_addon_sources}
   ${ogg_source_files}
)

if(ROXLU_PLATFORM_WIN)
  set(roxlu_addon_libs
    ${roxlu_addon_libs}
    ${roxlu_extern_lib_dir}/libogg_static_${roxlu_win_runtime}.lib
    ${roxlu_extern_lib_dir}/libtheora_static_${roxlu_win_runtime}.lib
    ${roxlu_extern_lib_dir}/libvorbis_static_${roxlu_win_runtime}.lib
    ${roxlu_extern_lib_dir}/vpxmt_${roxlu_win_runtime}.lib
    ${roxlu_extern_lib_dir}/libavutil_${roxlu_win_runtime}.a
    ${roxlu_extern_lib_dir}/libswscale_${roxlu_win_runtime}.a

    )
endif(ROXLU_PLATFORM_WIN)

add_library(roxlu_ogg STATIC ${ogg_source_files})
