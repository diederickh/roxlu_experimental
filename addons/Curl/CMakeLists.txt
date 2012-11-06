# Curl CMakeLists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt) # roxlu 

set(curl_base_dir ${CMAKE_CURRENT_LIST_DIR})
set(curl_src_dir ${curl_base_dir}/src/kurl/)
set(curl_include_dir ${curl_base_dir}/include)
set(curl_lib_dir ${curl_base_dir}/lib/${roxlu_platform}/${roxlu_bits})

include_directories(
  ${curl_include_dir}
  ${curl_base_dir}/extern/include/
  ${curl_src_dir}
)

set(curl_source_files
  ${curl_src_dir}/Kurl.cpp
)

set(roxlu_addon_sources 
  ${roxlu_addon_sources} 
  ${curl_source_files}
)

set(curl_libs
  ${curl_lib_dir}/libcurl.a
  z
)

if(ROXLU_PLATFORM_LINUX)
  set(twitter_libs ${twitter_libs}  dl pthread rt)
endif(ROXLU_PLATFORM_LINUX)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${curl_libs}
)

add_library(roxlu_curl STATIC ${curl_source_files})

#add_subdirectory(tests)