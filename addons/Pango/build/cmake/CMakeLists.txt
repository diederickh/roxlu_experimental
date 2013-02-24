# Pango addon
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/build/cmake/CMakeLists.txt) # roxlu

set(pango_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(pango_src_dir ${pango_base_dir}/src/)
set(pango_include_dir ${pango_base_dir}/include/)

include_directories(
  ${pango_include_dir}
  ${roxlu_extern_include_dir}/pango-1.0/
  ${roxlu_extern_include_dir}/glib-2.0/
  ${roxlu_extern_include_dir}/cairo/
  ${roxlu_extern_include_dir}/freetype2/          # experimental
  ${roxlu_extern_lib_dir}/glib-2.0/include/

)

if(ROXLU_PLATFORM_MAC)
  find_library(fr_cf CoreFoundation)
  find_library(fr_as ApplicationServices)

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
    ${fr_cf}
    ${fr_as}
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
  roxlu_add_extern_lib(pango-1.0.lib)
  roxlu_add_extern_lib(pangocairo-1.0.lib)
  roxlu_add_extern_lib(cairo.lib)
  roxlu_add_extern_lib(glib-2.0.lib)
  roxlu_add_extern_lib(gobject-2.0.lib)
  roxlu_add_extern_lib(pixman-1.lib)
  
  roxlu_add_dll(pango-1.dll)
  roxlu_add_dll(pangocairo-1.dll)
  roxlu_add_dll(cairo.dll)

  roxlu_add_dll(gmodule-2.dll)
  roxlu_add_dll(glib-2.dll)
  roxlu_add_dll(gobject-2.dll)
  roxlu_add_dll(libintl.dll)
  roxlu_add_dll(iconv.dll)
  roxlu_add_dll(libpng15.dll)
  roxlu_add_dll(zlib1.dll)
  roxlu_add_dll(pangowin32-1.dll)
endif()

set(pango_source_files
  ${pango_src_dir}/pango/TextSurface.cpp
  ${pango_src_dir}/pango/TextGLSurface.cpp
)

set(roxlu_addon_sources
  ${roxlu_addon_sources}
  ${pango_source_files}
)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${pango_libs}
)

add_library(roxlu_pango STATIC ${pango_source_files})


