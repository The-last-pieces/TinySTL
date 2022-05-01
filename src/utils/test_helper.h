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
        free_timer timer;                           \
        time_type s_cost,t_cost;                    \
        printf("%-30s : stl ~", (name));            \
        {                                           \
        auto &v = sv;                               \
        timer.start();                              \
        {code;}                                     \
        s_cost = timer.get_ns();                    \
        }                                           \
        printf(" ttl ~");                           \
        {                                           \
        auto &v = tv;                               \
        timer.start();                              \
        {code;}                                     \
        t_cost = timer.get_ns();                    \
        }                                           \
        report(t_cost, s_cost);                     \
    }while(false)                                   \


#define TTL_STL_COMPARE_2(t_code, s_code, name)     \
    do{                                             \
        free_timer timer;                           \
        time_type s_cost,t_cost;                    \
        printf("%-30s : stl ~", (name));            \
        timer.start();                              \
        {s_code;}                                   \
        s_cost = timer.get_ns();                    \
        printf(" ttl ~");                           \
        timer.start();                              \
        {t_code;}                                   \
        t_cost = timer.get_ns();                    \
        report(t_cost, s_cost);                     \
    }while(false)                                   \


    void report(time_type t_cost, time_type s_cost) {
        printf(" , %.2lf/%.2lf ms , %s\n",
               double(t_cost) / 1e6, double(s_cost) / 1e6, t_cost > s_cost ? "win" : "lose");
    }

    int randInt(int l, int r) {
        static auto maker = std::mt19937(clock());
        return int(maker()) % (r - l) + l;
    }

    int randInt(int up) { return randInt(0, up); }

    int randInt() { return randInt(0, RAND_MAX); }

    std::string randStr(int len, const char *base = nullptr) {
        std::string ret;
        for (int i = 0; i < len; ++i) ret.push_back(base ? base[i] : char('a' + randInt(0, 26)));
        return ret;
    }

    template<typename T1, typename T2>
    static void same(const T1 &sa, const T2 &ta) {
        assert(ttl::equal(sa.begin(), sa.end(), ta.begin(), ta.end()));
    }

    template<typename T, typename Callable>
    std::vector<T> randArray(int len, Callable call) {
        std::vector<T> ret;
        ret.reserve(len);
        while (len--) ret.template emplace_back(call());
        return ret;
    }
}

#endif //TINYSTL_TEST_HELPER_H
