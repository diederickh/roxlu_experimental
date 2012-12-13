# Videorecorder cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt)

if(ROXLU_PLATFORM_MAC)
set(roxlu_linker_flags 
   ${roxlu_linker_flags} 
)	    
endif(ROXLU_PLATFORM_MAC)

set(videorecorder_base_dir ${CMAKE_CURRENT_LIST_DIR})
set(videorecorder_src_dir ${videorecorder_base_dir}/src)
set(videorecorder_include_dir ${videorecorder_base_dir}/include)
set(videorecorder_lib_dir ${videorecorder_base_dir}/lib/${roxlu_platform}/${roxlu_bits})

include_directories(
  ${videorecorder_include_dir}
  ${videorecorder_base_dir}/extern/include/
  ${videorecorder_src_dir}
)

set(videorecorder_source_files
  ${videorecorder_src_dir}/videorecorder/AMF.cpp
  ${videorecorder_src_dir}/videorecorder/AMFTypes.cpp
  ${videorecorder_src_dir}/videorecorder/FLV.cpp
  ${videorecorder_src_dir}/videorecorder/VideoIO.cpp
  ${videorecorder_src_dir}/videorecorder/VideoIOFLV.cpp
  ${videorecorder_src_dir}/videorecorder/VideoIOH264.cpp
  ${videorecorder_src_dir}/videorecorder/VideoIOFLVSocket.cpp
  ${videorecorder_src_dir}/videorecorder/OpenGLRecorder.cpp
  ${videorecorder_src_dir}/videorecorder/VideoRecorder.cpp
)

set(roxlu_addon_sources 
  ${roxlu_addon_sources} 
  ${videorecorder_source_files}
)

set(videorecorder_libs
  ${roxlu_extern_lib_dir}/libspeex.a
  ${roxlu_extern_lib_dir}/libspeexdsp.a
  ${roxlu_extern_lib_dir}/libswscale.a
  ${roxlu_extern_lib_dir}/libx264.a
  ${roxlu_extern_lib_dir}/libyuv.a
  ${roxlu_extern_lib_dir}/libavutil.a
)


set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${videorecorder_libs}
)

set(CMAKE_CXX_FLAGS 
  ${CMAKE_CXX_FLAGS} 
  -D__STDC_CONSTANT_MACROS 
)

add_library(roxlu_videorecorder STATIC ${videorecorder_source_files})
