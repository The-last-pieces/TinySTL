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
            auto_timer timer("hashtable unique add");
            ttl::hashtable<int, int> tm;
            for (int i = 0; i < 10000; ++i) tm.emplace_unique(i % 100, i);
            assert(tm.size() == 100);
        }

        static void test2() { // equal add
            auto_timer timer("hashtable equal add");
            ttl::hashtable<int, int> tm;
            for (int i = 0; i < 10000; ++i) tm.emplace_equal(randInt(100), i);
            assert(tm.size() == 10000);
        }

        static void test3() { // erase
            auto_timer timer("hashtable erase key");
            ttl::hashtable<int, int> tm;
            for (int i = 0; i < 10000; ++i) tm.emplace_equal(randInt(100), i);
            size_t sub = 0;
            for (int i = 0; i < 100; ++i) sub += tm.erase(randInt(100));
            assert(tm.size() + sub == 10000);
        }

        static void test4() { // erase == count
            auto_timer timer("hashtable erase & count");
            ttl::hashtable<int, int> tm;
            for (int i = 0; i < 10000; ++i) tm.emplace_equal(randInt(100), i);
            for (int i = 0; i < 100; ++i) {
                int rd = randInt(100);
                size_t cnt = tm.count(rd);
                assert(cnt == tm.erase(rd));
            }
        }

        static void test5() {
            auto_timer timer("hashtable string equal add");
            ttl::hashtable<std::string, std::string> tm;
            for (int i = 0; i < 10000; ++i) tm.emplace_equal(randStr(26), randStr(26));
            assert(tm.size() == 10000);
        }

        static void test6() {
            ttl::hashtable<int, int> tm;
            std::unordered_multimap<int, int> sm;
            std::vector<int> rd = randArray<int>(10000, []() -> int { return randInt(); });
            TTL_STL_COMPARE_2(
                    {
                        for (auto v: rd) tm.emplace_equal(v, v);
                    },
                    {
                        for (auto v: rd) sm.emplace(v, v);
                    }, "hashtable equal push"
            );
            assert(tm.size() == sm.size());
        }
    };
}

#endif //TINYSTL_HASHTABLE_TEST_H
