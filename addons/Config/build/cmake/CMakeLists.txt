# libconfig
if(UNIX) 
  roxlu_add_extern_lib(libconfig.a)
  roxlu_add_extern_lib(libconfig++.a)
endif()

if(WIN32) 
  roxlu_add_extern_lib(libconfig.lib)
  roxlu_add_dll(libconfig.dll)
endif()
