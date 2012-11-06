# Curl/tests/curl_download CMakeLists.txt
cmake_minimum_required(VERSION 2.8)

get_filename_component(curl_test_name ${CMAKE_CURRENT_LIST_DIR} NAME)

file(GLOB curl_test_sources *.cpp)

add_executable(${curl_test_name} ${curl_test_sources} ${curl_source_files})

target_link_libraries(${curl_test_name} ${curl_libs})