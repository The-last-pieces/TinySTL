//
// Created by IMEI on 2022/4/30.
//

#ifndef TINYSTL_LIST_H
#define TINYSTL_LIST_H

#include "../allocator/memory.h"
#include "../iterator/iterator.h"

namespace ttl {
    template<typename T> // , typename Alloc = ttl::allocator<T>
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
        template<typename CVT>
        class list_iterator : public ttl::iterator<ttl::bidirectional_iterator_tag, CVT> {
            friend class list;

            using Iterator = list_base_node *;
        public:
            using value_type = CVT;
            using pointer = CVT *;
            using reference = CVT &;
            using size_type = size_t;
            using difference_type = ptrdiff_t;
        private:
            Iterator current;
        public: // constructor
            list_iterator() = default;

            list_iterator(const list_iterator &) = default;

            explicit list_iterator(const Iterator &it) noexcept: current(it) {}

            template<typename OV>
            list_iterator(const list_iterator<OV> oth) noexcept: // NOLINT(google-explicit-constructor)
                    current(oth.current) {}

        public: // ops
            reference operator*() const noexcept { return reinterpret_cast<list_node *>(current)->data; }

            pointer operator->() const noexcept {
                return alloc_type::address(reinterpret_cast<list_node *>(current)->data);
            }

            // forward
            list_iterator &operator++() noexcept {
                return current = current->next, *this;
            }

            list_iterator operator++(int) noexcept {
                list_iterator tmp = *this;
                return ++*this, tmp;
            }

            // bi direct
            list_iterator &operator--() noexcept {
                return current = current->prev, *this;
            }

            list_iterator operator--(int) noexcept {
                list_iterator tmp = *this;
                return --*this, tmp;
            }

            friend bool operator==(const list_iterator &lhs, const list_iterator &rhs) {
                return lhs.current == rhs.current;
            }

            friend bool operator!=(const list_iterator &lhs, const list_iterator &rhs) {
                return !(lhs.current == rhs.current);
            }
        };

#pragma endregion
    private:
        using alloc_type = ttl::allocator<T>;
        using base_alloc_type = ttl::allocator<list_base_node>;
        using node_alloc_type = ttl::allocator<list_node>;
    private:
        list_root root{};
    public: // iter
        using iterator = list_iterator<value_type>;
        using const_iterator = list_iterator<const value_type>;
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
            return insert_front_aux(pos, make_node_aux(val));
        }

        iterator insert(const_iterator pos, value_type &&val) {
            return insert_front_aux(pos, make_node_aux(std::move(val)));
        }

        iterator insert(const_iterator pos, size_type n, const value_type &val) {
            if (n-- == 0) return pos;
            --pos;
            iterator ret = insert_back_aux(pos, make_node_aux(val)), cur = ret;
            while (n--) cur = insert_back_aux(cur, make_node_aux(val));
            return ret;
        }

        template<class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last) {
            if (first == last) return pos;
            --pos;
            iterator ret = insert_back_aux(pos, make_node_aux(*first++)), cur = ret;
            while (first != last) cur = insert_back_aux(cur, make_node_aux(*first++));
            return ret;
        }

        iterator insert(const_iterator pos, std::initializer_list<T> init) {
            return insert(pos, init.begin(), init.end());
        }

        template<typename ...Args>
        iterator emplace(const_iterator pos, Args &&...args) {
            return insert_front_aux(pos, make_node_aux(std::forward<Args>(args)...));
        }

        iterator erase(const_iterator pos) {
            // pre->next , nxt->prev
            list_base_node *cur = pos.current;
            list_base_node *pre = cur->prev, *nxt = cur->next;
            pre->next = nxt, nxt->prev = pre;
            destroy_node(cur);
            return iterator(nxt);
        }

        iterator erase(const_iterator first, const_iterator last) {
            if (first == last) return last;
            // [first, last)
            list_base_node *pre = first.current->prev, *nxt = last.current;
            for (list_base_node *cur = pre->next, *temp; cur != nxt;) {
                temp = cur->next;
                destroy_node(cur);
                cur = temp;
            }
            pre->next = nxt, nxt->prev = pre;
            return last;
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
            emplace_back(std::move(val));
        }

        void push_front(const value_type &val) {
            emplace_front(val);
        }

        void push_front(value_type &&val) {
            emplace_front(std::move(val));
        }

        template<typename ...Args>
        iterator emplace_front(Args &&...args) {
            return emplace(cend(), std::forward<Args>(args)...);
        }

        void pop_front() {
            erase(cbegin());
        }

        void resize(size_type n) {
            resize(n, value_type());
        }

        void resize(size_type n, const value_type &x) {
            size_t cn = size();
            if (n > cn) {
                insert(cend(), n - cn, x);
            } else if (n < cn) {
                erase(at(n), cend());
            }
        }

        void swap(list &oth) {
            std::swap(root, oth.root);
        }

#pragma endregion
    private: // help change
