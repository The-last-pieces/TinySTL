cmake_minimum_required(VERSION 3.17)
project(tinySTL)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
include_directories(src/core/adapter)
include_directories(src/core/algorithm)
include_directories(src/core/allocator)
include_directories(src/core/container)
include_directories(src/core/functor)
include_directories(src/core/iterator)
include_directories(src/utils)

add_executable(tinySTL
        src/core/allocator/memory.h
        src/utils/profiler.h
        src/main.cpp
        src/core/iterator/iterator.h)
