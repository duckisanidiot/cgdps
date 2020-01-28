cmake_minimum_required(VERSION 3.10)

project(cgdps LANGUAGES C)

include_directories(include)
link_libraries(pthread mongoc-1.0 bson-1.0 http dl ssl crypto)

file(GLOB_RECURSE SOURCES "src/*.c")

set(CMAKE_BUILD_TYPE Debug)

add_executable(cgdps ${SOURCES})
