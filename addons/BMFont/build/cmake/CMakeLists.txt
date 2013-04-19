# bmfont

roxlu_add_addon(Image)

roxlu_addon_begin("bmfont")
 # --------------------------------------------------------------------------------------
 roxlu_addon_add_source_file(bmfont/BMFTypes.cpp)
 roxlu_addon_add_source_file(bmfont/BMFShader.cpp)

 roxlu_add_include_dir(${roxlu_extern_include_dir}/rapidxml/)
 # --------------------------------------------------------------------------------------
roxlu_addon_end()
