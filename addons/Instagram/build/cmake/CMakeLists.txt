# Instagram cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt)

#include(${base}/../../addons/Curl/CMakeLists.txt) # Curl addon

set(instagram_base_dir ${CMAKE_CURRENT_LIST_DIR})
set(instagram_src_dir ${instagram_base_dir}/src)
set(instagram_include_dir ${instagram_base_dir}/include)
set(instagram_lib_dir ${instagram_base_dir}/lib/${roxlu_platform}/${roxlu_bits})

include_directories(
  ${instagram_include_dir}
  ${instagram_src_dir}
)

set(instagram_source_files
  ${instagram_src_dir}/instagram/Instagram.cpp
  ${instagram_src_dir}/instagram/SSLBuffer.cpp
  ${instagram_src_dir}/instagram/HTTP.cpp
  ${instagram_src_dir}/instagram/OAuth.cpp
)

set(roxlu_addon_sources
  ${roxlu_addon_sources}
  ${instagram_source_files}
)
  
set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${roxlu_extern_lib_dir}/libcrypto.a
  ${roxlu_extern_lib_dir}/libssl.a
  ${roxlu_extern_lib_dir}/libuv.a
  z
)

add_library(roxlu_instagram STATIC ${instagram_source_files})
