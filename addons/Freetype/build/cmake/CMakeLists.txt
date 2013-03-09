# Freetype cmakelists.txt

roxlu_addon_begin("freetype")
  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(freetype/FreetypeFont.cpp)

  roxlu_add_include_dir(${roxlu_extern_include_dir}/freetype2/)

  if(WIN32)
    roxlu_add_extern_lib(freetype.lib)
  endif()

  if(UNIX)
    roxlu_add_extern_lib(libfreetype.a)
    roxlu_add_extern_lib(libz.a)
    roxlu_add_lib(-lbz2)
  endif()

  if(APPLE)
  endif(APPLE)

  # --------------------------------------------------------------------------------------
roxlu_addon_end()

