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
        printf("%-30s : stl vs", (name));           \
        {                                           \
        auto &v = sv;                               \
        timer.start();                              \
        {code;}                                     \
        s_cost = timer.get_ns();                    \
        }                                           \
        printf(" ttl : ");                          \
        {                                           \
        auto &v = tv;                               \
        timer.start();                              \
        {code;}                                     \
        t_cost = timer.get_ns();                    \
        }                                           \
        report(s_cost, t_cost);                     \
    }while(false)                                   \


#define TTL_STL_COMPARE_2(t_code, s_code, name)     \
    do{                                             \
        free_timer timer;                           \
        time_type s_cost,t_cost;                    \
        printf("%-30s : stl vs", (name));           \
        timer.start();                              \
        {s_code;}                                   \
        s_cost = timer.get_ns();                    \
        printf(" ttl : ");                          \
        timer.start();                              \
        {t_code;}                                   \
        t_cost = timer.get_ns();                    \
        report(s_cost, t_cost);                     \
    }while(false)                                   \


    void report(time_type s_cost, time_type t_cost) {
        static struct recorder {
            int win = 0, lose = 0, same = 0;

            auto record(time_type s, time_type t) {
                if (s == t) {
                    return ++same, "same";
                } else if (s > t) {
                    return ++win, "win";
                } else {
                    return ++lose, "lose";
                }
            }

            ~recorder() {
                printf("total cost : %ds , win : %d, lose : %d , same : %d", clock() / 1000, win, lose, same);
            }
        } recorder;
        auto str = recorder.record(s_cost, t_cost);
        auto rate = double(t_cost) / double(s_cost) * 100;
        if (s_cost < 10000 && t_cost < 10000) {
            printf(" %.2lf/%.2lf \tns , %-6.2lf%% %s\n", double(s_cost), double(t_cost), rate, str);
        } else {
            printf(" %.2lf/%.2lf \tms , %-6.2lf%% %s\n", double(s_cost) / 1e6, double(t_cost) / 1e6, rate, str);
        }
    }

    int randInt(int l, int r) {
        static auto seed = 0;//time(nullptr);
        static auto maker =
                (std::cout << "seed : " << seed << std::endl, std::mt19937(seed)); // NOLINT(cert-msc51-cpp)
        return int(maker() % (r - l)) + l;
    }

    int randInt(int up) { return randInt(0, up); }

    int randInt() { return randInt(0, RAND_MAX); }

    std::string randStr(int len, const char *base = nullptr) {
        std::string ret;
        for (int i = 0; i < len; ++i) ret.push_back(base ? base[i] : char('a' + randInt(0, 26)));
        return ret;
    }

    template<typename T1, typename T2>
    void same(const T1 &sa, const T2 &ta) {
        assert(ttl::equal(sa.begin(), sa.end(), ta.begin(), ta.end()));
    }

    template<typename T, typename Callable>
    std::vector<T> randArray(int len, Callable call) {
        std::vector<T> ret;
        ret.reserve(len);
        while (len--) ret.emplace_back(call());
        return ret;
    }

    std::vector<int> randIntArray(int len) {
        return randArray<int>(len, []() -> int { return randInt(); });
    }

    std::vector<std::string> randStrArray(int len, int sz) {
        return randArray<std::string>(len, [=]() -> std::string { return randStr(sz); });
    }
}

#endif //TINYSTL_TEST_HELPER_H
