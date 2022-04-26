cmake_minimum_required(VERSION 3.17)
project(tinySTL)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
include_directories(src/adapter)
include_directories(src/algorithm)
include_directories(src/allocator)
include_directories(src/container)
include_directories(src/functor)
include_directories(src/iterator)
include_directories(src/utils)

add_executable(tinySTL
        src/allocator/allocator.h
        src/utils/profiler.h
        src/main.cpp
        )
