# Twitter
# Example shows how to retrieve authorization tokens for a twitter application

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_add_addon(Twitter)

roxlu_app_initialize("twitter_get_tokens")
roxlu_app_add_source_file(main.cpp)

roxlu_install_app()
