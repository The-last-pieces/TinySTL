//
// Created by IMEI on 2022/4/30.
//

#ifndef TINYSTL_LIST_TEST_H
#define TINYSTL_LIST_TEST_H

#include "../container/list.h"
#include "../utils/profiler.h"
#include "../utils/test_helper.h"
#include <list>
#include <algorithm>
#include <functional>

namespace ttl::ttl_test {
    class list_test {
    public:
        static void runAll() {
            test1();
            test2();
            test3();
            test5();
            test6();
            test7();
            test8();
        }

    private:
        static void test1() {
            auto rd = randStrArray(10000, 10);
            ttl::list<std::string> tl;
            std::list<std::string> sl;
            TTL_STL_COMPARE(tl, sl, {
                v.assign(rd.begin(), rd.end());
                v.insert(ttl::prev(v.end(), 3), 50000, "world");
                v.insert(ttl::prev(v.rend(), 3000).base(), 50000, "asd");
            }, "list insert");
            same(sl, tl);
        }

        static void test2() {
            auto rd = randStrArray(100000, 10);
            auto rdi = randIntArray(1000);
            ttl::list<std::string> tl(rd.begin(), rd.end());
            std::list<std::string> sl(rd.begin(), rd.end());
            TTL_STL_COMPARE(tl, sl, {
                for (auto i: rdi) {
                    i %= v.size() / 5;
                    i *= 2;
                    v.erase(ttl::next(v.begin(), i), ttl::next(v.begin(), i * 2));
                }
            }, "list erase");
            same(sl, tl);
        }

        static void test3() {
            std::list<std::string> sa;
            ttl::list<std::string> ta;
            auto rds = randIntArray(100);
            auto rd = randStrArray(10000, 10);
            TTL_STL_COMPARE(ta, sa, {
                for (auto s: rds) v.resize(s % 100000, rd[s % 10000]);
            }, "list resize");
            same(sa, ta);
        }

        static void test5() {
            std::list<std::string> sa;
            ttl::list<std::string> ta;
            auto rd1 = randStrArray(100000, 10);
            std::sort(rd1.begin(), rd1.end());
            auto rd2 = randStrArray(100000, 10);
            std::sort(rd2.begin(), rd2.end());
            TTL_STL_COMPARE(ta, sa, {
                v.assign(rd1.begin(), rd1.end());
                v.merge({rd2.begin(), rd2.end()});
            }, "list merge");
            same(sa, ta);
        }

        static void test6() {
            auto rd = randStrArray(100000, 10);
            std::list<std::string> sa(rd.begin(), rd.end());
            ttl::list<std::string> ta(rd.begin(), rd.end());
            TTL_STL_COMPARE(ta, sa, {
                for (int i = 0; i < 101; ++i)v.reverse();
            }, "list reverse");
            same(sa, ta);
        }

        static void test7() {
            auto rd = randStrArray(1000000, 10);
            std::list<std::string> sa;
            ttl::list<std::string> ta;
            std::list<std::string> sa2(rd.begin(), rd.end());
            ttl::list<std::string> ta2(rd.begin(), rd.end());
            TTL_STL_COMPARE_2(
                    {
                        ta.splice(ta.end(), ta2, ttl::next(ta2.begin(), 1000000 / 2), ta2.end());
                    },
                    {
                        sa.splice(sa.end(), sa2, ttl::next(sa2.begin(), 1000000 / 2), sa2.end());
                    }, "list splice"
            );
            same(sa, ta);
            same(sa2, ta2);
        }

        static void test8() {
            auto rd = randStrArray(100000, 10);
            std::list<std::string> sa(rd.begin(), rd.end());
            ttl::list<std::string> ta(rd.begin(), rd.end());
            TTL_STL_COMPARE(ta, sa, {
                v.sort();
            }, "list sort");
            same(sa, ta);
        }
    };
}

#endif //TINYSTL_LIST_TEST_H
