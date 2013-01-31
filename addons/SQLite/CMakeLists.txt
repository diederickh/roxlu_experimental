# SQLite cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt) # Roxlu cmake

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-ggdb")
  endif()
  if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUGS} /Od /ZI")
    endif()
endif()

set(sqlite_base_dir ${CMAKE_CURRENT_LIST_DIR})
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

set(roxlu_addon_sources 
  ${roxlu_addon_sources} 
  ${sqlite_source_files}
)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
)

add_library(roxlu_sqlite STATIC ${sqlite_source_files})
