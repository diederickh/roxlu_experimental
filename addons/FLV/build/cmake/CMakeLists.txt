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

source_group("src\\flv" FILES ${flv_source_files})

if(ROXLU_PLATFORM_WIN)
    roxlu_add_extern_lib(libuv.lib)
    roxlu_add_extern_lib(avutil.lib)
    roxlu_add_extern_lib(swscale.lib)
    roxlu_add_extern_lib(x264.lib)
    roxlu_add_extern_lib(libmp3lame.lib)
    roxlu_add_extern_lib(libmpghip-static.lib)

    # for libuv/sockets
    roxlu_add_lib(ws2_32.lib)
    roxlu_add_lib(psapi.lib)
    roxlu_add_lib(iphlpapi.lib)

    roxlu_add_dll(avcodec-54.dll)
    roxlu_add_dll(swscale-2.dll)
    roxlu_add_dll(swscale.dll)
    roxlu_add_dll(avutil.dll)
    roxlu_add_dll(avutil-52.dll)
    roxlu_add_dll(x264.dll)
    roxlu_add_dll(libmp3lame.dll)

elseif(ROXLU_PLATFORM_LINUX)
  roxlu_add_extern_lib(libx264.a)
  roxlu_add_extern_lib(libmp3lame.a)
  roxlu_add_extern_lib(libswscale.a)
  roxlu_add_extern_lib(libavutil.a)

#  set(roxlu_addon_libs
#    ${roxlu_addon_libs}
#    ${roxlu_extern_lib_dir}/libx264.a
#    ${roxlu_extern_lib_dir}/libmp3lame.a
#    ${roxlu_extern_lib_dir}/libswscale.a
#    ${roxlu_extern_lib_dir}/libavutil.a
#    )

elseif(ROXLU_PLATFORM_MAC)
  roxlu_add_extern_lib(libmp3lame.a)
  roxlu_add_extern_lib(libswscale.a)
  roxlu_add_extern_lib(libavutil.a)
  roxlu_add_extern_lib(libx264.a)
endif()

add_library(roxlu_flv STATIC ${flv_source_files})
roxlu_add_lib(roxlu_flv)