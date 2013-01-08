cmake_minimum_required (VERSION 2.8)
set(libname "Trackpad")

project(${libname})

set(CMAKE_OSX_ARCHITECTURES i386)

include_directories(src)

add_library(${libname} STATIC src/Trackpad.mm)

target_link_libraries(${libname} "-framework Foundation")

install(TARGETS ${libname}
  LIBRARY DESTINATION .
  ARCHIVE DESTINATION .
)


set_target_properties(${libname} PROPERTIES PREFIX "")
