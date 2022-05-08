﻿//
// Created by IMEI on 2022/5/6.
//

#ifndef TINYSTL_DEQUE_H
#define TINYSTL_DEQUE_H

#include "../allocator/memory.h"
#include "../iterator/iterator.h"
#include "../algorithm/algorithm.h"

namespace ttl {

    //   using T = int;

    template<typename T>
    class deque {
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
    private:
        using map_pointer = pointer *;
        using alloc_type = ttl::allocator<value_type>;
        using map_alloc_type = ttl::allocator<pointer>;
    private: // helper class
#pragma region

        // 每个缓冲区需要容纳多少个元素
        static constexpr size_type buffer_size = sizeof(T) < 512 ? size_type(512 / sizeof(T)) : size_type(1);
        static constexpr size_type init_map_size = 8; // 最少map节点个数

        template<typename CVT>
        class deque_iterator {
            friend class deque;

        public:
            using iterator_category = ttl::random_access_iterator_tag;
            using value_type = CVT;
            using pointer = CVT *;
            using reference = CVT &;
            using size_type = size_t;
            using difference_type = ptrdiff_t;
        private:
            pointer cur{}; // 当前元素
            pointer first{}; // cur所在缓冲区的头指针
            pointer last{}; // cur所在缓冲区的尾指针
            map_pointer node{}; // cur所在缓冲区
        private:
            // deque_iterator(map_pointer buf, pointer where):cur(where),node(buf),first() {}
        public: // constructor
            deque_iterator() = default;

            deque_iterator(const deque_iterator &) = default;

            template<typename OV>
            deque_iterator(const deque_iterator<OV> oth) noexcept: // NOLINT(google-explicit-constructor)
                    cur(const_cast<pointer>(oth.cur)),
                    first(const_cast<pointer>(oth.first)),
                    last(const_cast<pointer>(oth.last)),
                    node(const_cast<map_pointer>(oth.node)) {}

            ~deque_iterator() = default;

        public: // ops
            reference operator*() const { return *cur; }

            pointer operator->() const { return alloc_type::address(*cur); }

            difference_type operator-(const deque_iterator &x) const {
                return difference_type(buffer_size) * (node - x.node - 1) +
                       (cur - first) + (x.last - x.cur);
            }

            deque_iterator &operator++() {
                ++cur;
                if (cur == last) set_node(node + 1), cur = first;
                return *this;
            }

            deque_iterator operator++(int) {
                return ++deque_iterator(*this);
            }

            deque_iterator &operator--() {
                if (cur == first) set_node(node - 1), cur = last;
                --cur;
                return *this;
            }

            deque_iterator operator--(int) {
                return --deque_iterator(*this);
            }

            deque_iterator &operator+=(difference_type n) {
                difference_type offset = n + (cur - first);
                if (offset >= 0 && offset < difference_type(buffer_size)) {
                    cur += n; // 处于同一缓冲区
                } else {
                    difference_type node_offset =
                            offset > 0 ? offset / difference_type(buffer_size) :
                            -difference_type((-offset - 1) / buffer_size) - 1;
                    set_node(node + node_offset); // 切换到不同缓冲区
                    cur = first + (offset - node_offset * difference_type(buffer_size));
                }
                return *this;
            }

            deque_iterator operator+(difference_type n) const {
                return deque_iterator(*this) += n;
            }

            deque_iterator &operator-=(difference_type n) {
                return (*this) += (-n);
            }

            deque_iterator operator-(difference_type n) const {
                return deque_iterator(*this) += -n;
            }

            reference operator[](size_type n) const { return *(*this + difference_type(n)); }

            bool operator==(const deque_iterator &rhs) const {
                return cur == rhs.cur;
            }

            bool operator!=(const deque_iterator &rhs) const {
                return !(rhs == *this);
            }

            bool operator<(const deque_iterator &rhs) const {
                return (node == rhs.node) ? cur < rhs.cur : node < rhs.node;
            }

            bool operator>(const deque_iterator &rhs) const {
                return rhs < *this;
            }

            bool operator<=(const deque_iterator &rhs) const {
                return !(rhs < *this);
            }

            bool operator>=(const deque_iterator &rhs) const {
                return !(*this < rhs);
            }

        private:
            // 设置新缓冲区
            void set_node(map_pointer new_node) {
                node = new_node;
                first = *new_node;
                last = first + difference_type(buffer_size);
            }
        };

#pragma endregion
    public: // iter
        using iterator = deque_iterator<value_type>;
        using const_iterator = deque_iterator<const value_type>;
        using reverse_iterator = ttl::reverse_iterator<iterator>;
        using const_reverse_iterator = ttl::reverse_iterator<const_iterator>;
    private:
        map_pointer map_buffer{}; // 缓冲区列表
        size_type map_size{}; // buffer_map可用的最大长度
        // begin & end 迭代器 , [ start.node , finish.node ] 为实际使用的node范围
        iterator start, finish;
    public: // constructors
#pragma region

