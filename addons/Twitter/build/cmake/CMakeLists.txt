
# Twitter cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/build/cmake/CMakeLists.txt)

set(twitter_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(twitter_src_dir ${twitter_base_dir}/src)
set(twitter_include_dir ${twitter_base_dir}/include)

include_directories(
  ${twitter_include_dir}
  ${twitter_base_dir}/extern/include/
  ${twitter_src_dir}
  ${roxlu_extern_src_dir} # for http_parser
)

set(twitter_source_files
  ${twitter_src_dir}/twitter/OAuth.cpp 
  ${twitter_src_dir}/twitter/HTTP.cpp 
  ${twitter_src_dir}/twitter/Twitter.cpp 
  ${twitter_src_dir}/twitter/Buffer.cpp 
  ${twitter_src_dir}/twitter/SSLBuffer.cpp 
  ${twitter_src_dir}/twitter/PercentEncode.cpp
  ${twitter_base_dir}/extern/src/formats/hmac_sha1.c
  ${twitter_base_dir}/extern/src/formats/json.c
  ${twitter_base_dir}/extern/src/formats/Base64Encoder.cpp
)

set(roxlu_addon_sources 
  ${twitter_source_files}
  ${roxlu_addon_sources} 
)

if(APPLE) 
  find_library(fr_corefoundation CoreFoundation)
  find_library(fr_core_services CoreServices)

  roxlu_add_lib(${fr_corefoundation})
  roxlu_add_lib(${fr_core_services})

  roxlu_add_extern_lib(libcrypto.a)
  roxlu_add_extern_lib(libssl.a)
  roxlu_add_extern_lib(libuv.a)


elseif(WIN32)
  roxlu_add_extern_lib(ssleay32.lib)
  roxlu_add_extern_lib(libeay32.lib)
  roxlu_add_extern_lib(zlib.lib)
  roxlu_add_extern_lib(libuv.lib)

  roxlu_add_lib(ws2_32.lib)
  roxlu_add_lib(psapi.lib)
  roxlu_add_lib(iphlpapi.lib)

  roxlu_add_dll(libcurl.dll)
  roxlu_add_dll(libeay32.dll)
  roxlu_add_dll(ssleay32.dll)

endif()

if(NOT APPLE AND UNIX)
  roxlu_add_extern_lib(libssl.a)
  roxlu_add_extern_lib(libuv.a)
  roxlu_add_extern_lib(libcrypto.a)
  roxlu_add_lib(dl)
  roxlu_add_lib(pthread)
  roxlu_add_lib(rt)
   
#  set(twitter_libs
#    ${roxlu_extern_lib_dir}/libssl.a
#    ${roxlu_extern_lib_dir}/libuv.a
#    ${roxlu_extern_lib_dir}/libcrypto.a
#    dl
#    pthread 
#    rt
#    )
endif()

add_library(roxlu_twitter STATIC ${twitter_source_files})
