# pango
roxlu_addon_begin("pango")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(pango/TextSurface.cpp)
  roxlu_addon_add_source_file(pango/TextGLSurface.cpp)

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
    roxlu_add_extern_lib(libharfbuzz.a)
    
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
  
    roxlu_add_lib(dl)
    roxlu_add_lib(pthread)
    roxlu_add_lib(rt)
    roxlu_add_lib(freetype)
    roxlu_add_lib(z)
    roxlu_add_lib(fontconfig)

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
  # --------------------------------------------------------------------------------------

roxlu_addon_end()


