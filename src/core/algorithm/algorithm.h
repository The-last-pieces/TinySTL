//
// Created by IMEI on 2022/4/27.
//

#ifndef TINYSTL_ALGORITHM_H
#define TINYSTL_ALGORITHM_H

namespace ttl {

    /*
     * 不修改序列的操作
     */

    /*
     * 修改序列的操作
     */
    template<typename ForwardIt, typename OutputIt>
    OutputIt copy(ForwardIt first, ForwardIt last, OutputIt result) {
        while (first != last) {
            *result++ = *first++;
        }
        return result;
    }

    template<typename ForwardIt, typename OutputIt>
    OutputIt copy_n(ForwardIt first, size_t n, OutputIt result) {
        while (n--) {
            *result++ = *first++;
        }
        return result;
    }

    /*
     * 划分操作
     */

    /*
     * 排序操作
     */

    /*
     * 二分搜索操作(需要已排序)
     */

    /*
     * 集合操作(需要已排序)
     */

    /*
     * 其他已排序范围上的操作
     */

    /*
     * 堆操作
     */

    /*
     * 最小/最大操作
     */
    template<typename T>
    T min(const T &a, const T &b) {
        return a < b ? a : b;
    }

    template<typename T>
    T max(const T &a, const T &b) {
        return a < b ? b : a;
    }

    /*
     * 比较操作
     */
    // 字典序<
    template<typename InputIt1, typename InputIt2>
    bool lexicographical_compare(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2) {
        for (; (first1 != last1) && (first2 != last2); ++first1, ++first2) {
            if (*first1 < *first2) return true;
            if (*first2 < *first1) return false;
        }
        return (first1 == last1) && (first2 != last2);
    }

    /*
     * 排列操作
     */
}

#endif //TINYSTL_ALGORITHM_H
