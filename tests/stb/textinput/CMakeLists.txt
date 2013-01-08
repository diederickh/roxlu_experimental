cmake_minimum_required(VERSION 2.6)

set(rdir ../../../lib/)

set(sources
  main.cpp
  ${rdir}src/roxlu/external/stb_textedit.cpp
  ${rdir}src/roxlu/experimental/Input.cpp
)

include_directories(
  ${rdir}/include/
)

add_executable(textedit ${sources})