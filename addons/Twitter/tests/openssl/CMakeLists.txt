cmake_minimum_required(VERSION 2.6)
project(openssl_test)

add_executable(openssl_test main.cpp ${files})
target_link_libraries(openssl_test ${libs})


