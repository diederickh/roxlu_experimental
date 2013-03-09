# libconfig
if(UNIX) 
  roxlu_add_extern_lib(libconfig.a)
  roxlu_add_extern_lib(libconfig++.a)
endif()

if(WIN32) 
  # warning 4290 is about exceptions.. http://stackoverflow.com/questions/3140875/how-to-get-rid-of-c-exception-specification-ignored-warning
  add_definitions("/wd4290")
  roxlu_add_extern_lib(libconfig.lib)
  roxlu_add_extern_lib(libconfig++.lib)
  roxlu_add_dll(libconfig.dll)
  roxlu_add_dll(libconfig++.dll)
endif()
