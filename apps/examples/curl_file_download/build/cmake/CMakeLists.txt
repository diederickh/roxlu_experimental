# Curl file downloader
# Shows how to use the curl addon to download a file from a webserver (non-blocking)

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_add_addon(Curl)

roxlu_app_initialize("curl_file_download")
roxlu_app_add_source_file(main.cpp)
roxlu_install_app()
