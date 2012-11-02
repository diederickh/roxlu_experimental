cmake_minimum_required(VERSION 2.6)
project(libuv_openssl1)

add_executable(libuv_openssl1 main.cpp ${files})
target_link_libraries(libuv_openssl1 ${libs})


