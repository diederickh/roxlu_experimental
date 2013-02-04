# Curl CMakeLists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/build/cmake/CMakeLists.txt) # roxlu 

set(curl_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(curl_src_dir ${curl_base_dir}/src/kurl/)
set(curl_include_dir ${curl_base_dir}/include)

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

if(WIN32)
    set(curl_libs
      ${roxlu_extern_lib_dir}/libcurl.lib
      ${roxlu_extern_lib_dir}/ssleay32.lib
      ${roxlu_extern_lib_dir}/libeay32.lib
      ${roxlu_extern_lib_dir}/zdll.lib
      )

    set(roxlu_install_files
      ${roxlu_install_files}
      ${roxlu_extern_bin_dir}/libcurl.dll
      ${roxlu_extern_bin_dir}/libeay32.dll
      ${roxlu_extern_bin_dir}/ssleay32.dll
      )

else()
  set(curl_libs
    ${roxlu_extern_lib_dir}/libcurl.a
    ${roxlu_extern_lib_dir}/libssl.a
    ${roxlu_extern_lib_dir}/libcrypto.a
    ${roxlu_extern_lib_dir}/libz.a
    )
endif()

if(UNIX AND NOT APPLE)
  set(curl_libs
    ${curl_libs}
    dl
    rt
    )
endif()

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${curl_libs}
)

add_library(roxlu_curl STATIC ${curl_source_files})

#add_subdirectory(tests)