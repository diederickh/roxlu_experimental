cmake_minimum_required(VERSION 2.6)
project(libuv_openssl2)

add_executable(libuv_openssl2 main.cpp ${files})
target_link_libraries(libuv_openssl2 ${libs})


