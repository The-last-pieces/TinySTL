//
// Created by IMEI on 2022/4/30.
//

#ifndef TINYSTL_LIST_H
#define TINYSTL_LIST_H

#include "../allocator/memory.h"
#include "../iterator/iterator.h"

namespace ttl {
    template<typename T, typename Alloc = ttl::allocator<T>>
    class list {
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
    private: // helper class
#pragma region
        // 数据结点
        struct list_base_node {
            list_base_node *next;
            list_base_node *prev;
        };
        struct list_node : list_base_node {
            T data;
        };
        // 虚拟根节点, root->next == list.head
        struct list_root : list_base_node {
            size_t size;
            list_base_node *tail;
        };

        // 迭代器
        class list_iterator : public ttl::iterator<ttl::bidirectional_iterator_tag, T> {
            friend class list;

            using Iterator = list_base_node *;

            Iterator current;
        public:
            list_iterator() : current{} {}

            explicit list_iterator(const Iterator &it) noexcept: current(it) {}

            list_iterator(const list_iterator &) = default;

            list_iterator(list_iterator &&) noexcept = default;

            list_iterator &operator=(const list_iterator &) noexcept = default;

            ~list_iterator() = default;

        public:
            reference operator*() const noexcept { return reinterpret_cast<list_node *>(current)->data; }

            pointer operator->() const noexcept {
                return alloc_type::address(reinterpret_cast<list_node *>(current)->data);
            }

        public:
            // forward
            list_iterator &operator++() noexcept {
                current = current->next;
                return *this;
            }

            list_iterator operator++(int) noexcept {
                Iterator old = current;
                current = current->next;
                return list_iterator(old);
            }

            // bi direct
            list_iterator &operator--() noexcept {
                current = current->prev;
                return *this;
            }

            list_iterator operator--(int) noexcept {
                Iterator old = current;
                current = current->prev;
                return list_iterator(old);
            }

            friend bool operator==(const list_iterator &lhs, const list_iterator &rhs) {
                return lhs.current == rhs.current;
            }

            friend bool operator!=(const list_iterator &lhs, const list_iterator &rhs) {
                return lhs.current != rhs.current;
            }
        };

        class const_list_iterator : ttl::iterator<ttl::bidirectional_iterator_tag, const T> {
            friend class list;

            using Iterator = const list_base_node *;

            Iterator current;

            list_iterator iter() const {
                return list_iterator(const_cast<list_base_node *>(current));
            }

        public:
            const_list_iterator() : current{} {}

            explicit const_list_iterator(const Iterator &it) noexcept: current(it) {}

            const_list_iterator// NOLINT(google-explicit-constructor)
                    (const list_iterator &oth) noexcept: current(oth.current) {};

            const_list_iterator(const const_list_iterator &) = default;

            const_list_iterator(const_list_iterator &&) noexcept = default;

            const_list_iterator &operator=(const const_list_iterator &) noexcept = default;

            ~const_list_iterator() = default;

        public:
            reference operator*() const noexcept { return reinterpret_cast<const list_node *>(current)->data; }

            pointer operator->() const noexcept {
                return alloc_type::address(reinterpret_cast<const list_node *>(current)->data);
            }

        public:
            // forward
            const_list_iterator &operator++() noexcept {
                current = current->next;
                return *this;
            }

            const_list_iterator operator++(int) noexcept {
                Iterator old = current;
                current = current->next;
                return const_list_iterator(old);
            }

            // bi direct
            const_list_iterator &operator--() noexcept {
                current = current->prev;
                return *this;
            }

            const_list_iterator operator--(int) noexcept {
                Iterator old = current;
                current = current->prev;
                return const_list_iterator(old);
            }

            friend bool operator==(const const_list_iterator &lhs, const const_list_iterator &rhs) {
                return lhs.current == rhs.current;
            }

            friend bool operator!=(const const_list_iterator &lhs, const const_list_iterator &rhs) {
                return lhs.current != rhs.current;
            }
        };

