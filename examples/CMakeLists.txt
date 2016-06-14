cmake_minimum_required (VERSION 3.4)
project (Examples)

set (CMAKE_CXX_STANDARD 11)

include_directories (..)

aux_source_directory(. SRCS)

add_executable(Example1
        ${SRCS}
        )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/forbid.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

