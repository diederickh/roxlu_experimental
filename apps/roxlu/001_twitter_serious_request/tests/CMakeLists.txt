cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/CMakeLists.txt) # roxlu cmake

set(base_dir ${CMAKE_CURRENT_LIST_DIR})
set(mongo_include_dir ${base_dir}/../src/mongo/include)
set(mongo_lib_dir ${base_dir}/../src/mongo/lib/${roxlu_platform}/${roxlu_bits})

set(mongo_libs
        ${mongo_lib_dir}/libmongoc.a
)

include_directories(
        ${mongo_include_dir}
)

#add_subdirectory(${base_dir}/gridfs)


