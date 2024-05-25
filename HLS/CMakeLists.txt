cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS "-g")

# set(CMAKE_BUILD_TYPE "Release")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(hls)    # addr. for Random Netlist Generator

# clang
set(CMAKE_EXPORT_COMPILE_COMMANDS True)


# define macros
# add_definitions(-DUSE_EDGE_POINTER=0)

# get every directory in the src folder
FILE(GLOB_RECURSE ROOT_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE SUB_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(ALL_SRC ${ROOT_SRC_FILES} ${SUB_SRC_FILES})

add_executable(${PROJECT_NAME} ${ALL_SRC})

# SET EXE NAME
# if debug mode, add _d to the name
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}${CMAKE_DEBUG_POSTFIX})