        deque() { fill_initialize(0); }

        explicit deque(size_type count) {
            fill_initialize(count);
        }

        explicit deque(size_type count, const T &value) {
            fill_initialize(count, value);
        }

        // Todo
        template<class InputIt>
        deque(InputIt first, InputIt last) {}

        // Todo
        deque(const deque &other) {}

        // Todo
        deque(deque &&other) noexcept {}

        deque(std::initializer_list<T> init) : deque(init.begin(), init.end()) {}

        ~deque() {
            destroy_all_node();
            dealloc_node(*start.node);
            map_alloc_type::deallocate(map_buffer, map_size);
            map_buffer = nullptr, map_size = 0;
        }

#pragma endregion
    private: // memory Todo
#pragma region

        static pointer alloc_node() {
            return alloc_type::allocate(buffer_size);
        }

        static void dealloc_node(pointer node) {
            alloc_type::deallocate(node, buffer_size);
        }

        // 初始化map结点 , ele_count为预定的元素个数
        void init_map_node_by_count(size_type ele_count) {
            size_type node_count = ele_count / buffer_size + 1;
            map_size = ttl::max(init_map_size, node_count + 2); // 前后加2
            map_buffer = map_alloc_type::allocate(map_size);
            // 使得起始位置位于中间
            map_pointer m_start = map_buffer + (map_size - node_count) / 2;
            map_pointer m_finish = m_start + node_count - 1;
            for (map_pointer cur = m_start; cur <= m_finish; ++cur)
                *cur = alloc_node();
            // 设置迭代器
            start.set_node(m_start);
            start.cur = start.first;
            finish.set_node(m_finish);
            finish.cur = finish.first + ele_count % buffer_size;
        }

