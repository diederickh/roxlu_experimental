# Curl/tests CMakeLists.txt
cmake_minimum_required(VERSION 2.8)

#include(${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt) # curl
message(${curl_libs})

add_subdirectory(curl_download)