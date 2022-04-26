#include "./utils/profiler.h"
#include "./core/allocator/memory.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;
using namespace ttl;

// write all test code
int main() {
    {
        std::allocator<string> allocator;
        auto *ptr = allocator.allocate(1000000);
        {
            auto_timer timer("stl");
            std::uninitialized_fill(ptr, ptr + 1000000, "123");
        }
        allocator.deallocate(ptr, 1000000);
    }
    {
        ttl::allocator<string> allocator;
        auto *ptr = allocator.allocate(1000000);
        {
            auto_timer timer("ttl");
            ttl::uninitialized_fill(ptr, ptr + 1000000, "123");
        }
        allocator.deallocate(ptr, 1000000);
    }
    return 0;
}