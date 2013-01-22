# Twitter cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-ggdb")
  endif()
  if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUGS} /Od /ZI")
    endif()
endif()

if(ROXLU_PLATFORM_MAC)
  set(roxlu_linker_flags 
    ${roxlu_linker_flags} 
    " -framework CoreFoundation -framework CoreServices "
    )	    
endif(ROXLU_PLATFORM_MAC)

set(twitter_base_dir ${CMAKE_CURRENT_LIST_DIR})
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

set(twitter_libs
  ${roxlu_extern_lib_dir}/libcrypto.a
  ${roxlu_extern_lib_dir}/libssl.a
  ${roxlu_extern_lib_dir}/libuv.a
)

if(ROXLU_PLATFORM_LINUX)
  set(twitter_libs ${twitter_libs} dl pthread rt)
endif(ROXLU_PLATFORM_LINUX)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${twitter_libs}
)

add_library(roxlu_twitter STATIC ${twitter_source_files})
