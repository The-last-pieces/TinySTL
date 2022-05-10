//
// Created by IMEI on 2022/4/28.
//

#ifndef TINYSTL_VECTOR_TEST_H
#define TINYSTL_VECTOR_TEST_H

#include "../container/vector.h"
#include "../utils/profiler.h"
#include "../utils/test_helper.h"
#include <vector>

namespace ttl::ttl_test {
    class vector_test {
    public:
        static void runAll() {
            test0();
            test1();
            test2();
            test3();
            test4();
            test5();
            test6();
            test7();
        }

    private:
        static void test0() {
            auto arr = randIntArray(1000000);
            ttl::vector<int> ta;
            std::vector<int> sa;
            TTL_STL_COMPARE(ta, sa, {
                for (auto &temp: arr) v.push_back(temp);
            }, "vector pod push");
            same(sa, ta);
        }

        static void test1() {
            auto arr = randStrArray(10000, 26);
            ttl::vector<std::string> ta;
            std::vector<std::string> sa;
            TTL_STL_COMPARE(ta, sa, {
                for (auto &temp: arr) v.push_back(temp);
            }, "vector class push");
            same(sa, ta);
        }

        static void test2() {
            auto rd = randIntArray(88888);
            auto rdi = randIntArray(8888);
            auto rdv = randIntArray(88888);
            std::vector<int> sa(rd.begin(), rd.end());
            ttl::vector<int> ta(rd.begin(), rd.end());
            TTL_STL_COMPARE(ta, sa, {
                for (auto i: rdi) v.insert(v.begin() + i % v.size(), rdv[i % rdv.size()]);
            }, "vector pod insert");
            same(sa, ta);
        }

        static void test3() {
            auto rd = randStrArray(8888, 10);
            auto rdi = randIntArray(888);
            auto rdv = randStrArray(88888, 10);
            std::vector<std::string> sa(rd.begin(), rd.end());
            ttl::vector<std::string> ta(rd.begin(), rd.end());
            TTL_STL_COMPARE(ta, sa, {
                for (auto i: rdi) v.insert(v.begin() + i % v.size(), rdv[i % rdv.size()]);
            }, "vector class insert");
            same(sa, ta);
        }

        static void test4() {
            auto rd = randIntArray(100000);
            std::vector<int> sa(rd.begin(), rd.end());
            ttl::vector<int> ta(rd.begin(), rd.end());
            auto rde = randIntArray(10000);
            TTL_STL_COMPARE(ta, sa, {
                for (auto i: rde) v.erase(v.begin() + (i % v.size()));
            }, "vector pod erase");
            same(sa, ta);
        }

        static void test5() {
            auto rd = randStrArray(10000, 26);
            auto rde = randIntArray(1000);
            std::vector<std::string> sa(rd.begin(), rd.end());
            ttl::vector<std::string> ta(rd.begin(), rd.end());
            TTL_STL_COMPARE(ta, sa, {
                for (auto i: rde) v.erase(v.begin() + i % v.size());
            }, "vector class erase");
            same(sa, ta);
        }

        static void test6() {
            std::vector<std::string> sa;
            ttl::vector<std::string> ta;
            auto rdi = randIntArray(1000);
            auto rd = randStrArray(10000, 10);
            TTL_STL_COMPARE(ta, sa, {
                for (auto i: rdi) {
                    i %= rd.size() / 3;
                    v.assign(rd.begin() + i, rd.begin() + i * 3);
                }
            }, "vector assign");
            same(sa, ta);
        }

        static void test7() {
            std::vector<std::string> sa;
            ttl::vector<std::string> ta;
            auto rds = randIntArray(1000);
            auto rd = randStrArray(10000, 10);
            TTL_STL_COMPARE(ta, sa, {
                for (auto s: rds) v.resize(s % 100000, rd[s % 10000]);
            }, "vector resize");
            same(sa, ta);
        }
    };
}

#endif //TINYSTL_VECTOR_TEST_H
