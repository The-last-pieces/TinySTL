//
// Created by IMEI on 2022/4/27.
//

#ifndef TINYSTL_VECTOR_H
#define TINYSTL_VECTOR_H

#include "../allocator/memory.h"
#include "../algorithm/algorithm.h"
#include "../iterator/iterator.h"

namespace ttl {

    // Todo 特化vector bool
    template<typename T> // , typename Alloc = ttl::allocator<T>
    class vector {
    private:
        using alloc_type = ttl::allocator<T>; //typename Alloc::template rebind<T>::other;

        T *start{}; // 起始指针
        T *finish{}; // 逻辑上的结尾
        T *end_of_storage{}; // 内存实际分配的末尾
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
    public: // iter
        using iterator = ttl::normal_iterator<pointer, vector>;
        using const_iterator = ttl::normal_iterator<const_pointer, vector>;
        using reverse_iterator = ttl::reverse_iterator<iterator>;
        using const_reverse_iterator = ttl::reverse_iterator<const_iterator>;
    public: // constructor
#pragma region

        vector() = default;

        explicit vector(size_type n) {
            create_storage(n);
            finish = ttl::uninitialized_default_construct_n(start, n);
        }

        vector(size_type n, const value_type &value) {
            create_storage(n);
            finish = ttl::uninitialized_fill_n(start, n, value);
        }

        vector(const vector &x) {
            create_storage(x.size());
            finish = ttl::uninitialized_copy(x.begin(), x.end(), start);
        }

        vector(vector &&x) noexcept {
            move_storage(x);
        }

        template<typename InputIt>
        vector(InputIt first, InputIt last) {
            create_storage(ttl::distance(first, last));
            finish = ttl::uninitialized_copy(first, last, start);
        }

        vector(std::initializer_list<T> init) {
            create_storage(init.size());
            finish = ttl::uninitialized_copy(init.begin(), init.end(), start);
        }

        ~vector() {
            destroy_storage();
            deallocate_storage();
        }

#pragma endregion
    public: // assign , same logic
#pragma region

        vector &operator=(const vector &x) {
            if (this == &x) return *this;
            assign_aux(x.begin(), x.end(), x.size());
            return *this;
        }

        vector &operator=(vector &&x) noexcept {
            if (this == &x) return *this;
            move_storage(x);
            return *this;
        }

        vector &operator=(std::initializer_list<value_type> x) {
            assign_aux(x.begin(), x.end(), x.size());
            return *this;
        }

        void assign(size_type n, const value_type &value) {
            destroy_storage();
            if (n > capacity()) { // 需要扩容
                deallocate_storage();
                create_storage(n);
            }
            finish = ttl::uninitialized_fill_n(start, n, value);
        }

        template<typename InputIt>
        void assign(InputIt first, InputIt last) {
            assign_aux(first, last, ttl::distance(first, last));
        }

        void assign(std::initializer_list<value_type> x) {
            assign_aux(x.begin(), x.end(), x.size());
        }

#pragma endregion
    public: // visit
#pragma region

        reference at(size_t i) {
            if (i >= size()) throw std::out_of_range("i >= vector size");
            return start[i];
        }

        const_reference at(size_t i) const {
            if (i >= size()) throw std::out_of_range("i >= vector size");
            return start[i];
        }

        reference operator[](size_t i) {
            return start[i];
        }

        const_reference operator[](size_t i) const {
            return start[i];
        }

        reference front() {
            return *start;
        }

        const_reference front() const {
            return *start;
        }

        reference back() {
            return *(finish - 1);
        }

        const_reference back() const {
            return *(finish - 1);
        }

        pointer data() {
            return start;
        }

        pointer data() const {
            return start;
        }

#pragma endregion
    public: // iterators
#pragma region

        iterator begin() const { return iterator(start); }

        iterator end() const { return iterator(finish); }

        const_iterator cbegin() const { return const_iterator(start); }

        const_iterator cend() const { return const_iterator(finish); }

        reverse_iterator rbegin() const { return reverse_iterator(end()); }

