//
// Created by IMEI on 2022/7/6.
//

#ifndef TINYSTL_SEGMENT_TREE_TEST_H
#define TINYSTL_SEGMENT_TREE_TEST_H

#include "../core/container/expand/segment_tree.h"
#include "../utils/profiler.h"
#include "../utils/test_helper.h"


namespace ttl::ttl_test {

    class segment_tree_test {
    public:
        static void runAll() {
            test1();
        }

    private:
        using Container = std::vector<int>;

        static void vec_add(Container &arr, int l, int r, int val) {
            for (int i = l; i <= r; ++i) {
                arr[i] += val;
            }
        }

        static void vec_mut(Container &arr, int l, int r, int val) {
            for (int i = l; i <= r; ++i) {
                arr[i] *= val;
            }
        }

        static void vec_set(Container &arr, int l, int r, int val) {
            for (int i = l; i <= r; ++i) {
                arr[i] = val;
            }
        }

    private:
        static void test1() {
            const int n = 1000, m = 1000;
            Container arr1(n, 0);// = randIntArray(n);
            Container arr2 = arr1;

            segment_tree<int> tree(arr1);

            auto ops = randIntArray(m);
            auto ls = randIntArray(m);
            auto rs = randIntArray(m);
            auto vs = randIntArray(m);
            for (auto &v: ops) v %= 3;//= randInt({0, 1, 2});
            for (auto &v: ls) v %= n;
            for (auto &v: rs) v %= n;
            //for (auto &v: vs) v = v % 3 + 1;
            for (int i = 0; i < m; ++i) if (ls[i] > rs[i]) std::swap(ls[i], rs[i]);

            for (int i = 0; i < m; ++i) {
                auto op = ops[i], l = ls[i], r = rs[i], val = vs[i];
                if (op == 0) {
                    tree.add(l, r, val);
                    vec_add(arr2, l, r, val);
                } else if (op == 1) {
                    tree.mut(l, r, val);
                    vec_mut(arr2, l, r, val);
                } else if (op == 2) {
                    tree.set(l, r, val);
                    vec_set(arr2, l, r, val);
                }

//                std::cout << "tree : ";
//                for (int j = 0; j < n; ++j) {
//                    std::cout << tree.get(j) << ' ';
//                }
//                std::cout << '\n';
//                std::cout << "arr  : ";
//                for (int j = 0; j < n; ++j) {
//                    std::cout << arr2[j] << ' ';
//                }
//                std::cout << '\n';

                int cnt = 0;
                for (int j = 0; j < n; ++j) {
                    cnt += (tree[j] == arr2[j]);
                }
                if (i % 100 == 0)
                    std::cout << cnt << '/' << n << '\n';
                assert(cnt == n);
            }

            for (auto v: tree) std::cout << v << ' ';
        }
    };

}

#endif //TINYSTL_SEGMENT_TREE_TEST_H
