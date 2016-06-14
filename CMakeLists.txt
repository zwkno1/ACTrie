cmake_minimum_required (VERSION 3.4)

project (ACTrie)
include_directories(./src)

add_subdirectory(examples)


option(BUILD_PYACTRIE "" OFF)

if(BUILD_PYACTRIE)
    add_subdirectory(pyactrie)
endif(BUILD_PYACTRIE)


