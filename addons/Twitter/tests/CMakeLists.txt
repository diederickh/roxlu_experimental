cmake_minimum_required(VERSION 2.6)

option(USE_OPENSSL "Use openSSL" ON)

if(USE_OPENSSL)
  add_definitions(-DUSE_OPENSSL)
endif(USE_OPENSSL)

set(CMAKE_OSX_ARCHITECTURES "i386")
set(CMAKE_BUILD_TYPE Debug)
set(BASE ${CMAKE_SOURCE_DIR}/../)
set(SRC ${CMAKE_SOURCE_DIR}/../src/)

include_directories(
  ${BASE}/include/
  ${BASE}/extern/include/
  ${BASE}/src/
)

set(files 
  ${SRC}/twitter/OAuth.cpp 
  ${SRC}/twitter/HTTP.cpp 
  ${SRC}/twitter/Twitter.cpp 
  ${SRC}/twitter/Buffer.cpp 
  ${SRC}/twitter/PercentEncode.cpp
  ${BASE}/extern/src/formats/hmac_sha1.c
  ${BASE}/extern/src/formats/json.c
  ${BASE}/extern/src/formats/Base64Encoder.cpp
)

if(NOT USE_OPENSSL)
  list(APPEND ${files}  ${BASE}/extern/src/formats/sha1.c)
endif(NOT USE_OPENSSL)

set(libs 
  ${CMAKE_SOURCE_DIR}/../lib/libevent.a
  ${CMAKE_SOURCE_DIR}/../lib/libevent_openssl.a
  ${CMAKE_SOURCE_DIR}/../lib/libssl.a
  ${CMAKE_SOURCE_DIR}/../lib/libcrypto.a
)

add_subdirectory(https)