    private:
        using alloc_type = ttl::allocator<T>;
        using base_alloc_type = ttl::allocator<list_base_node>;
        using node_alloc_type = ttl::allocator<list_node>;

#pragma endregion
    private:
        list_root root{};
    public: // iter
        using iterator = list_iterator;
        using const_iterator = const_list_iterator;
        using reverse_iterator = ttl::reverse_iterator<iterator>;
        using const_reverse_iterator = ttl::reverse_iterator<const_iterator>;
    public: // constructor
#pragma region

        list() { init_end(); }

        list(size_type n, const value_type &val) {
            init_end();
            auto it = cend();
            while (n--)emplace(it, val);
        }

        explicit list(size_type n) {
            init_end();
            auto it = cend();
            while (n--)emplace(it);
        }

        template<typename InputIt>
        list(InputIt first, InputIt last) {
            init_end();
            assign_copy_aux(first, last);
        }

        list(const list &oth) {
            init_end();
            assign_copy_aux(oth.begin(), oth.end());
        }

        list(list &&oth) noexcept {
            move_storage(oth);
        }

        list(std::initializer_list<value_type> init) {
            init_end();
            assign_copy_aux(init.begin(), init.end());
        }

        ~list() {
            destroy_all();
            base_alloc_type::deallocate(root.tail, 1);
        }

    private:
        // 不变的end()
        void init_end() {
            auto *const_node = base_alloc_type::allocate(1);
            root.next = root.tail = const_node;
            const_node->next = nullptr;
            const_node->prev = &root;
        }

        // 销毁对象
        void destroy_all() {
            list_base_node *cur = root.next, *nxt;
            // [next,tail)为list_node , tail为list_base_node
            for (; cur != root.tail;) {
                nxt = cur->next;
                destroy_node(cur);
                cur = nxt;
            }
            root.next = root.tail;
        }

#pragma endregion
    public: // assign
#pragma region

        list &operator=(const list &oth) {
            if (&oth == this) return *this;
            assign_copy_aux(oth.begin(), oth.end());
            return *this;
        }

        list &operator=(list &&oth) noexcept {
            if (&oth == this) return *this;
            move_storage(oth);
            return *this;
        }

        list &operator=(std::initializer_list<value_type> init) {
            assign_copy_aux(init.begin(), init.end());
            return *this;
        }

        void assign(size_type n, const value_type &value) {
            assign_fill_aux(n, value);
        }

        template<typename InputIt>
        void assign(InputIt first, InputIt last) {
            assign_copy_aux(first, last);
        }

        void assign(std::initializer_list<value_type> x) {
            assign_copy_aux(x.begin(), x.end());
        }

    private:
        // 拷贝赋值
        template<typename InputIt>
        void assign_copy_aux(InputIt first, InputIt last) {
            iterator cur = begin(), ed = end();
            while (first != last && cur != ed)
                *cur++ = *first++;
            while (first != last)
                emplace(ed, *first++);
            erase(cur, ed);
        }

        // 拷贝填充
        void assign_fill_aux(size_type n, const value_type &val) {
            iterator cur = begin(), ed = end();
            while (cur != ed && n--)
                *cur++ = val;
            while (n--)
                emplace(ed, val);
            erase(cur, ed);
        }

        // 移动对象
        void move_storage(list &oth) {
            root = oth.root, oth.root = {}, oth.init_end();
        }

#pragma endregion
    public: // visit
#pragma region

        reference front() { return *begin(); }

        const_reference front() const { return *begin(); }

        reference back() { return *--end(); }

        const_reference back() const { return *--end(); }

#pragma endregion
    public: // iterator
#pragma region

        iterator begin() const { return iterator(root.next); }

        iterator end() const { return iterator(root.tail); }

        const_iterator cbegin() const { return const_iterator(root.next); }

        const_iterator cend() const { return const_iterator(root.tail); }

        reverse_iterator rbegin() const { return reverse_iterator(end()); }

        reverse_iterator rend() const { return reverse_iterator(begin()); }

        const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }

        const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

#pragma endregion
    public: // capacity
#pragma region

        bool empty() const { return root.size == 0; }

        size_type size() const { return root.size; }

        static size_type max_size() {
            return alloc_type::max_size();
        }

#pragma endregion
    public: // change
#pragma region

        void clear() { destroy_all(); }

