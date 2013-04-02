cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_add_addon("Log")

# Uncomment this line, if you want to remove all logging code except from RX_ERROR() from your source
# add_definitions(-DRX_LOG_LEVEL=RX_LOG_LEVEL_ERROR)

roxlu_app_initialize("logger")
   # ---------------------------------------------
   roxlu_app_add_source_file(main.cpp)
   # ---------------------------------------------
roxlu_install_app()
