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

add_library(roxlu_ogg STATIC ${ogg_source_files})