        // 重新分配map结点 , 使得能容乃nodes_to_add个新增节点
        void recall_map_node(size_type nodes_to_add, bool add_at_front) {
            size_type old_nodes = finish.node - start.node + 1; // 实际
            size_type new_nodes = old_nodes + nodes_to_add;

            map_pointer new_start;
            if (map_size > 2 * new_nodes) {
                // 剩余缓冲区还很多 , 移动到中间即可 , 注意finish为闭区间
                new_start = map_buffer + (map_size - new_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                if (new_start < start.node) { // 左移
                    ttl::copy(start.node, finish.node + 1, new_start);
                } else { // 右移
                    ttl::copy_backward(start.node, finish.node + 1, new_start + old_nodes);
                }
            } else {
                // 重新分配空间 , 最少一倍 , +2为两端
                size_type new_map_size = map_size + ttl::max(map_size, nodes_to_add) + 2;
                map_pointer new_map = map_alloc_type::allocate(new_map_size);
                new_start = new_map + (new_map_size - new_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                // 拷贝原指针
                ttl::copy(start.node, finish.node + 1, new_start);
                // 释放并重设原map
                map_alloc_type::deallocate(map_buffer, map_size);
                map_buffer = new_map, map_size = new_map_size;
            }
            // 重设迭代器,不负责设置迭代器的指针位置
            start.set_node(new_start);
            finish.set_node(new_start + old_nodes - 1);
        }

        void fill_initialize(size_type count) {
            init_map_node_by_count(count);
            for (map_pointer cur = start.node; cur < finish.node; ++cur) {
                ttl::uninitialized_default_construct_n(*cur, buffer_size);
            }
            ttl::uninitialized_default_construct(finish.first, finish.cur);
        }

        void fill_initialize(size_type count, const value_type &value) {
            init_map_node_by_count(count);
            for (map_pointer cur = start.node; cur < finish.node; ++cur) {
                ttl::uninitialized_fill_n(*cur, buffer_size, value);
            }
            ttl::uninitialized_fill(finish.first, finish.cur, value);
        }

        // 销毁所有结点
        void destroy_all_node() {
            // 不直接ttl::destroy , deque迭代器自增效率较低
            // 处理处头尾的缓冲区
            for (map_pointer node = start.node + 1; node < finish.node; ++node) {
                ttl::destroy(*node, *node + buffer_size);
                dealloc_node(*node);
            }
            // 至少有两个
            if (start.node != finish.node) {
                ttl::destroy(start.cur, start.last);
                ttl::destroy(finish.first, finish.cur);
                dealloc_node(*finish.node);
            } else {
                ttl::destroy(start.cur, finish.cur); // 头结点需要保留
            }
            finish = start;
        }
        // 回收所有map_pointer
#pragma endregion
    public: // assign Todo
#pragma region

        deque &operator=(const deque &other) {
            return *this;
        }

        deque &operator=(deque &&other) noexcept {
            return *this;
        }

        deque &operator=(std::initializer_list<T> init) {
            return *this;
        }

        void assign(size_type count, const T &value) {}

        template<class InputIt>
        void assign(InputIt first, InputIt last) {}

        void assign(std::initializer_list<T> init) {}

#pragma endregion
    public: // visit
#pragma region

        reference at(size_type pos) { return start[pos]; }

        const_reference at(size_type pos) const { return start[pos]; }

        reference operator[](size_type pos) { return start[pos]; }

        const_reference operator[](size_type pos) const { return start[pos]; }

        reference front() { return *start; }

        const_reference front() const { return *start; }

        reference back() { return *--iterator(finish); }

        const_reference back() const { return *--iterator(finish); }

#pragma endregion
    public: // iterators
#pragma region

        iterator begin() const { return start; }

        iterator end() const { return finish; }

        const_iterator cbegin() const { return start; }

        const_iterator cend() const { return finish; }

        reverse_iterator rbegin() const { return reverse_iterator(end()); }

        reverse_iterator rend() const { return reverse_iterator(begin()); }

        const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }

        const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

#pragma endregion
    public: // capacity
#pragma region

        bool empty() const { return finish == start; }

        size_type size() const { return finish - start; }

        static size_type max_size() {
            return alloc_type::max_size();
        }

        // Todo
        void shrink_to_fit() {}

#pragma endregion
    public: // change Todo
#pragma region

        void clear() { destroy_all_node(); }

        void insert() {}

        void emplace() {}

        void erase() {}

        void push_back() {}

        template<typename ...Args>
        void emplace_back(Args &&...args) {
            prepare_at_back(1);
            alloc_type::construct((finish - 1).cur, std::forward<Args>(args)...);
        }

        void pop_back() {}

        void push_front() {}

        template<typename ...Args>
        void emplace_front(Args &&...args) {
            prepare_at_front(1);
            alloc_type::construct(start.cur, std::forward<Args>(args)...);
        }

        void pop_front() {}

        void resize() {}

        void swap() {}

#pragma endregion
#pragma region
    private: // helper
        // front处有几个空余元素位置
        size_type front_leave() {
            return difference_type(buffer_size) * (start.node - map_buffer) +
                   (start.cur - start.first);
        }

        // back处有几个空余元素位置
        size_type back_leave() {
            return difference_type(buffer_size) * (map_buffer + (map_size - 1) - finish.node) +
                   (finish.last - finish.cur);
        }

        // Todo 改为处于未初始化状态
        // 腾出足够的空间 , 使得[start+index, start+index+count)处于默认构造状态
        // 返回true表示左移(push_front) , 否则为右移(push_back)
        // 功能更多 , 复杂度更高 , 会移动对对象 & 更新迭代器
        // index = 0时等价于prepare_at_front
        // index = size()时等价于prepare_at_back
        bool prepare_at(size_type index, size_type count) {
            size_type sz = size();
            // index插入点之前的元素个数
            if (index < sz / 2) {
                // 左移 front
                prepare_at_front(count);
                // [start + count ,) => [start ,) for index
                ttl::move_n(start + difference_type(count), index, start);
                return true;
            } else {
                // 右移 back
                prepare_at_back(count);
                // [, finish - count) => [, finish) for size() - index
                ttl::move_n_backward(finish - difference_type(count), sz - index, finish);
                return false;
            }
        }

        // 在front处腾出足够位置 , 并更新头迭代器
        void prepare_at_front(size_type count) {
            size_type leave = front_leave();
            if (leave < count) {
                // 空余不够 , 重新分配
                recall_map_node((count - leave) / buffer_size + 1, true);
            }
            size_type node_leave = start.cur - start.first;
            if (node_leave < count) {
                // 当前缓冲区不够 , 申请缓冲区
                size_type nodes = (count - node_leave) / buffer_size + 1;
                map_pointer cur = start.node - 1;
                while (nodes--) *cur-- = alloc_node();
            }
            start -= difference_type(count);
            uninitialized_default_construct_n(start, count);
        }

        // 在front处腾出足够位置 , 并更新尾迭代器
        void prepare_at_back(size_type count) {
            size_type leave = back_leave();
            if (leave <= count) {
                // 空余不够 , 重新分配map
                recall_map_node((count - leave) / buffer_size + 1, false);
            }
            size_type node_leave = finish.last - finish.cur;
            if (node_leave <= count) {
                // 当前缓冲区不够 , 申请缓冲区
                size_type nodes = (count - node_leave) / buffer_size + 1;
                map_pointer cur = finish.node + 1;
                while (nodes--) *cur++ = alloc_node();
            }
            uninitialized_default_construct_n(finish, count);
            finish += difference_type(count);
        }

#pragma endregion
    public: // operators Todo
#pragma region
#pragma endregion
    private: // helper Todo
#pragma region
#pragma endregion
    };
}

#endif //TINYSTL_DEQUE_H
