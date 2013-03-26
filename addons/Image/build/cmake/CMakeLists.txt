# bmfont

roxlu_addon_begin("image")
 # --------------------------------------------------------------------------------------
 if(UNIX) 
   roxlu_add_extern_lib(libpng15.a)
   roxlu_add_extern_lib(libz.a)
 endif()

 if(WIN32) 
   roxlu_add_dll(libpng15.dll)
   roxlu_add_extern_lib(libpng15.lib)
   roxlu_add_dll(zlib1.dll)

 endif()

 roxlu_addon_add_source_file(image/PNG.cpp)

 # --------------------------------------------------------------------------------------
roxlu_addon_end()
