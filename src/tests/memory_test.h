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
            const int n = 10000000, m = 1000000;
            std::allocator<std::string> allocator;
            auto *ptr = allocator.allocate(n);
            TTL_STL_COMPARE_2(
                    {
                        ttl::uninitialized_fill(ptr + n - m, ptr + n, "hello world");
                    },
                    {
                        std::uninitialized_fill(ptr, ptr + m, "hello world");
                    }, "fill"
            );
            ttl::equal(ptr + n - m, ptr + n, ptr, ptr + m);
            ttl::destroy(ptr, ptr + m);
            ttl::destroy(ptr + n - m, ptr + n);
            allocator.deallocate(ptr, n);
        }
    };
}

#endif //TINYSTL_MEMORY_TEST_H
