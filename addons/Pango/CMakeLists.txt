# Pango addon
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt) # roxlu
set(pango_base_dir ${CMAKE_CURRENT_LIST_DIR})
set(pango_src_dir ${pango_base_dir}/src/)
set(pango_include_dir ${pango_base_dir}/include/)

include_directories(
  ${pango_include_dir}
  ${roxlu_extern_include_dir}/pango-1.0/
  ${roxlu_extern_include_dir}/glib-2.0/
  ${roxlu_extern_include_dir}/cairo/
  ${roxlu_extern_lib_dir}/glib-2.0/include/
)

set(pango_libs
  ${roxlu_extern_lib_dir}/libglib-2.0.a
  ${roxlu_extern_lib_dir}/libgobject-2.0.a
  ${roxlu_extern_lib_dir}/libgmodule-2.0.a
  ${roxlu_extern_lib_dir}/libiconv.a
  ${roxlu_extern_lib_dir}/libintl.a
  ${roxlu_extern_lib_dir}/libffi.a
  ${roxlu_extern_lib_dir}/libcairo.a
  ${roxlu_extern_lib_dir}/libpixman-1.a
  ${roxlu_extern_lib_dir}/libpng15.a
  ${roxlu_extern_lib_dir}/libpango-1.0.a
  ${roxlu_extern_lib_dir}/libpangocairo-1.0.a
)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${pango_libs}
)

set(pango_source_files
  ${pango_src_dir}/pango/TextSurface.cpp
)
set(roxlu_addon_sources
  ${roxlu_addon_sources}
  ${pango_source_files}
)

if(APPLE)
  set(roxlu_linker_flags
   ${roxlu_linker_flags}
   "${CMAKE_EXE_LINKER_FLAGS}  -framework CoreFoundation -framework QuartzCore -framework ApplicationServices -lz -lbz2 -lexpat")
endif(APPLE)

add_library(roxlu_pango STATIC ${pango_source_files})


