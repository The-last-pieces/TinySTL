//
// Created by IMEI on 2022/4/29.
//

#ifndef TINYSTL_TEST_HELPER_H
#define TINYSTL_TEST_HELPER_H

#include <type_traits>
#include <random>
#include <cassert>
#include "profiler.h"
#include "../algorithm/algorithm.h"

namespace ttl::ttl_test {
#define TTL_STL_COMPARE(tv, sv, code, name)         \
    do{                                             \
        {                                           \
        auto &v = tv;                               \
        auto_timer timer("ttl " name);              \
        {code;}                                     \
        }                                           \
        {                                           \
        auto &v = sv;                               \
        auto_timer timer("stl " name);              \
        {code;}                                     \
        }                                           \
    }while(false)                                   \


    int randInt(int l = 0, int r = RAND_MAX) {
        static auto maker = std::mt19937(clock());
        return int(maker()) % (r - l) + l;
    }

    std::string randStr(int len, const char *base = nullptr) {
        std::string ret;
        for (int i = 0; i < len; ++i) ret.push_back(base ? base[i] : char('a' + randInt(0, 26)));
        return ret;
    }

    template<typename T1, typename T2>
    static void same(const T1 &sa, const T2 &ta) {
        assert(ttl::equal(sa.begin(), sa.end(), ta.begin(), ta.end()));
    }
}

#endif //TINYSTL_TEST_HELPER_H
