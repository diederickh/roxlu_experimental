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

  roxlu_add_extern_lib(libglib-2.0.a)
  roxlu_add_extern_lib(libgobject-2.0.a)
  roxlu_add_extern_lib(libgmodule-2.0.a)
  roxlu_add_extern_lib(libiconv.a)
  roxlu_add_extern_lib(libintl.a)
  roxlu_add_extern_lib(libffi.a)
  roxlu_add_extern_lib(libcairo.a)
  roxlu_add_extern_lib(libpixman-1.a)
  roxlu_add_extern_lib(libpng15.a)
  roxlu_add_extern_lib(libpango-1.0.a)
  roxlu_add_extern_lib(libpangocairo-1.0.a)
  roxlu_add_extern_lib(libfontconfig.a)
  roxlu_add_extern_lib(libpangoft2-1.0.a)
  roxlu_add_extern_lib(libfreetype.a)
  roxlu_add_extern_lib(libz.a)
  roxlu_add_extern_lib(libxml2.a)
  
  roxlu_add_lib(-lbz2)
  roxlu_add_lib(-lexpat)
  roxlu_add_lib(${fr_cf})
  roxlu_add_lib(${fr_as})

endif()

if(ROXLU_PLATFORM_LINUX)
  
  roxlu_add_extern_lib(libiconv.a)
  roxlu_add_extern_lib(libffi.a)
  roxlu_add_extern_lib(libcairo.a)
  roxlu_add_extern_lib(libpixman-1.a)
  roxlu_add_extern_lib(libpng15.a)
  roxlu_add_extern_lib(libpango-1.0.a)
  roxlu_add_extern_lib(libgthread-2.0.a)
  roxlu_add_extern_lib(libgio-2.0.a)
  roxlu_add_extern_lib(libglib-2.0.a)
  roxlu_add_extern_lib(libgobject-2.0.a)
  roxlu_add_extern_lib(libgmodule-2.0.a)
  roxlu_add_extern_lib(libpangocairo-1.0.a)
  roxlu_add_extern_lib(libintl.a)
  roxlu_add_extern_lib(libiconv.a)
  roxlu_add_extern_lib(libpangoft2-1.0.a)

  roxlu_add_lib(-ldl)
  roxlu_add_lib(-lpthread)
  roxlu_add_lib(-lrt)
  roxlu_add_lib(-lfreetype)
  roxlu_add_lib(-lz)
  roxlu_add_lib(-lfontconfig)

  
#  set(pango_libs
#    ${roxlu_extern_lib_dir}/libiconv.a
#    ${roxlu_extern_lib_dir}/libffi.a
#    ${roxlu_extern_lib_dir}/libcairo.a
#    ${roxlu_extern_lib_dir}/libpixman-1.a
#    ${roxlu_extern_lib_dir}/libpng15.a
#    ${roxlu_extern_lib_dir}/libpango-1.0.a
#    ${roxlu_extern_lib_dir}/libgthread-2.0.a
#    ${roxlu_extern_lib_dir}/libgio-2.0.a
#    ${roxlu_extern_lib_dir}/libglib-2.0.a
#    ${roxlu_extern_lib_dir}/libgobject-2.0.a
#    ${roxlu_extern_lib_dir}/libgmodule-2.0.a
#    ${roxlu_extern_lib_dir}/libpangocairo-1.0.a
#    ${roxlu_extern_lib_dir}/libintl.a
#    ${roxlu_extern_lib_dir}/libiconv.a
#    ${roxlu_extern_lib_dir}/libpangoft2-1.0.a
#    -ldl
#    -lpthread
#    -lrt
#    -lfreetype
#    -lz
#    -lfontconfig
#  )

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

source_group("src\\pango" FILES ${pango_source_files})

set(roxlu_addon_sources
  ${roxlu_addon_sources}
  ${pango_source_files}
)

#set(roxlu_addon_libs
#  ${roxlu_addon_libs}
#  ${pango_libs}
#)

add_library(roxlu_pango STATIC ${pango_source_files})


