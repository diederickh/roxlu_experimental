# FLV
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/build/cmake/CMakeLists.txt) # roxlu

set(flv_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(flv_src_dir ${flv_base_dir}/src/)
set(flv_include_dir ${flv_base_dir}/include/)

include_directories(
  ${flv_include_dir}
  ${flv_include_platform_dir}
)

set(flv_source_files
  ${flv_src_dir}/flv/AV.cpp
  ${flv_src_dir}/flv/FLV.cpp
  ${flv_src_dir}/flv/FLVScreenRecorder.cpp
)
 
set(roxlu_addon_sources
   ${roxlu_addon_sources}
   ${flv_source_files}
)

if(ROXLU_PLATFORM_WIN)
  set(roxlu_addon_libs
    ${roxlu_addon_libs}
    ${roxlu_extern_lib_dir}/libuv.lib
    ${roxlu_extern_lib_dir}/avutil.lib
    ${roxlu_extern_lib_dir}/swscale.lib
    ${roxlu_extern_lib_dir}/x264.lib
    ${roxlu_extern_lib_dir}/libmp3lame.lib
    ${roxlu_extern_lib_dir}/libmpghip-static.lib

    # for libuv/sockets
    ws2_32.lib
    psapi.lib
    iphlpapi.lib
    )

  set(roxlu_install_files
    ${roxlu_install_files}
    ${roxlu_extern_bin_dir}/avcodec-54.dll
    ${roxlu_extern_bin_dir}/swscale-2.dll
    ${roxlu_extern_bin_dir}/swscale.dll
    ${roxlu_extern_bin_dir}/avutil.dll
    ${roxlu_extern_bin_dir}/avutil-52.dll
    ${roxlu_extern_bin_dir}/x264.dll
    ${roxlu_extern_bin_dir}/libmp3lame.dll
    )

elseif(ROXLU_PLATFORM_LINUX)
  set(roxlu_addon_libs
    ${roxlu_addon_libs}
    ${roxlu_extern_lib_dir}/libx264.a
    ${roxlu_extern_lib_dir}/libmp3lame.a
    ${roxlu_extern_lib_dir}/libavutil.a
    ${roxlu_extern_lib_dir}/libswscale.a
    )
elseif(ROXLU_PLATFORM_MAC)
  set(roxlu_addon_libs
    ${roxlu_addon_libs}

    ${roxlu_extern_lib_dir}/libmp3lame.a
    ${roxlu_extern_lib_dir}/libavutil.a
    ${roxlu_extern_lib_dir}/libswscale.a
    )

  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(roxlu_addon_libs ${roxlu_addon_libs} ${roxlu_extern_lib_debug_dir}/libx264.a)
  else()
    set(roxlu_addon_libs ${roxlu_addon_libs} ${roxlu_extern_lib_dir}/libx264.a)
  endif()

endif()


add_library(roxlu_flv STATIC ${flv_source_files})
