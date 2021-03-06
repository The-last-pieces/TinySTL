﻿cmake_minimum_required(VERSION 3.17)
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
        src/core/iterator/iterator.h
        src/core/container/vector.h
        src/core/algorithm/algorithm.h
        src/tests/vector_test.h
        src/tests/memory_test.h
        src/utils/test_helper.h
        src/core/container/list.h
        src/tests/list_test.h
        src/core/container/private/hashtable.h
        src/tests/hashtable_test.h
        src/core/container/unordered_map.h
        src/tests/unordered_map_test.h
        src/core/adapter/priority_queue.h
        src/tests/priority_queue_test.h
        src/core/container/expand/bitset.h
        src/tests/bitset_test.h
        src/core/container/deque.h
        src/tests/deque_test.h
        src/core/container/expand/avl_tree.h
        src/core/functor/function.h
        src/core/normal/tuple.h
        src/core/normal/smart_ptr.h
        src/tests/smart_ptr_test.h
        src/core/container/private/bs_tree.h
        src/tests/bs_tree_test.h
        src/core/container/expand/segment_tree.h
        src/tests/segment_tree_test.h
        src/core/container/expand/trie.h
        src/core/container/expand/union_set.h
        src/core/container/expand/lru_cache.h
        src/core/container/expand/bignum.h)
