# Twitter streaming user example
# Shows a stream of a user

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_add_addon(Twitter)

roxlu_app_initialize("twitter_streaming_user_example")
roxlu_app_add_source_file(main.cpp)

roxlu_install_app()
