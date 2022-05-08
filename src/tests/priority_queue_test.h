//
// Created by IMEI on 2022/5/5.
//

#ifndef TINYSTL_PRIORITY_QUEUE_TEST_H
#define TINYSTL_PRIORITY_QUEUE_TEST_H

#include "../adapter/priority_queue.h"
#include "../utils/profiler.h"
#include "../utils/test_helper.h"
#include <queue>

namespace ttl::ttl_test {

    class priority_queue_test {
        template<typename T>
        static void pq_same(
                ttl::priority_queue<T> tq,
                std::priority_queue<T> sq) {
            std::vector<T> ta, tb;
            while (!tq.empty()) ta.push_back(tq.top()), tq.pop();
            while (!sq.empty()) tb.push_back(sq.top()), sq.pop();
            assert(ta == tb);
        }

    public:
        static void runAll() {
//            int arr[] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
//            ttl::make_heap(arr, std::end(arr) - 1);
//            ttl::push_heap(arr, std::end(arr));
//            ttl::sort_heap(arr, std::end(arr));

            test1();
            test2();
        }

    private:
        static void test1() {
            ttl::priority_queue<int> tq;
            std::priority_queue<int> sq;
            std::vector<int> rd = randIntArray(100000);
            TTL_STL_COMPARE(tq, sq, {
                for (auto x: rd) v.emplace(x);
            }, "pq insert");
            pq_same(tq, sq);
        }

        static void test2() {
            ttl::priority_queue<int> tq;
            std::priority_queue<int> sq;
            std::vector<int> rd = randIntArray(100000);
            TTL_STL_COMPARE(tq, sq, {
                for (auto x: rd) v.emplace(x);
                for (int i = 0; i < 50000; ++i) v.pop();
            }, "pq insert & pop");
            pq_same(tq, sq);
        }
    };

}
#endif //TINYSTL_PRIORITY_QUEUE_TEST_H
