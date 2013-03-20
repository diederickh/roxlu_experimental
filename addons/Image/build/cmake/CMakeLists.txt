# bmfont

roxlu_addon_begin("image")
 # --------------------------------------------------------------------------------------
 if(UNIX) 
   roxlu_add_extern_lib(libpng15.a)
   roxlu_add_extern_lib(libz.a)
 endif()

 roxlu_addon_add_source_file(image/PNG.cpp)

 # --------------------------------------------------------------------------------------
roxlu_addon_end()