        reverse_iterator rend() const { return reverse_iterator(begin()); }

        const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }

        const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

#pragma endregion
    public: // capacity
#pragma region

        bool empty() const { return start == finish; }

        size_type size() const { return finish - start; }

        static size_type max_size() {
            return alloc_type::max_size();
        }

        void reserve(size_type n) {
            if (n > capacity()) {
                recall_capacity(n);
            }
        }

        size_type capacity() const { return end_of_storage - start; }

        void shrink_to_fit() {
            if (finish < end_of_storage) {
                recall_capacity(size());
            }
        }

#pragma endregion
    public: // change
#pragma region

        void clear() { destroy_storage(); }

        // R,M,S , Range , Init
        iterator insert(const_iterator pos, const value_type &value) {
            size_t i = pos - cbegin();
            m_prepare_insert(i, 1);
            alloc_type::construct(start + i, value);
            return begin() + i;
        }

        iterator insert(const_iterator pos, value_type &&value) {
            size_t i = pos - cbegin();
            m_prepare_insert(i, 1);
            alloc_type::construct(start + i, std::move(value));
            return begin() + i;
        }

        iterator insert(const_iterator pos, size_type n, const value_type &value) {
            size_t i = pos - cbegin();
            m_prepare_insert(i, n);
            ttl::uninitialized_fill_n(begin() + i, n, value);
            return begin() + i + n;
        }

        // Todo 特化输入迭代器
        template<typename InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last) {
            size_t i = pos - cbegin(), n = ttl::distance(first, last);
            m_prepare_insert(i, n);
            ttl::uninitialized_copy(first, last, begin() + i);
            return begin() + i + n;
        }

        iterator insert(const_iterator pos, std::initializer_list<value_type> init) {
            size_t i = pos - cbegin();
            m_prepare_insert(i, init.size());
            ttl::uninitialized_copy(init.begin(), init.end(), begin() + i);
            return begin() + i + init.size();
        }

        template<typename ...Args>
        iterator emplace(const_iterator pos, Args &&...args) {
            size_t i = pos - cbegin();
            m_prepare_insert(i, 1);
            alloc_type::construct(start + i, std::forward<Args>(args)...);
            return begin() + i;
        }

        iterator erase(const_iterator pos) {
            return m_erase(begin() + (pos - cbegin()));
        }

        iterator erase(const_iterator first, const_iterator last) {
            iterator beg = begin();
            const_iterator c_beg = cbegin();
            return m_erase(beg + (first - c_beg), beg + (last - c_beg));
        }

        // R,M
        void push_back(const value_type &value) {
            insert(end(), value);
        }

        void push_back(value_type &&value) {
            insert(end(), std::move(value));
        }

        template<typename ...Args>
        reference emplace_back(Args &&...args) {
            return *emplace(end(), std::forward<Args>(args)...);
        }

        void pop_back() {
            alloc_type::destroy(--finish);
        }

        void resize(size_type new_size) {
            size_type cur_size = size();
            if (new_size > cur_size) {
                if (new_size > capacity()) {
                    recall_capacity(new_size);
                }
                finish = ttl::uninitialized_default_construct_n(finish, new_size - cur_size);
            } else {
                ttl::destroy(start + new_size, finish);
                finish = start + new_size;
            }
        }

        void resize(size_type new_size, const value_type &val) {
            size_type cur_size = size();
            if (new_size > cur_size) {
                if (new_size > capacity()) {
                    recall_capacity(new_size);
                }
                finish = ttl::uninitialized_fill_n(finish, new_size - cur_size, val);
            } else {
                ttl::destroy(start + new_size, finish);
                finish = start + new_size;
            }
        }

        void swap(vector &oth) {
            std::swap(start, oth.start);
            std::swap(finish, oth.finish);
            std::swap(end_of_storage, oth.end_of_storage);
        }

#pragma endregion
    public: // compare
