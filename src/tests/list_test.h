//
// Created by IMEI on 2022/4/30.
//

#ifndef TINYSTL_LIST_TEST_H
#define TINYSTL_LIST_TEST_H

#include "../container/list.h"
#include "../utils/profiler.h"
#include "../utils/test_helper.h"
#include <list>
#include <algorithm>
#include <functional>

namespace ttl::ttl_test {
    class list_test {
    public:
        static void runAll() {
            test1();
        }

    private:
        static void test1() {
            ttl::list<std::string> tl;
            std::list<std::string> sl;
            TTL_STL_COMPARE(tl, sl, {
                v.emplace_front("hhh2");
                v.emplace_front("hhh2");
                v.emplace_front("hhh2");
                v.assign(112300, "hello");
                v.insert(-- -- --v.end(), 102420, "world");
                v.emplace_back("hhh");
                v.pop_front();
                v.pop_front();
                v.erase(++ ++ ++v.begin(), -- -- --v.end());
            }, "list insert & remove");
            assert(ttl::equal(tl.begin(), tl.end(), sl.begin(), sl.end()));
        }
    };
}

#endif //TINYSTL_LIST_TEST_H
