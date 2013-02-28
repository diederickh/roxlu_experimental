# Twitter streaming example

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

if(WIN32)
  message(FATAL_ERROR "The streaming example will work on windows, but I need to find a replacement for unistd.h")
endif()

roxlu_add_addon(Twitter)

roxlu_app_initialize("twitter_streaming_example")
roxlu_app_add_source_file(main.cpp)

roxlu_install_app()


