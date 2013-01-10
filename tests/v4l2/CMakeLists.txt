cmake_minimum_required(VERSION 2.8)

set(source_files 
  src/main.cpp
  src/V4L2Capture.cpp
)

set(libs
  udev
)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

add_executable(v4l2_test ${source_files})
target_link_libraries(v4l2_test ${libs})
