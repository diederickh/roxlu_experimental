cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_add_addon(Curl)
roxlu_add_addon(SQLite)

set(uploader_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(uploader_install_dir ${uploader_base_dir}/bin/)

# UPLOADER
# --------------------------------------------
set(uploader_sources
  ${uploader_base_dir}src/uploader.cpp
  ${roxlu_addon_sources}
  ${roxlu_sources}
)


if(LINUX)
  set(uploader_libs
    ${roxlu_addon_libs}
    ${roxlu_extern_lib_dir}/libuv.a
    )
endif()

if(APPLE)
  find_library(fr_foundation CoreFoundation)
  find_library(fr_cs CoreServices)
  set(uploader_libs
    ${uploader_libs}
    ${fr_foundation}
    ${fr_cs}  
    )  
endif()

if(WIN32)
  set(uploader_libs
    ${uploader_libs}
    ${roxlu_addon_libs}
    ${roxlu_extern_lib_dir}/libuv.lib
    )
  
endif()


add_executable(uploader ${uploader_sources})
target_link_libraries(uploader ${uploader_libs})
install(TARGETS uploader DESTINATION ${uploader_install_dir})
install(FILES ${roxlu_install_files} DESTINATION ${uploader_install_dir})

# FAKE INSERTER, inserts entries into the DB
# --------------------------------------------
set(uploader_fake_inserter_sources
  ${uploader_base_dir}src/uploader_fake_inserter.cpp
  ${roxlu_addon_sources}
  ${roxlu_sources}
)

find_library(fr_foundation CoreFoundation)
find_library(fr_cs CoreServices)

add_executable(uploader_fake_inserter ${uploader_fake_inserter_sources})
target_link_libraries(uploader_fake_inserter ${uploader_libs})
install(TARGETS uploader_fake_inserter DESTINATION ${uploader_install_dir})
