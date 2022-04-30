//
// Created by IMEI on 2022/4/28.
//

#ifndef TINYSTL_MEMORY_TEST_H
#define TINYSTL_MEMORY_TEST_H

#include "../allocator/memory.h"
#include "../utils/profiler.h"
#include "../utils/test_helper.h"
#include <memory>
#include <string>

namespace ttl::ttl_test {
    class memory_test {
    public:
        static void runAll() {
            test1();
            test2();
        }

    private:
        // 分配速度
        static void test1() {
            ttl::allocator<std::string> tv;
            std::allocator<std::string> sv;
            TTL_STL_COMPARE(tv, sv, {
                for (int i = 1; i <= 1000; ++i) v.deallocate(v.allocate(i), i);
            }, "alloc speed");

        }

        // 填充速度
        static void test2() {
            std::allocator<std::string> allocator;
            auto *ptr = allocator.allocate(10000000);
            {
                auto_timer timer("stl fill");
                std::uninitialized_fill(ptr, ptr + 1000000, "hello world");
            }
            {
                auto_timer timer("ttl fill");
                ttl::uninitialized_fill(ptr + 10000000 - 1000000, ptr + 10000000, "hello world");
            }
            allocator.deallocate(ptr, 10000000);
        }
    };
}

#endif //TINYSTL_MEMORY_TEST_H
