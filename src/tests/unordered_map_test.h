//
// Created by IMEI on 2022/5/4.
//

#ifndef TINYSTL_UNORDERED_MAP_TEST_H
#define TINYSTL_UNORDERED_MAP_TEST_H

#include "../container/unordered_map.h"
#include "../utils/profiler.h"
#include "../utils/test_helper.h"
#include <unordered_map>

namespace ttl::ttl_test {

    class unordered_map_test {
        template<typename T1, typename T2>
        static void un_map_same(
                ttl::unordered_map<T1, T2> &tm,
                std::unordered_map<T1, T2> &sm) {
            std::unordered_map<T1, T2> tmp;
            for (auto &kv: tm) tmp.insert(kv);
            assert(tmp == sm);
        }

    public:
        static void runAll() {
            test1();
        }

    private:
        static void test1() {
            ttl::unordered_map<int, int> tm;
            std::unordered_map<int, int> sm;
            std::vector<int> rd = randIntArray(10000);
            TTL_STL_COMPARE(tm, sm, {
                for (auto x: rd) v[x] = x;
            }, "u_map insert");
            un_map_same(tm, sm);
        }
    };

}
#endif //TINYSTL_UNORDERED_MAP_TEST_H
