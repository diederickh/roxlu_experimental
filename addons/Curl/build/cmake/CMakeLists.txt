# Curl CMakeLists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/build/cmake/CMakeLists.txt) # roxlu 

set(curl_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(curl_src_dir ${curl_base_dir}/src/kurl/)
set(curl_include_dir ${curl_base_dir}/include)

roxlu_add_include_dir(${curl_include_dir})
roxlu_add_include_dir(${curl_base_dir}/extern/include/)
roxlu_add_include_dir(${curl_src_dir})

set(curl_source_files
  ${curl_src_dir}/Kurl.cpp
)

source_group("src\\curl" FILES ${curl_source_files})

if(WIN32)
  roxlu_add_extern_lib(libcurl.lib)
  roxlu_add_extern_lib(ssleay32.lib)
  roxlu_add_extern_lib(libeay32.lib)
  roxlu_add_extern_lib(zlib.lib)
  roxlu_add_lib(ws2_32.lib)
  roxlu_add_lib(iphlpapi.lib)
  roxlu_add_lib(psapi.lib)

  roxlu_add_dll(libcurl.dll)
  roxlu_add_dll(libeay32.dll)
  roxlu_add_dll(ssleay32.dll)
  roxlu_add_dll(zlib.dll)

else()
  roxlu_add_extern_lib(libcurl.a)
  roxlu_add_extern_lib(libssl.a)
  roxlu_add_extern_lib(libcrypto.a)
  roxlu_add_extern_lib(libz.a)
endif()

if(UNIX AND NOT APPLE)
  roxlu_add_lib(dl)
  roxlu_add_lib(rt)
endif()

add_library(roxlu_curl STATIC ${curl_source_files})
roxlu_add_lib(roxlu_curl)
