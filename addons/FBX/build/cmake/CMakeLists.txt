# FBX sdk wrapper

roxlu_addon_begin("fbx")

  # --------------------------------------------------------------------------------------
  #roxlu_addon_add_source_file(protractor/Protractor.cpp)
  add_definitions(-DFBXSDK_NEW_API)
  if(APPLE)
    roxlu_add_extern_dylib(libfbxsdk-2013.3.dylib)
  endif()
  # --------------------------------------------------------------------------------------

roxlu_addon_end()
