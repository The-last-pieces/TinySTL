//
// Created by IMEI on 2022/5/7.
//

#ifndef TINYSTL_DEQUE_TEST_H
#define TINYSTL_DEQUE_TEST_H

#include "../container/deque.h"
#include "../utils/profiler.h"
#include "../utils/test_helper.h"
#include <deque>
#include <iterator>

namespace ttl::ttl_test {

    class deque_test {
    public:
        static void runAll() {
            test1();
        }

    private:
        static void test1() {
            const int n = 10000000;
            std::deque<int> sd;
            ttl::deque<int> td;
            TTL_STL_COMPARE(td, sd, {
                for (int i = 1; i <= n; ++i) v.emplace_front(i);
                for (int i = 1; i <= n; ++i) v.emplace_back(i);
                for (int i = 1; i <= n; ++i) v.emplace_front(i);
            }, "deque push");
            same(sd, td);
        }
    };

}

#endif //TINYSTL_DEQUE_TEST_H
