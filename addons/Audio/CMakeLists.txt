# Audio cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt)

if(ROXLU_PLATFORM_MAC)
set(roxlu_linker_flags 
   ${roxlu_linker_flags} 
)	    
endif(ROXLU_PLATFORM_MAC)

set(audio_base_dir ${CMAKE_CURRENT_LIST_DIR})
set(audio_src_dir ${audio_base_dir}/src)
set(audio_include_dir ${audio_base_dir}/include)
set(audio_lib_dir ${audio_base_dir}/lib/${roxlu_platform}/${roxlu_bits})

include_directories(
  ${audio_include_dir}
  ${audio_src_dir}
)

set(audio_source_files
  ${audio_src_dir}/audio/Audio.cpp
  ${audio_src_dir}/audio/PCMWriter.cpp
)

set(roxlu_addon_sources 
  ${roxlu_addon_sources} 
  ${audio_source_files}
)

set(audio_libs
  ${roxlu_extern_lib_dir}/libportaudio.a
  ${roxlu_extern_lib_dir}/libportaudiocpp.a
)

if(APPLE)
  set(CMAKE_EXE_LINKER_FLAGS " ${CMAKE_EXE_LINKER_FLAGS} -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices "
)
endif(APPLE)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${audio_libs}
)

add_library(roxlu_audio STATIC ${audio_source_files})
