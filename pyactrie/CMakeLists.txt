cmake_minimum_required (VERSION 3.4)
project (actrie)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

find_package(Boost COMPONENTS python REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

include_directories (..)

aux_source_directory(. SRCS)

set(CMAKE_POSITION_INDEPENDENT_CODE  ON)

add_library(pyactrie SHARED ${SRCS})
set_target_properties(pyactrie PROPERTIES PREFIX "")
target_link_libraries(pyactrie ${Boost_LIBRARIES})
target_link_libraries(pyactrie ${PYTHON_LIBRARIES})

#message("${Boost_LIBRARIES}")
#message("${PYTHON_LIBRARIES}")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/example.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/forbid.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
