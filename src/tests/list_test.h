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
            test4();
            test5();
            test6();
            test7();
            test8();
        }

    private:
        static void test1() {
            auto rd = randArray<std::string>(
                    10000,
                    []() -> std::string { return randStr(10); });
            ttl::list<std::string> tl;
            std::list<std::string> sl;
            TTL_STL_COMPARE(tl, sl, {
                v.assign(rd.begin(), rd.end());
                v.insert(ttl::prev(v.end(), 3), 12420, "world");
                v.emplace_back("hhh");
                v.insert(ttl::prev(v.rend(), 3000).base(), 13211, "asd");
            }, "list insert");
            same(sl, tl);
        }

        static void test2() {
            auto rd = randArray<std::string>(
                    10000,
                    []() -> std::string { return randStr(10); });
            ttl::list<std::string> tl;
            std::list<std::string> sl;
            TTL_STL_COMPARE(tl, sl, {
                v.assign(rd.begin(), rd.end());
                v.erase(ttl::prev(v.rend(), 100).base(), ttl::next(v.begin(), 5000));
                v.erase(ttl::next(v.begin(), 1000), ttl::next(v.begin(), 3000));
            }, "list erase");
            same(sl, tl);
        }

        static void test3() {
            std::list<std::string> sa;
            ttl::list<std::string> ta;
            TTL_STL_COMPARE(ta, sa, {
                v.resize(10000, "12345");
                v.resize(9900);
            }, "list resize 1");
            same(sa, ta);
        }

        static void test4() {
            std::list<std::string> sa;
            ttl::list<std::string> ta;
            TTL_STL_COMPARE(ta, sa, {
                v.resize(9900, "12345");
                v.resize(1);
            }, "list resize 2");
            same(sa, ta);
        }

        static void test5() {
            std::list<int> sa;
            ttl::list<int> ta;
            TTL_STL_COMPARE(ta, sa, {
                v.assign({1, 3, 5, 7, 9});
                v.merge({2, 4, 6, 8, 10});
            }, "list merge 1");
            same(sa, ta);
            TTL_STL_COMPARE(ta, sa, {
                v.assign({1, 2, 3});
                v.merge({4, 5, 6});
            }, "list merge 2");
            same(sa, ta);
        }

        static void test6() {
            auto rd = randArray<std::string>(
                    100000,
                    []() -> std::string { return randStr(10); });
            std::list<std::string> sa(rd.begin(), rd.end());
            ttl::list<std::string> ta(rd.begin(), rd.end());
            TTL_STL_COMPARE(ta, sa, {
                v.reverse();
                v.reverse();
                v.reverse();
            }, "list reverse");
            same(sa, ta);
        }

        static void test7() {
            auto rd = randArray<std::string>(
                    100000,
                    []() -> std::string { return randStr(10); });

            std::list<std::string> sa;
            ttl::list<std::string> ta;
            std::list<std::string> sa2(rd.begin(), rd.end());
            ttl::list<std::string> ta2(rd.begin(), rd.end());
            TTL_STL_COMPARE_2(
                    {
                        ta.splice(ta.end(), ta2, ttl::next(ta2.begin(), 500), ta2.end());
                    },
                    {
                        sa.splice(sa.end(), sa2, ttl::next(sa2.begin(), 500), sa2.end());
                    }, "list splice"
            );
            same(sa, ta);
            same(sa2, ta2);
        }

        static void test8() {
            auto rd = randArray<std::string>(
                    100000,
                    []() -> std::string { return randStr(randInt(10) + 1); });
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
