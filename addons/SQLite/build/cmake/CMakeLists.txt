# SQLite cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/build/cmake/CMakeLists.txt) # Roxlu cmake

set(sqlite_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(sqlite_src_dir ${sqlite_base_dir}/src)
set(sqlite_include_dir ${sqlite_base_dir}/include)
set(sqlite_lib_dir ${sqlite_base_dir}/lib/${roxlu_platform}/${roxlu_bits})

include_directories(
  ${sqlite_include_dir}
  ${sqlite_base_dir}/extern/sqlite/
)

set(sqlite_source_files
  ${sqlite_src_dir}/sqlite/Database.cpp
  ${sqlite_src_dir}/sqlite/Query.cpp
  ${sqlite_src_dir}/sqlite/QueryDelete.cpp
  ${sqlite_src_dir}/sqlite/QueryInsert.cpp
  ${sqlite_src_dir}/sqlite/QueryParam.cpp
  ${sqlite_src_dir}/sqlite/QueryParams.cpp
  ${sqlite_src_dir}/sqlite/QueryResult.cpp
  ${sqlite_src_dir}/sqlite/QuerySelect.cpp
  ${sqlite_base_dir}/extern/sqlite/sqlite3.c
)

if(UNIX AND NOT APPLE)
  roxlu_add_lib(pthread)
  roxlu_add_lib(dl)

#  set(roxlu_addon_libs
#    ${roxlu_addon_libs}
#    pthread
#    dl
#    )
endif()

set(roxlu_addon_sources 
  ${roxlu_addon_sources} 
  ${sqlite_source_files}
)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
)

add_library(roxlu_sqlite STATIC ${sqlite_source_files})
