//
// Created by IMEI on 2022/5/6.
//

#ifndef TINYSTL_BITSET_TEST_H
#define TINYSTL_BITSET_TEST_H

#include "../container/expand/bitset.h"
#include "../utils/profiler.h"
#include "../utils/test_helper.h"
#include <bitset>

namespace ttl::ttl_test {

    class bitset_test {
        template<size_t N>
        static void bs_same(const ttl::bitset<N> &tm, const std::bitset<N> &sm) {
            for (int i = 0; i < N; ++i) {
                if (tm.test(i) != sm.test(i)) assert(false);
            }
            assert(tm.size() == sm.size());
            assert(tm.all() == sm.all());
            assert(tm.any() == sm.any());
            assert(tm.to_string() == sm.to_string());
            assert(tm.count() == sm.count());
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
        static const int N = 10000;

        static void test1() {
            ttl::bitset<N> tb;
            std::bitset<N> sb;
            auto rd = randIntArray(10000000);
            TTL_STL_COMPARE(tb, sb, {
                for (auto x: rd) {
                    v.set(x % N, x % 2);
                    //if (x % 3000 == 0) v.flip();
                }
            }, "bitset write");
            bs_same(tb, sb);
        }

        static void test2() {
            ttl::bitset<N> tb;
            std::bitset<N> sb;
            auto rd = randIntArray(1000000);
            for (auto x: rd) tb.set(x % N, x % 2);
            for (auto x: rd) sb.set(x % N, x % 2);
            TTL_STL_COMPARE(tb, sb, {
                for (auto x: rd) v.test(x % N);
            }, "bitset read");
            bs_same(tb, sb);
        }

        static void test3() {
            ttl::bitset<N> tb;
            std::bitset<N> sb;
            auto rd = randIntArray(1000000);
            for (auto x: rd) tb.set(x % N, x % 2);
            for (auto x: rd) sb.set(x % N, x % 2);
            TTL_STL_COMPARE(tb, sb, {
                for (auto x: rd) {
                    if (x % 3000 == 0) v.flip();
                    v.flip(x % N);
                }
            }, "bitset flip");
            bs_same(tb, sb);
        }

        static void test4() {
            ttl::bitset<N> tb;
            std::bitset<N> sb;
            auto rd = randIntArray(1000000);
            TTL_STL_COMPARE(tb, sb, {
                for (auto x: rd) {
                    v.set(x % N, x % 2);
                    if (x % 300 == 0) v.count();
                }
            }, "bitset count");
            bs_same(tb, sb);
        }

        static void test5() {
            ttl::bitset<N> tb;
            std::bitset<N> sb;
            auto rd = randIntArray(1000000);
            TTL_STL_COMPARE(tb, sb, {
                for (auto x: rd) {
                    v[x % N] = x % 2;
                }
            }, "bitset index visit");
            bs_same(tb, sb);
        }

        static void test6() {
            ttl::bitset<N> tb;
            std::bitset<N> sb;
            auto rd = randIntArray(10000);
            TTL_STL_COMPARE(tb, sb, {
                for (auto x: rd) {
                    v.set(x % N, x % 2);
                    if (x % 123) v <<= (x % 321);
                    if (x % 321) v >>= (x % 123);
                }
            }, "bitset offset");
            bs_same(tb, sb);
        }
    };

}

#endif //TINYSTL_BITSET_TEST_H
