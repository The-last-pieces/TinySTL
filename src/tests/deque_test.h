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
            test2();
            test3();
            test4();
            test5();
        }

    private:
        // 构造函数
        static void test1() {
            using T = std::string;
            same(ttl::deque<T>(), std::deque<T>());
            auto sz = 10000;
            same(ttl::deque<T>(sz), std::deque<T>(sz));
            T val = "123123123131";
            same(ttl::deque<T>(sz, val), std::deque<T>(sz, val));
            auto rd = randStrArray(sz, 10);
            ttl::deque<T> td(rd.begin(), rd.end());
            std::deque<T> sd(rd.begin(), rd.end());
            same(td, sd);
            auto tmp1 = td;
            auto tmp2 = sd;
            same(tmp1, tmp2);
            auto tmp3 = std::move(td);
            auto tmp4 = std::move(sd);
            same(tmp3, tmp4);
            same(td, sd); // NOLINT(bugprone-use-after-move)
            std::initializer_list<T> init = {"a", "b", "c"};
            same(ttl::deque<T>(init), std::deque<T>(init));
        }

        // 赋值函数
        static void test2() {
            using T = std::string;
            const int sz = 10000;
            ttl::deque<T> td;
            std::deque<T> sd;
            auto rd = randStrArray(sz, 10);
            ttl::deque<T> td1(rd.begin(), rd.end());
            std::deque<T> sd1(rd.begin(), rd.end());
            td = td1;
            sd = sd1;
            same(td, sd);
            td = std::move(td1);
            sd = std::move(sd1);
            same(td, sd);
            td = {"a", "b", "c", "d"};
            sd = {"a", "b", "c", "d"};
            same(td, sd);
            td.assign(sz, "hello");
            sd.assign(sz, "hello");
            same(td, sd);
            td.assign(rd.begin(), rd.end());
            sd.assign(rd.begin(), rd.end());
            same(td, sd);
            td.assign({"a", "b", "c", "d"});
            sd.assign({"a", "b", "c", "d"});
            same(td, sd);
        }

        // 插入
        static void test3() {
            using T = std::string;
            const int n = 50000;
            std::deque<T> sd;
            ttl::deque<T> td;
            auto rd = randStrArray(n, 10);
            TTL_STL_COMPARE(td, sd, {
                int j = 0;
                v.insert(v.end(), "12345");
                v.insert(v.end(), 12345u, "4321");
                // Todo fix 注释中的代码取消后会有内存错误
                // v.insert(v.begin() + 5005, rd.begin(), rd.end());
                v.insert(v.begin() + 1235, {"-3", "4", "-5"});
                for (int i = 1; i <= n; ++i) v.emplace_front(std::to_string(j++));
                for (int i = 1; i <= n; ++i) v.emplace_back(std::to_string(j++));
                for (int i = 1; i <= n; ++i) v.emplace_front(std::to_string(j++));
                //for (int i = 1; i <= n; ++i) v.emplace(v.begin() + i, j++);
            }, "deque push");
            same(sd, td);
        }

        // 删除
        static void test4() {
            using T = std::string;
            const int n = 10000;
            auto rd = randStrArray(n, 10);
            std::deque<T> sd(rd.begin(), rd.end());
            ttl::deque<T> td(rd.begin(), rd.end());
            auto rde = randIntArray(n / 10);
            TTL_STL_COMPARE(td, sd, {
                for (auto i: rde) v.erase(v.begin() + (i % v.size()));
                for (auto i: rde) {
                    auto id = i % v.size();
                    if (i + i < v.size())
                        v.erase(v.begin() + i, v.begin() + i + i);
                }
                for (int i = 0; i < n / 10 && v.size() > 1000; ++i) {
                    v.pop_back();
                    v.pop_front();
                }
            }, "deque erase");
            same(sd, td);
        }

        // resize
        static void test5() {
            using T = std::string; // int;//
            std::deque<T> sd;
            ttl::deque<T> td;
            TTL_STL_COMPARE(td, sd, {
                v.insert(v.end(), 100000, "world");
                v.pop_back();
                v.pop_front();
                v.erase(v.begin() + 10000, v.end() - 10000);
                v.insert(v.end(), 100000, "world");
                v.resize(10000);
                v.resize(1000);
                v.resize(100000, "hello");
            }, "deque resize");
            same(sd, td);
        }
    };

}

#endif //TINYSTL_DEQUE_TEST_H
