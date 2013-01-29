# Pango addon
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt) # roxlu

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-ggdb")
  endif()
  if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUGS} /Od /ZI")
    endif()
endif()

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

if(ROXLU_PLATFORM_MAC)

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
  ${roxlu_extern_lib_dir}/libfontconfig.a
  ${roxlu_extern_lib_dir}/libpangoft2-1.0.a
  ${roxlu_extern_lib_dir}/libfreetype.a
  ${roxlu_extern_lib_dir}/libz.a
  ${roxlu_extern_lib_dir}/libxml2.a
  -lbz2
  -lexpat
)

endif()

if(ROXLU_PLATFORM_LINUX)
  set(pango_libs
    ${roxlu_extern_lib_dir}/libiconv.a
    ${roxlu_extern_lib_dir}/libffi.a
    ${roxlu_extern_lib_dir}/libcairo.a
    ${roxlu_extern_lib_dir}/libpixman-1.a
    ${roxlu_extern_lib_dir}/libpng15.a
    ${roxlu_extern_lib_dir}/libpango-1.0.a
    ${roxlu_extern_lib_dir}/libgthread-2.0.a
    ${roxlu_extern_lib_dir}/libgio-2.0.a
    ${roxlu_extern_lib_dir}/libglib-2.0.a
    ${roxlu_extern_lib_dir}/libgobject-2.0.a
    ${roxlu_extern_lib_dir}/libgmodule-2.0.a
    ${roxlu_extern_lib_dir}/libpangocairo-1.0.a
    ${roxlu_extern_lib_dir}/libintl.a
    ${roxlu_extern_lib_dir}/libiconv.a
    ${roxlu_extern_lib_dir}/libpangoft2-1.0.a
    -ldl
    -lpthread
    -lrt
    -lfreetype
    -lz
    -lfontconfig
  )
endif()

if(ROXLU_PLATFORM_WIN) 
  set(pango_libs
    ${roxlu_extern_lib_dir}/pango-1.0.lib
    ${roxlu_extern_lib_dir}/pangocairo-1.0.lib
    ${roxlu_extern_lib_dir}/cairo.lib
    ${roxlu_extern_lib_dir}/glib-2.0.lib
    ${roxlu_extern_lib_dir}/gobject-2.0.lib
    ${roxlu_extern_lib_dir}/pixman-1.lib
    )
  set(roxlu_install_files
    ${roxlu_install_files}
    ${roxlu_extern_bin_dir}/pango-1.dll
    ${roxlu_extern_bin_dir}/pangocairo-1.dll
    ${roxlu_extern_bin_dir}/cairo.dll
    ${roxlu_extern_bin_dir}/gmodule-2.dll
    ${roxlu_extern_bin_dir}/glib-2.dll
    ${roxlu_extern_bin_dir}/gobject-2.dll
    ${roxlu_extern_bin_dir}/libintl.dll
    ${roxlu_extern_bin_dir}/iconv.dll
    ${roxlu_extern_bin_dir}/libpng15.dll
    ${roxlu_extern_bin_dir}/zlib1.dll
    ${roxlu_extern_bin_dir}/pangowin32-1.dll
    )
endif()


set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${pango_libs}
)

set(pango_source_files
  ${pango_src_dir}/pango/TextSurface.cpp
  ${pango_src_dir}/pango/TextGLSurface.cpp
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


