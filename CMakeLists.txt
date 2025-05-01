cmake_minimum_required(VERSION 3.13)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(compilerception)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")

find_package(Python3 REQUIRED)

# set(PROGRAM_FILE "${CMAKE_CURRENT_SOURCE_DIR}/program.lox")
# set(PROGRAM_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/program.hpp")
#
# add_custom_command(
#     OUTPUT ${PROGRAM_HEADER}
#     COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_program_header.py
#             ${PROGRAM_FILE} ${PROGRAM_HEADER}
#     DEPENDS ${PROGRAM_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_program_header.py
#     COMMENT "Generating program header"
# )

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES include/*.hpp)

add_executable(interpreter ${SOURCE_FILES} ${HEADER_FILES} ${PROGRAM_HEADER})
