//
// Created by IMEI on 2022/5/2.
//

#ifndef TINYSTL_HASHTABLE_TEST_H
#define TINYSTL_HASHTABLE_TEST_H

#include "../container/private/hashtable.h"
#include "../utils/profiler.h"
#include "../utils/test_helper.h"
#include <unordered_map>

namespace ttl::ttl_test {

    class hashtable_test {
        template<typename T1, typename T2>
        static void hmp_same(
                ttl::hashtable<T1, T2> &tm,
                std::unordered_multimap<T1, T2> &sm) {
            std::unordered_multimap<T1, T2> tmp;
            for (auto &kv: tm) tmp.insert(kv);
            assert(tmp == sm);
        }

    public:
        static void runAll() {
            test1();
            test2();
            test3();
            test4();
            test5();
            test6();
        }

    private:
        static void test1() { // unique add
            ttl::hashtable<int, int> tm;
            std::unordered_multimap<int, int> sm;
            std::vector<int> rd = randIntArray(100000);
            TTL_STL_COMPARE_2(
                    {
                        for (auto x: rd)tm.emplace_unique(x, x);
                    }, {
                        for (auto x: rd)if (!sm.count(x))sm.insert({x, x});
                    }, "hashtable unique add"
            );
            hmp_same(tm, sm);
        }

        static void test2() { // equal add
            ttl::hashtable<int, int> tm;
            std::unordered_multimap<int, int> sm;
            std::vector<int> rd = randIntArray(100000);
            TTL_STL_COMPARE_2(
                    {
                        for (auto x: rd)tm.emplace_equal(x, x);
                    }, {
                        for (auto x: rd)sm.insert({x, x});
                    }, "hashtable equal add"
            );
            hmp_same(tm, sm);
        }

        static void test3() { // erase
            ttl::hashtable<int, int> tm;
            std::unordered_multimap<int, int> sm;
            std::vector<int> rd = randIntArray(100000);
            std::vector<int> rde = randIntArray(50000);
            for (auto x: rd)tm.emplace_equal(x, x);
            for (auto x: rd)sm.insert({x, x});
            TTL_STL_COMPARE_2(
                    {
                        for (auto x: rde)tm.erase(x);
                    }, {
                        for (auto x: rde)sm.erase(x);
                    }, "hashtable erase key"
            );
            hmp_same(tm, sm);
        }

        static void test4() { // erase == count
            ttl::hashtable<int, int> tm;
            std::unordered_multimap<int, int> sm;
            std::vector<int> rd = randIntArray(100000);
            std::vector<int> rde = randIntArray(50000);
            std::vector<int> r1, r2;
            for (auto x: rd)tm.emplace_equal(x, x);
            for (auto x: rd)sm.insert({x, x});
            TTL_STL_COMPARE_2(
                    {
                        for (auto x: rde)r1.push_back(tm.erase(x));
                    }, {
                        for (auto x: rde)r2.push_back(sm.erase(x));
                    }, "hashtable erase & count"
            );
            hmp_same(tm, sm);
        }

        static void test5() {
            ttl::hashtable<std::string, std::string> tm;
            std::unordered_multimap<std::string, std::string> sm;
            auto rd = randStrArray(100000, 10);
            TTL_STL_COMPARE_2(
                    {
                        for (auto x: rd)tm.emplace_unique(x, x);
                    }, {
                        for (auto x: rd)if (!sm.count(x))sm.insert({x, x});
                    }, "hashtable string unique add"
            );
            hmp_same(tm, sm);
        }

        static void test6() {
            ttl::hashtable<std::string, std::string> tm;
            std::unordered_multimap<std::string, std::string> sm;
            auto rd = randStrArray(100000, 10);
            TTL_STL_COMPARE_2(
                    {
                        for (auto x: rd)tm.emplace_equal(x, x);
                    }, {
                        for (auto x: rd)sm.insert({x, x});
                    }, "hashtable string equal add"
            );
            hmp_same(tm, sm);
        }
    };
}

#endif //TINYSTL_HASHTABLE_TEST_H
