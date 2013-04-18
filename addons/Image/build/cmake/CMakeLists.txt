# image

roxlu_addon_begin("image")
 # --------------------------------------------------------------------------------------
 if(UNIX) 
   roxlu_add_extern_lib(libpng15.a)
   roxlu_add_extern_lib(libz.a)
   roxlu_add_extern_lib(libjpeg.a)
 endif()

 if(WIN32) 
   roxlu_add_dll(libpng15.dll)
   roxlu_add_dll(zlib1.dll)

   roxlu_add_extern_lib(libjpeg.lib)

   if(CMAKE_BUILD_TYPE STREQUAL Debug)
     roxlu_add_extern_lib(libpng15-static.lib)
     message("NOTE THAT LIBPNG ON WINDOWS RESULTS IN UNEXPECTED CRASHES. RELEASE BUILDS WORK FINE, BUT I HAVENT FOUND TIME TO FIGURE OUT WHATS GOING WRONG, IT SEEMS A LINKER/CODE GENERATION PROBLEM")
   else()
     roxlu_add_extern_lib(libpng15.lib)
   endif()

   roxlu_add_extern_lib(zlib.lib)
 endif()

 roxlu_addon_add_source_file(image/Image.cpp)
 roxlu_addon_add_source_file(image/PNG.cpp)
 roxlu_addon_add_source_file(image/TGA.cpp)
 roxlu_addon_add_source_file(image/JPG.cpp)


 # --------------------------------------------------------------------------------------
roxlu_addon_end()
