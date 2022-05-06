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
     * 最大堆是拥有下列属性的元素范围 [f,l) :
     * 设 N = l - f ,对于所有 0 < i < N , comp(f[parent_index(i)],f[i]) == false
     * 且对任意0 < i < N , comp(f[0], f[i])为false
     * Comp默认为std::less , 即大顶堆
     */
    namespace {
        using heap_index = size_t;

        // i>0
        heap_index parent_index(heap_index pos) {
            return (pos - 1) >> 1;
        }

        heap_index left_index(heap_index pos) {
            return (pos << 1) | 1;
        }

        heap_index right_index(heap_index pos) {
            return (pos + 1) << 1;
        }

        // 上浮最后一个元素
        template<typename RandomIt, typename Compare>
        void swim_heap(RandomIt first, heap_index pos, Compare comp) {
            heap_index parent = parent_index(pos);
            while (pos && comp(first[parent], first[pos])) {
                std::swap(first[parent], first[pos]);
                pos = parent, parent = parent_index(pos);
            }
        }

        // 从left,right,parent中选取一个最大的下标
        template<typename RandomIt, typename Compare>
        heap_index sink_index_selector(RandomIt first, heap_index parent, heap_index heap_size, Compare comp) {
            heap_index ret = parent, l = left_index(parent), r = right_index(parent);
            if (l < heap_size && comp(first[ret], first[l])) ret = l;
            if (r < heap_size && comp(first[ret], first[r])) ret = r;
            return ret;
        }

        // 下沉第一个元素
        template<typename RandomIt, typename Compare>
        void sink_heap(RandomIt first, heap_index parent, heap_index heap_size, Compare comp) {
            heap_index idx;
            while ((idx = sink_index_selector(first, parent, heap_size, comp)) != parent) {
                std::swap(first[parent], first[idx]), parent = idx;
            }
        }
    }

    // 检验范围 [first, last) 并寻找始于 first 且为最大堆的最大范围
    template<typename RandomIt, typename Compare>
    RandomIt is_heap_until(RandomIt first, RandomIt last, Compare comp) {
        if (first == last) return last;
        RandomIt pre = first, cur = ++first;
        bool add_pre = false;
        while (cur != last) {
            if (comp(*pre, *cur++))break;
            if (add_pre) ++pre, add_pre = false;
            else add_pre = true;
        }
        return cur;
    }

    template<typename RandomIt>
    RandomIt is_heap_until(RandomIt first, RandomIt last) {
        return ttl::is_heap_until(first, last, std::less<>());
    }

    // 检测是否为堆
    template<typename RandomIt, typename Compare>
    bool is_heap(RandomIt first, RandomIt last, Compare comp) {
        return is_heap_until(first, last, comp) == last;
    }

    template<typename RandomIt>
    bool is_heap(RandomIt first, RandomIt last) {
        return ttl::is_heap(first, last, std::less<>());
    }

    // 将last-1位置处的元素插入到[first,last-1)的堆中
    template<typename RandomIt, typename Compare>
    void push_heap(RandomIt first, RandomIt last, Compare comp) {
        if (last <= first + 1) return;
        heap_index pos = last - first - 1;
        swim_heap(first, pos, comp);
    }

    template<typename RandomIt>
    void push_heap(RandomIt first, RandomIt last) {
        return ttl::push_heap(first, last, std::less<>());
    }

    // 将first位置处的元素从[first,last)的堆中移除,放置到last-1处
    template<typename RandomIt, typename Compare>
    void pop_heap(RandomIt first, RandomIt last, Compare comp) {
        if (last <= first + 1) return;
        heap_index heap_size = last - first - 1;
        std::swap(*first, *(first + heap_size));
        sink_heap(first, 0, heap_size, comp);
    }

    template<typename RandomIt>
    void pop_heap(RandomIt first, RandomIt last) {
        return ttl::pop_heap(first, last, std::less<>());
    }

    // 将[first,last)的中的元素整理为一个堆
    template<typename RandomIt, typename Compare>
    void make_heap(RandomIt first, RandomIt last, Compare comp) {
        if (last <= first + 1) return;
        heap_index heap_size = last - first, parent = heap_size / 2; // 2*p+1<len
        while (true) {
            sink_heap(first, parent, heap_size, comp);
            if (parent-- == 0) break;
        }
    }

    template<typename RandomIt>
    void make_heap(RandomIt first, RandomIt last) {
        return ttl::make_heap(first, last, std::less<>());
    }

    // 将[first,last)的堆排序为升序序列
    template<typename RandomIt, typename Compare>
    void sort_heap(RandomIt first, RandomIt last, Compare comp) {
        while (last > first + 1) ttl::pop_heap(first, last--, comp);
    }

    template<typename RandomIt>
    void sort_heap(RandomIt first, RandomIt last) {
        return ttl::sort_heap(first, last, std::less<>());
    }


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

    // 元素相等
    template<class InputIt1, class InputIt2>
    bool equal(InputIt1 first1, InputIt1 last1,
               InputIt2 first2, InputIt2 last2) {
        for (; first1 != last1, first2 != last2; ++first1, ++first2) {
            if (!(*first1 == *first2)) {
                return false;
            }
        }
        return first1 == last1 && first2 == last2;
    }

    template<class InputIt1, class InputIt2>
    bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
        for (; first1 != last1; ++first1, ++first2) {
            if (!(*first1 == *first2)) {
                return false;
            }
        }
        return true;
    }

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


    /*
     * 位操作
     */
    namespace {
        // Todo 注释
        int popcount32(uint32_t x) noexcept {
            x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
            x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
            x = (x & 0x0f0f0f0f) + ((x >> 4) & 0x0f0f0f0f);
            x = (x & 0x00ff00ff) + ((x >> 8) & 0x00ff00ff);
            x = (x & 0x0000ffff) + ((x >> 16) & 0x0000ffff);
            return int(x);
        }

        // Todo
        int popcount8(uint8_t x) noexcept {
            return popcount32(x);
        }

        // Todo
        int popcount16(uint16_t x) noexcept {
            return popcount32(x);
        }

        // Todo
        int popcount64(uint64_t x) noexcept {
            return popcount32(x & 0xffffffff) + popcount32(x >> 32);
        }
    }

    template<typename T>
    int popcount(T x) noexcept {
        const int digit = std::numeric_limits<T>::digits;
        static_assert(digit <= 64);
        if constexpr(digit <= 8) return popcount8(digit);
        else if constexpr(digit <= 16) return popcount16(digit);
        else if constexpr(digit <= 32) return popcount32(digit);
        else return popcount64(digit);
    }
}

#endif //TINYSTL_ALGORITHM_H
