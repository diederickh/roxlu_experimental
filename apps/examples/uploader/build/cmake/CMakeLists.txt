# uploader

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake


roxlu_add_addon(Curl)
roxlu_add_addon(SQLite)
roxlu_add_addon(Config)
roxlu_add_addon(UV)


roxlu_app_initialize("uploader")
# ---------------------------------
  roxlu_app_add_source_file(uploader.cpp)
# ---------------------------------
roxlu_install_app()
