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
            test1();
            test2();
            test3();
            test4();
        }

    private:
        static void test1() {
            std::vector<std::string> arr;
            for (int i = 0; i < 10000; ++i) arr.push_back(randStr(26));
            ttl::vector<std::string> ta;
            std::vector<std::string> sa;
            TTL_STL_COMPARE(ta, sa, {
                for (auto &temp: arr) v.push_back(temp);
            }, "vector push");
            same(sa, ta);
        }

        static void test2() {
            std::vector<int> sa;
            ttl::vector<int> ta;
            TTL_STL_COMPARE(ta, sa, {
                v.insert(v.end(), {1, 2, 3});
                v.insert(v.end(), 100u, 4);
                v.insert(v.end() - 50, 10000u, 5);
                v.insert(v.end(), 100u, 6);
                v.insert(v.end() - 50, 1000u, 7);
                // v.erase(v.end() - 2);
                //v.erase(v.begin() + 1123, v.end() - 3456);
            }, "vector pod insert & erase");
            same(sa, ta);
        }

        static void test3() {
            std::vector<std::string> sa;
            ttl::vector<std::string> ta;
            TTL_STL_COMPARE(ta, sa, {
                v.insert(v.end(), {"asd", "hod", "qwe"});
                v.resize(100);
                v.insert(v.end(), 100, "hello");
                v.pop_back();
                v.emplace_back("asd");
                v.insert(v.end() - 50, 1000, "???");
                v.pop_back();
                v.resize(10000, "hhhh");
                v.pop_back();
                v.push_back("www");
                v.insert(v.end(), 10000, "world");
                v.emplace_back("www");
                v.insert(v.end() - 5000, 12030, "!!!");
                v.emplace_back("zxd");
                v.resize(10000);
                v.erase(v.end() - 122);
                v.erase(v.begin() + 1123, v.end() - 3456);
            }, "vector class insert & erase");
            same(sa, ta);
        }

        static void test4() {
            std::vector<std::string> sa;
            ttl::vector<std::string> ta;
            TTL_STL_COMPARE(ta, sa, {
                v.operator=({"1", "2"});
                v.assign({"asd", "bds"});
                v.assign(10000, "1234");
                v.insert((v.rbegin() + 1020).base(), 123, "asd");
            }, "vector assign");
            same(sa, ta);
        }
    };
}

#endif //TINYSTL_VECTOR_TEST_H
