# OpenCV cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt)

set(opencv_base_dir ${CMAKE_CURRENT_LIST_DIR})
set(opencv_src_dir ${opencv_base_dir}/src)
set(opencv_include_dir ${opencv_base_dir}/include)
set(opencv_lib_dir ${opencv_base_dir}/lib/${roxlu_platform}/${roxlu_bits})

include_directories(
  ${opencv_include_dir}
  ${opencv_base_dir}/extern/include/
  ${opencv_src_dir}
)

set(opencv_source_files
  ${opencv_src_dir}/cv/VideoGrabber.cpp
)

set(roxlu_addon_sources 
  ${roxlu_addon_sources} 
  ${opencv_source_files}
)

set(opencv_libs
  ${opencv_lib_dir}/libopencv_core.a
  ${opencv_lib_dir}/libopencv_highgui.a
  ${opencv_lib_dir}/libopencv_imgproc.a
  ${opencv_lib_dir}/libopencv_objdetect.a
  ${opencv_lib_dir}/libopencv_video.a
)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${opencv_libs}
)

add_library(roxlu_opencv STATIC ${opencv_source_files})
