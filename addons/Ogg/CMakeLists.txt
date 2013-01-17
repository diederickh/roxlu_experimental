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
  if(ROXLU_WIN_LIB_STATIC) 

    set(roxlu_addon_libs
      ${roxlu_addon_libs}
      ${roxlu_extern_lib_dir}/libogg_static_${roxlu_win_runtime}.lib
      ${roxlu_extern_lib_dir}/libtheora_static_${roxlu_win_runtime}.lib
      ${roxlu_extern_lib_dir}/libvorbis_static_${roxlu_win_runtime}.lib
      ${roxlu_extern_lib_dir}/vpx_static_${roxlu_win_runtime}.lib
      ${roxlu_extern_lib_dir}/libavutil_static_${roxlu_win_runtime}.a
      ${roxlu_extern_lib_dir}/libswscale_static_${roxlu_win_runtime}.a
    )
  elseif(ROXLU_WIN_LIB_DYNAMIC)

    set(roxlu_addon_libs
      ${roxlu_addon_libs}
      ${roxlu_extern_lib_dir}/libogg_import_${roxlu_win_runtime}.lib
      ${roxlu_extern_lib_dir}/libtheora_import_${roxlu_win_runtime}.lib
      ${roxlu_extern_lib_dir}/libvorbis_import_${roxlu_win_runtime}.lib
      ${roxlu_extern_lib_dir}/vpx_static_${roxlu_win_runtime}.lib
      ${roxlu_extern_lib_dir}/libavutil_import_MD.lib
      ${roxlu_extern_lib_dir}/libswscale_import_MD.lib
      )

    set(roxlu_install_files
      ${roxlu_install_files}
      ${roxlu_extern_lib_dir}/libavutil.dll
      ${roxlu_extern_lib_dir}/libswscale.dll
      ${roxlu_extern_lib_dir}/libogg.dll
      ${roxlu_extern_lib_dir}/libtheora.dll
      ${roxlu_extern_lib_dir}/libvorbis.dll
      ${roxlu_extern_lib_dir}/libvorbisfile.dll
      ${roxlu_extern_lib_dir}/swscale-2.dll
      ${roxlu_extern_lib_dir}/avutil-52.dll
      )
  endif()
elseif(ROXLU_PLATFORM_LINUX)
  set(roxlu_addon_libs
    ${roxlu_addon_libs}
    ${roxlu_extern_lib_dir}/libogg.a
    ${roxlu_extern_lib_dir}/libvorbis.a
    ${roxlu_extern_lib_dir}/libvorbisenc.a
    ${roxlu_extern_lib_dir}/libtheora.a
    ${roxlu_extern_lib_dir}/libswscale.a
    ${roxlu_extern_lib_dir}/libavutil.a
    ${roxlu_extern_lib_dir}/libvpx.a
    )
elseif(ROXLU_PLATFORM_MAC)
  set(roxlu_addon_libs
    ${roxlu_addon_libs}
    ${roxlu_extern_lib_dir}/libogg.a
    ${roxlu_extern_lib_dir}/libvorbis.a
    ${roxlu_extern_lib_dir}/libvorbisenc.a
    ${roxlu_extern_lib_dir}/libtheora.a
    ${roxlu_extern_lib_dir}/libswscale.a
    ${roxlu_extern_lib_dir}/libavutil.a
    ${roxlu_extern_lib_dir}/libvpx.a
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-ggdb")
  endif()
  if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUGS} /Od")
  endif()
endif();

add_library(roxlu_ogg STATIC ${ogg_source_files})