#pragma region

        friend bool operator==(const vector &lhs, const vector &rhs) {
            size_t n = lhs.size();
            if (n != rhs.size()) return false;
            if (&lhs == &rhs || lhs.start == rhs.start) return true;
            for (int i = 0; i < n; ++i) if (lhs[i] != rhs[i]) return false;
            return true;
        }

        friend bool operator!=(const vector &lhs, const vector &rhs) {
            return !(lhs == rhs);
        }

        friend bool operator<(const vector &lhs, const vector &rhs) {
            return ttl::lexicographical_compare(
                    lhs.begin(), lhs.end(),
                    rhs.begin(), rhs.end());
        }

        friend bool operator>(const vector &lhs, const vector &rhs) {
            return rhs < lhs;
        }

        friend bool operator<=(const vector &lhs, const vector &rhs) {
            return !(rhs < lhs);
        }

        friend bool operator>=(const vector &lhs, const vector &rhs) {
            return !(lhs < rhs);
        }

#pragma endregion
    private: // helper  Todo 使用memmove优化
#pragma region

        // 移除一个元素
        iterator m_erase(iterator pos) {
            if (pos == end()) return end();
            return m_erase(pos, pos + 1);
        }

        // [first, last)
        iterator m_erase(iterator first, iterator last) {
            if (first >= last) return last;
            if (first == end()) return end();
            // [last, end()) => [first, first + end()-last)
            first = ttl::move(last, end(), first);
            ttl::destroy(first, end());
            finish = first.base();
            return first;
        }

        // 准备新增 , pos ~ [0,size()]
        void m_prepare_insert(size_type pos, size_type n) {
            if (n == 0) return;
            size_t new_size = size() + n;
            if (new_size > capacity()) {
                recall_capacity(ttl::max(new_size, next_size(capacity())));
            }
            // [tail, finish) => [tail+n, finish+n)
            pointer tail = start + pos;
            ttl::uninitialized_default_construct_n(finish, n);
            // 从后往前移动空出n个位置, 并析构原来的元素
            ttl::move_backward(tail, finish, finish + n);
            // 析构剩余元素
            ttl::destroy(tail, tail + n);
            finish += n;
        }

#pragma endregion
    private: // memory
#pragma region

        // 分配内存,但不进行初始化
        void create_storage(size_type n) {
            start = finish = alloc_type::allocate(n);
            end_of_storage = start + n;
        }

        // 从左值窃取已有的内存
        void move_storage(vector &x) {
            ttl::destroy(start, finish);
            alloc_type::deallocate(start, capacity());
            start = x.start;
            finish = x.finish;
            end_of_storage = x.end_of_storage;
            x.start = x.finish = x.end_of_storage = nullptr;
        }

        // 销毁对象,不负责内存回收
        void destroy_storage() {
            ttl::destroy(start, finish);
            finish = start;
        }

        // 回收内存,不负责销毁
        void deallocate_storage() {
            alloc_type::deallocate(start, capacity());
            start = end_of_storage = start = nullptr;
        }

        // 在尾部进行内存的初始化,不检测边界
        template<typename InputIt>
        void un_init_insert_end(InputIt first, InputIt last) {
            finish = ttl::uninitialized_copy(first, last, finish);
        }

        // 销毁旧对象,以新对象初始化
        template<typename InputIt>
        void assign_aux(InputIt first, InputIt last, size_t n) {
            destroy_storage();
            if (n > capacity()) { // 需要扩容
                deallocate_storage();
                create_storage(n);
            }
            un_init_insert_end(first, last);
        }

        // 更换capacity , 不确保new_cap >= size()
        void recall_capacity(size_type new_cap) {
            size_type old_cap = capacity();
            if (new_cap != old_cap) {
                auto new_start = alloc_type::allocate(new_cap);
                auto ne_finish = ttl::uninitialized_move(start, finish, new_start);
                ttl::destroy(start, finish); // Todo 优化
                alloc_type::deallocate(start, old_cap);
                start = new_start, finish = ne_finish;
                end_of_storage = start + new_cap;
            }
        }

        // 增长策略
        static size_type next_size(size_type cur) {
            return (size_type) ((cur / 2) * 3 + 4);
        }

#pragma endregion
    };
}


#endif //TINYSTL_VECTOR_H