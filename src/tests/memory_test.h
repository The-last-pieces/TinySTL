//
// Created by IMEI on 2022/4/28.
//

#ifndef TINYSTL_MEMORY_TEST_H
#define TINYSTL_MEMORY_TEST_H

#include "../allocator/memory.h"
#include "../utils/profiler.h"
#include <memory>
#include <string>

namespace ttl::ttl_test {
    class memory_test {
    public:
        static void runAll() {
            test1();
        }

    private:
        static void test1() {
            {   // 分配速度
                {
                    std::allocator<std::string> allocator;
                    auto_timer timer("stl");
                    for (int i = 1; i <= 100000; ++i) allocator.deallocate(allocator.allocate(i), i);
                }
                {
                    ttl::allocator<std::string> allocator;
                    auto_timer timer("ttl");
                    for (int i = 1; i <= 100000; ++i) allocator.deallocate(allocator.allocate(i), i);
                }
            }
            {   // 填充速度
                std::allocator<std::string> allocator;
                auto *ptr = allocator.allocate(10000000);
                {
                    auto_timer timer("stl");
                    std::uninitialized_fill(ptr, ptr + 1000000, "hello world");
                }
                {
                    auto_timer timer("ttl");
                    ttl::uninitialized_fill(ptr + 10000000 - 1000000, ptr + 10000000, "hello world");
                }
                allocator.deallocate(ptr, 10000000);
            }
        }
    };
}

#endif //TINYSTL_MEMORY_TEST_H