#pragma region

        // 在pos后面插入元素
        static iterator insert_back_aux(iterator it, list_base_node *node) {
            // pos->node->nxt
            auto *pos = it.current;
            auto *nxt = pos->next;
            hook(pos, node), hook(node, nxt);
            return iterator(node);
        }

        // 在pos前面插入元素
        static iterator insert_front_aux(iterator it, list_base_node *node) {
            // pre->node->nxt
            auto *pos = it.current;
            auto *pre = pos->prev;
            hook(pre, node), hook(node, pos);
            return iterator(node);
        }

        // 在pos后面插入[first,last]
        static void insert_back_aux(iterator it, list_base_node *first, list_base_node *last) {
            // pos->first->....->last->nxt
            auto *pos = it.current;
            auto *nxt = pos->next;
            hook(pos, first), hook(last, nxt);
        }

        // 在pos后面插入[first,last]
        static void insert_front_aux(iterator it, list_base_node *first, list_base_node *last) {
            // pre->first->....->last->pos
            auto *pos = it.current;
            auto *pre = pos->prev;
            hook(pre, first), hook(last, pos);
        }

        // 连接pre<->nxt
        static void hook(list_base_node *pre, list_base_node *nxt) {
            pre->next = nxt, nxt->prev = pre;
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

        // 获取第i个元素的迭代器
        iterator at(size_type i) const {
            return i < size() / 2 ?
                   ttl::next(begin(), difference_type(i)) :
                   ttl::prev(end(), difference_type(size() - i));
        }

#pragma endregion
    public: // else operator
#pragma region

        // 合并两个有序列表,将oth中的节点移动到*this中(不检查有序)
        void merge(list &&oth) {
            merge(std::move(oth), std::less<>());
        }

        template<typename Compare>
        void merge(list &&oth, Compare compare) {
            using base_pointer = list_base_node *;
            using node_pointer = list_node *;
            iterator insert_it = begin(), insert_end = end();
            iterator input_it = oth.begin(), input_end = oth.end();
            while (input_it != input_end) {
                if (insert_it != insert_end && compare(*insert_it, *input_it)) {
                    ++insert_it; // 等待>=待插入节点的位置或者end
                } else {
                    insert_it = insert_front_aux(insert_it, (input_it++).current);
                }
            }
            oth.root.next = oth.root.tail;
            root.size += oth.root.size;
            oth.root.size = 0;
        }

        // 从oth中直接移动指针节点到pos
        void splice(const_iterator pos, list &&oth) {
            splice(pos, std::move(oth), oth.cbegin(), oth.cend());
        }

        void splice(const_iterator pos, list &oth) {
            splice(pos, std::move(oth), oth.cbegin(), oth.cend());
        }

        void splice(const_iterator pos, list &&oth, const_iterator start) {
            splice(pos, std::move(oth), start, oth.cend());
        }

        void splice(const_iterator pos, list &oth, const_iterator start) {
            splice(pos, std::move(oth), start, oth.cend());
        }

        // 移动[first, last)到pos的前面
        void splice(const_iterator pos, list &oth, const_iterator first, const_iterator last) {
            splice(pos, std::move(oth), first, last);
        }

        void splice(const_iterator pos, list &&oth, const_iterator first, const_iterator last) {
            if (first == last) return;
            size_type n = ttl::distance(first, last);
            this->root.size += n, oth.root.size -= n;
            list_base_node *pre = first.current->prev, *nxt = last.current;
            list_base_node *first_ptr = first.current, *last_ptr = nxt->prev;
            hook(pre, nxt), insert_front_aux(pos, first_ptr, last_ptr);
        }

        size_type remove(const T &value) {
            size_type ret = 0;
            for (auto it = cbegin(), ed = cend(); it != ed;) {
                if (*it == value) it = erase(it), ++ret;
                else ++it;
            }
            return ret;
        }

        template<class UnaryPredicate>
        size_type remove_if(UnaryPredicate predicate) {
            size_type ret = 0;
            for (auto it = cbegin(), ed = cend(); it != ed;) {
                if (predicate(*it)) it = erase(it), ++ret;
                else ++it;
            }
            return ret;
        }

        void reverse() {
            if (size() <= 1) return;
            list_base_node *cur = root.next, *nxt, *tmp;
            root.next = root.tail;
            while (cur != root.tail) {
                tmp = cur->next, nxt = root.next;
                hook(&root, cur), hook(cur, nxt), cur = tmp;
            }
        }

        size_type unique() {
            if (size() <= 1) return 0;
            size_type ret = 0;
            for (auto it = cbegin(), ed = cend(), nxt = ttl::next(it); nxt != ed;) {
                if (*it == *nxt) it = erase(it), ++ret;
                else it = nxt;
                nxt = ttl::next(it);
            }
            return ret;
        }

        template<class BinaryPredicate>
        size_type unique(BinaryPredicate predicate) {
            if (size() <= 1) return 0;
            size_type ret = 0;
            for (auto it = cbegin(), ed = cend(), nxt = ttl::next(it); nxt != ed;) {
                if (predicate(*it, *nxt)) it = erase(it), ++ret;
                else it = nxt;
                nxt = ttl::next(it);
            }
            return ret;
        }

        // 原地排序
        void sort() {
            sort(std::less<>());
        }

        template<typename Compare>
        void sort(Compare compare) {
            if (this->size() <= 1) return;
            list half;
            half.splice(half.cend(), *this, at(size() / 2), end());
            this->sort();
            half.sort();
            this->merge(std::move(half), compare);
        }

#pragma endregion
    };
}

#endif //TINYSTL_LIST_H