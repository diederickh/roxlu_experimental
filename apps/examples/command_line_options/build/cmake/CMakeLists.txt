# Command Line Options parsing using the excellent argtable library

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_add_addon("CommandLineOptions")

roxlu_app_initialize("command_line_options")
   # ---------------------------------------------
   roxlu_app_add_source_file(main.cpp)
   # ---------------------------------------------
roxlu_install_app()