        iterator insert(const_iterator pos, const value_type &val) {
            return insert_front_aux(pos.iter(), make_node_aux(val));
        }

        iterator insert(const_iterator pos, value_type &&val) {
            return insert_front_aux(pos.iter(), make_node_aux(val));
        }

        iterator insert(const_iterator pos, size_type n, const value_type &val) {
            if (n-- == 0) return pos.iter();
            --pos;
            iterator ret = insert_back_aux(pos.iter(), make_node_aux(val)), cur = ret;
            while (n--) cur = insert_back_aux(cur, make_node_aux(val));
            return ret;
        }

        template<class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last) {
            if (first == last) return pos.iter();
            --pos;
            iterator ret = insert_back_aux(pos.iter(), make_node_aux(*first++)), cur = ret;
            while (first != last) cur = insert_back_aux(cur, make_node_aux(*first++));
            return ret;
        }

        iterator insert(const_iterator pos, std::initializer_list<T> init) {
            return insert(pos, init.begin(), init.end());
        }

        template<typename ...Args>
        iterator emplace(const_iterator pos, Args &&...args) {
            return insert_front_aux(pos.iter(), make_node_aux(std::forward<Args>(args)...));
        }

        iterator erase(const_iterator pos) {
            // pre->next , nxt->prev
            list_base_node *cur = pos.iter().current;
            list_base_node *pre = cur->prev, *nxt = cur->next;
            pre->next = nxt, nxt->prev = pre;
            destroy_node(cur);
            return iterator(nxt);
        }

        iterator erase(const_iterator first, const_iterator last) {
            if (first == last) return last.iter();
            // [first, last)
            list_base_node *pre = first.iter().current->prev, *nxt = last.iter().current;
            for (list_base_node *cur = pre->next, *temp; cur != nxt;) {
                temp = cur->next;
                destroy_node(cur);
                cur = temp;
            }
            pre->next = nxt, nxt->prev = pre;
            return last.iter();
        }

        void pop_back() {
            erase(--cbegin());
        }

        template<typename ...Args>
        iterator emplace_back(Args &&...args) {
            return emplace(cend(), std::forward<Args>(args)...);
        }

        void push_back(const value_type &val) {
            emplace_back(val);
        }

        void push_back(value_type &&val) {
            emplace_back(val);
        }

        void push_front(const value_type &val) {
            emplace_front(val);
        }

        void push_front(value_type &&val) {
            emplace_front(val);
        }

        template<typename ...Args>
        iterator emplace_front(Args &&...args) {
            return emplace(cend(), std::forward<Args>(args)...);
        }

        void pop_front() {
            erase(cbegin());
        }

        // Todo
        void resize() {}

        void swap() {}

    private:
        // 在pos后面插入元素
        static iterator insert_back_aux(iterator it, list_base_node *node) {
            // 更新pos->next , nxt->prev
            auto *pos = it.current;
            auto *nxt = pos->next;
            pos->next = node, node->next = nxt;
            nxt->prev = node, node->prev = pos;
            return iterator(node);
        }

        // 在pos前面插入元素
        static iterator insert_front_aux(iterator it, list_base_node *node) {
            // 更新pos->prev , pre->next
            auto *pos = it.current;
            auto *pre = pos->prev;
            pos->prev = node, node->prev = pre;
            pre->next = node, node->next = pos;
            return iterator(node);
        }

        // 创建实体结点
        template<typename ...Args>
        list_node *make_node_aux(Args &&...args) {
            list_node *node = node_alloc_type::allocate(1);
            alloc_type::construct(alloc_type::address(node->data), std::forward<Args>(args)...);
            ++root.size;
            return node;
        }

        // 销毁实体结点
        void destroy_node(list_base_node *node) {
            node_alloc_type::destroy(reinterpret_cast<list_node *>(node));
            node_alloc_type::deallocate(reinterpret_cast<list_node *>(node), 1);
            --root.size;
        }

#pragma endregion
    public: // else operator Todo
#pragma region

        void merge() {}

        void splice() {}

        void remove() {}

        void remove_if() {}

        void reverse() {}

        void unique() {}

        void sort() {}

#pragma endregion
    };
}

#endif //TINYSTL_LIST_H