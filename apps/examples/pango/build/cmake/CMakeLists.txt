# Pango
# Basic example which shows the usage of pango.

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_add_addon(Pango)

roxlu_app_initialize("pango")
roxlu_app_add_source_file(main.cpp)

roxlu_install_app()
