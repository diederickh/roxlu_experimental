# Audio cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-ggdb")
  endif()
  if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUGS} /Od /ZI")
    endif()
endif()

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

if(ROXLU_PLATFORM_WIN) 

  set(audio_libs 
    ${roxlu_extern_lib_dir}/portaudio_x86.lib
    )

  set(roxlu_install_files
    ${roxlu_install_files}
    ${roxlu_extern_bin_dir}/portaudio_x86.dll
    )

else() 

  set(audio_libs
    ${roxlu_extern_lib_dir}/libportaudio.a
    )

endif(ROXLU_PLATFORM_WIN)

if(APPLE)
 find_library(fr_audio CoreAudio)
 find_library(fr_audio_unit AudioUnit)
 find_library(fr_audio_toolbox AudioToolbox)
 find_library(fr_core_services CoreServices)

 set(audio_libs
   ${audio_libs}
   ${fr_audio}
   ${fr_audio_unit}
   ${fr_audio_toolbox}
   ${fr_core_services}
   )

endif(APPLE)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${audio_libs}
)

add_library(roxlu_audio STATIC ${audio_source_files})

