//
// Created by IMEI on 2022/4/28.
//

#ifndef TINYSTL_VECTOR_TEST_H
#define TINYSTL_VECTOR_TEST_H

#include "../container/vector.h"
#include "../utils/profiler.h"
#include <vector>
#include <cassert>

namespace ttl::ttl_test {
    class vector_test {
    public:
        static void runAll() {
            test1();
        }

    private:
        static void test1() {
            std::vector<std::string> arr;
            std::vector<std::string> sa;
            ttl::vector<std::string> ta;
            for (int i = 0; i < 100000; ++i) {
                std::string temp;
                for (int j = 0; j < 10; ++j)temp.push_back(rand() % 26 + 'a');
                arr.push_back(temp);
            }
            {
                auto_timer timer("stl vector");
                for (auto &temp: arr) sa.push_back(temp);
            }
            {
                auto_timer timer("ttl vector");
                for (auto &temp: arr) ta.push_back(temp);
            }
            assert(std::equal(
                    sa.begin().base(), sa.end().base(),
                    ta.begin().base(), ta.end().base()));
        }
    };
}

#endif //TINYSTL_VECTOR_TEST_H
