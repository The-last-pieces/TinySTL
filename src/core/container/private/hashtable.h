//
// Created by IMEI on 2022/5/2.
//

#ifndef TINYSTL_HASHTABLE_H
#define TINYSTL_HASHTABLE_H

#include "../../allocator/memory.h"
#include "../../container/vector.h"
#include "../../iterator/iterator.h"

namespace ttl {
    template<
            typename K, typename V,
            typename HashFcn = std::hash<K>,
            typename KeyEqualFcn = std::equal_to<>>
    class hashtable {
    public:
        using hasher = HashFcn;
        using key_equal = KeyEqualFcn;
        using value_type = std::pair<const K, V>; // Todo ttl::pair

        using pointer = value_type *;
        using const_pointer = const value_type *;
        using reference = value_type &;
        using const_reference = const value_type &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
    private: // helper class
#pragma region

        // 开链法
        struct bucket_node {
            bucket_node *next;
            value_type value;
        };

        using bucket_container = ttl::vector<bucket_node *>;

        using alloc_type = ttl::allocator<value_type>;
        using node_alloc_type = ttl::allocator<bucket_node>;

        // 迭代器
        class hashtable_iterator : public ttl::iterator<ttl::forward_iterator_tag, value_type> {
            friend class hashtable;

            bucket_node *cur; // 迭代器当前位置
            hashtable *table; // 所属的容器
        public: // constructor
            hashtable_iterator() = default;

            hashtable_iterator(bucket_node *node, hashtable *belong) :
                    cur(node), table(belong) {}

            hashtable_iterator(const hashtable_iterator &oth) = default;

            hashtable_iterator(hashtable_iterator &&oth) noexcept = default;

            ~hashtable_iterator() = default;

        public: // ops
            reference operator*() const { return cur->value; }

            pointer operator->() const { return alloc_type::address(cur->value); }

            hashtable_iterator &operator++() {
                cur = table->next_node(cur);
                return *this;
            }

            hashtable_iterator operator++(int) {
                hashtable_iterator tmp = *this;
                ++*this;
                return tmp;
            }

        public:
            friend bool operator==(const hashtable_iterator &lhs, const hashtable_iterator &rhs) {
                return lhs.cur == rhs.cur;
            }

            friend bool operator!=(const hashtable_iterator &lhs, const hashtable_iterator &rhs) {
                return !(rhs == lhs);
            }
        };

        class const_hashtable_iterator : public ttl::iterator<ttl::forward_iterator_tag, value_type> {
            friend class hashtable;

            bucket_node *cur; // 迭代器当前位置
            hashtable *table; // 所属的容器
        public: // constructor
            const_hashtable_iterator() = default;

            const_hashtable_iterator(const bucket_node *node, const hashtable *belong) :
                    cur(const_cast<bucket_node *>(node)), table(const_cast<hashtable *>(belong)) {}

            const_hashtable_iterator(const hashtable_iterator &oth) : // NOLINT(google-explicit-constructor)
                    const_hashtable_iterator(oth.cur, oth.table) {}

            const_hashtable_iterator(const const_hashtable_iterator &oth) = default;

            const_hashtable_iterator(const_hashtable_iterator &&oth) noexcept = default;

            ~const_hashtable_iterator() = default;

        public: // ops
            const_reference operator*() const { return cur->value; }

            const_pointer operator->() const { return alloc_type::address(cur->value); }

            const_hashtable_iterator &operator++() {
                cur = table->next_node(cur);
                return *this;
            }

            const_hashtable_iterator operator++(int) {
                const_hashtable_iterator tmp = *this;
                ++*this;
                return tmp;
            }

        public:
            friend bool operator==(const const_hashtable_iterator &lhs, const const_hashtable_iterator &rhs) {
                return lhs.cur == rhs.cur;
            }

            friend bool operator!=(const const_hashtable_iterator &lhs, const const_hashtable_iterator &rhs) {
                return !(rhs == lhs);
            }
        };

        using iterator = hashtable_iterator;
        using const_iterator = const_hashtable_iterator;

#pragma endregion
    private: // fields
        hasher hash_fcn;
        key_equal equal_fcn;
        bucket_container buckets;
        size_type num_elements{}; // 实际元素个数
        float factor = 1; // size()/bucket_size()<=factor

        static const size_type default_size = 11;
    public: // constructor
#pragma region

        hashtable() : hashtable(default_size) {}

        explicit hashtable(size_type bucket_count,
                           const hasher &hash = hasher(),
                           const key_equal &equal = key_equal()
        ) : hash_fcn(hash), equal_fcn(equal), buckets(bucket_count, nullptr) {}

        hashtable(const hashtable &oth) {
            copy_from(oth);
        }

        hashtable(hashtable &&oth) noexcept:
                buckets(std::move(oth.buckets)),
                hash_fcn(std::move(oth.hash_fcn)),
                equal_fcn(std::move(oth.equal_fcn)) {
            num_elements = oth.num_elements, oth.num_elements = 0;
        }

        ~hashtable() {
            destroy_all();
        }

        hashtable &operator=(const hashtable &oth) {
            if (&oth == this) return *this;
            copy_from(oth);
            return *this;
        }

        hashtable &operator=(hashtable &&oth) noexcept {
            if (&oth == this) return *this;
            buckets = std::move(oth.buckets);
            num_elements = oth.num_elements, oth.num_elements = 0;
            hash_fcn = std::move(oth.hash_fcn), equal_fcn = std::move(oth.equal_fcn);
            return *this;
        }

#pragma endregion
    public: // iterators
#pragma region

        iterator begin() { return {first_bucket(), this}; }

        const_iterator begin() const { return {first_bucket(), this}; }

        const_iterator cbegin() const { return {first_bucket(), this}; }

        iterator end() { return {nullptr, this}; }

        const_iterator end() const { return {nullptr, this}; }

        const_iterator cend() const { return {nullptr, this}; }

    private:
        bucket_node *first_bucket() const {
            auto it = buckets.cbegin(), ed = buckets.cend();
            while (*it == nullptr && it != ed) ++it;
            return it == ed ? nullptr : *it;
        }

#pragma endregion
    public: // capacity
#pragma region

        bool empty() const { return num_elements == 0; }

        size_type size() const { return num_elements; }

        size_type max_size() const { return primers[primer_cnt - 1]; }

#pragma endregion
    private: // change helper
#pragma region

#pragma region // constant
        static const size_t primer_cnt = 28;
        constexpr static const size_type primers[primer_cnt] = {
                53, 97, 193, 389,
                769, 1543, 3079, 6151,
                12289, 24593, 49157, 98317,
                196613, 393241, 786433, 1572869,
                3145739, 6291469, 12582917, 25165843,
                50331653, 100663319, 201326611, 402653189,
                805306457, 1610612741, 3221225473ul, 4294967291ul
        };
#pragma endregion

        // 最接近target的质数
        static size_type next_primer(size_type target) {
            auto ed = primers + primer_cnt;
            auto it = std::lower_bound(primers, ed, target);
            return it == ed ? *--ed : *it;
        }

        // 创建node
        static bucket_node *make_node(const value_type &kv) {
            bucket_node *ptr = node_alloc_type::allocate(1);
            alloc_type::construct(alloc_type::address(ptr->value), kv);
            return ptr;
        }

        // 摧毁node
        static void destroy_node(bucket_node *node) {
            alloc_type::destroy(alloc_type::address(node->value));
            node_alloc_type::deallocate(node, 1);
        }

        // 将某个结点放到bucket头部
        static bucket_node *put_front(bucket_node *&pos, bucket_node *node) {
            node->next = pos, pos = node;
            return node;
        }

        // 将node放在pre处
        static bucket_node *put_back(bucket_node *pre, bucket_node *node) {
            node->next = pre->next, pre->next = node;
            return node;
        }

        // 未超载
        bool un_overload(size_type ele_size, size_type bkt_size) const {
            return static_cast<float>(ele_size) > static_cast<float>(bkt_size) * factor;
        }

        // 获取key在size个bucket中应该位于的下标
        size_type bucket_index(const K &key, size_type size) const {
            return hash_fcn(key) % size;
        }

        size_type bucket_index(const K &key) const {
            return bucket_index(key, buckets.size());
        }

        // 析构buckets里的每个元素
        void destroy_all() {
            bucket_node *tmp;
            for (auto &ptr: buckets) {
                auto bucket = ptr;
                ptr = nullptr;
                while (bucket) {
                    tmp = bucket->next;
                    destroy_node(bucket);
                    bucket = tmp;
                }
            }
        }

        // 设置最适合的bucket size
        void resize(size_type hint_element_size) {
            size_type bucket_size = buckets.size();
            if (un_overload(hint_element_size, bucket_size)) return;
            size_type new_bucket_size = next_primer(hint_element_size);
            if (un_overload(new_bucket_size, bucket_size)) return;
            bucket_container tmp(new_bucket_size, nullptr);
            bucket_node *nxt;
            for (auto ptr: buckets) {
                while (ptr) {
                    size_type new_index = bucket_index(ptr->value.first, new_bucket_size);
                    nxt = ptr->next; // 保留nxt指针
                    put_front(tmp[new_index], ptr);
                    ptr = nxt;
                }
            }
            buckets.swap(tmp);
        }

        // 复制
        void copy_from(const hashtable &oth) {
            clear();
            size_type bucket_size = oth.buckets.size();
            buckets.resize(bucket_size, nullptr);
            num_elements = oth.num_elements;
            for (size_type i = 0; i < bucket_size; ++i) {
                if (auto ptr = oth.buckets[i]) {
                    while (ptr) {
                        bucket_node *tmp = make_node(ptr->value);
                        ptr = ptr->next;
                        put_front(buckets[i], tmp);
                    }
                }
            }
        }

        // 不重复插入
        std::pair<iterator, bool> insert_unique_no_resize(const value_type &kv) {
            const size_type pos = bucket_index(kv.first); // 插入位置
            // 查询是否有相等结点
            for (auto cur = buckets[pos]; cur; cur = cur->next) {
                if (equal_fcn(cur->value.first, kv.first)) {
                    return std::make_pair(iterator(cur, this), false);
                }
            }
            // 直接头查
            ++num_elements;
            return std::make_pair(iterator(put_front(buckets[pos], make_node(kv)), this), true);
        }

        // 可重复插入
        std::pair<iterator, bool> insert_equal_no_resize(const value_type &kv) {
            const size_type pos = bucket_index(kv.first); // 插入位置
            // 查询是否有相等结点
            for (auto cur = buckets[pos]; cur; cur = cur->next) {
                if (equal_fcn(cur->value.first, kv.first)) {
                    ++num_elements;
                    return std::make_pair(iterator(put_back(cur, make_node(kv)), this), true);
                }
            }
            // 直接头插
            ++num_elements;
            return std::make_pair(iterator(put_front(buckets[pos], make_node(kv)), this), true);
        }

        // 下一个非空位置,没有则返回nullptr
        bucket_node *next_node(bucket_node *cur) const {
            if (!cur) return nullptr;
            bucket_node *old = cur;
            cur = cur->next;
            if (!cur) {
                size_type pos = bucket_index(old->value.first);
                size_type n = bucket_count();
                while (!cur && ++pos < n) cur = buckets[pos];
            }
            return cur;
        }

        // 使pre->next = nxt , pre为nullptr时buckets[index]等效为nullptr->next
        void set_next(bucket_node *pre, bucket_node *nxt, size_type index) {
            (pre ? pre->next : buckets[index]) = nxt;
        }

#pragma endregion
    public: // change
#pragma region

        void clear() { destroy_all(); }

        template<typename ...Args>
        std::pair<iterator, bool> emplace_unique(Args &&...args) {
            resize(num_elements + 1);
            return insert_unique_no_resize(value_type(std::forward<Args>(args)...));
        }

        template<typename ...Args>
        std::pair<iterator, bool> emplace_equal(Args &&...args) {
            resize(num_elements + 1);
            return insert_equal_no_resize(value_type(std::forward<Args>(args)...));
        }

        iterator erase(const_iterator pos) {
            size_type index = bucket_index(pos->first);
            bucket_node *cur = buckets[index], *ptr = pos.cur, *nxt = ptr->next, *pre = nullptr;
            while (cur != ptr) pre = cur, cur = cur->next;
            set_next(pre, nxt, index);
            destroy_node(cur), --num_elements;
            return {nxt, this};
        }

        iterator erase(const_iterator first, const_iterator last) {
            if (first == last) return {last.cur, this};
            size_type index = bucket_index(first->first), n = bucket_count();
            bucket_node *cur = buckets[index], *pre = nullptr, *bound = last.cur, *start = first.cur, *nxt;
            // 找到第一个迭代器的前驱
            while (cur != start) pre = cur, cur = cur->next;
            while (true) {
                // 需要释放节点
                nxt = cur->next, destroy_node(cur), cur = nxt, --num_elements;
                if (cur == nullptr) { // 需要跳bucket
                    set_next(pre, nxt, index), pre = nullptr;
                    while (!cur && ++index < n) cur = buckets[index];
                }
                if (cur == bound) {
                    if (bound) set_next(pre, nxt, index);
                    break;
                }
            }
            return {last.cur, this};
        }

        size_type erase(const K &key) {
            size_type index = bucket_index(key), ret = 0;
            bucket_node *cur = buckets[index], *pre = nullptr, *nxt;
            while (cur && !equal_fcn(cur->value.first, key)) pre = cur, cur = cur->next;
            if (!cur) return 0;
            do {
                nxt = cur->next, destroy_node(cur), cur = nxt, ++ret;
            } while (cur && equal_fcn(cur->value.first, key));
            (pre ? pre->next : buckets[index]) = cur, num_elements -= ret;
            return ret;
        }

        void swap(hashtable &oth) {
            buckets.swap(oth.buckets);
            std::swap(num_elements, oth.num_elements);
            std::swap(hash_fcn, oth.hash_fcn);
            std::swap(equal_fcn, oth.equal_fcn);
        }

#pragma endregion
    public: // find
#pragma region

        size_type count(const K &key) const {
            size_type ret = 0, pos = bucket_index(key);
            bucket_node *cur = buckets[pos];
            while (cur && !equal_fcn(cur->value.first, key)) cur = cur->next;
            while (cur && equal_fcn(cur->value.first, key)) cur = cur->next, ++ret;
            return ret;
        }

        iterator find(const K &key) {
            return {find_by_key(key), this};
        }

        const_iterator find(const K &key) const {
            return {find_by_key(key), this};
        }

        std::pair<iterator, iterator> equal_range(const K &key) {
            auto tmp = find_range_by_key(key);
            return {{tmp.first,  this},
                    {tmp.second, this}};
        }

        std::pair<const_iterator, const_iterator> equal_range(const K &key) const {
            auto tmp = find_range_by_key(key);
            return {{tmp.first,  this},
                    {tmp.second, this}};
        }

    private:
        bucket_node *find_by_key(const K &key) const {
            size_type pos = bucket_index(key);
            bucket_node *cur = buckets[pos];
            while (cur && !equal_fcn(cur->value.first, key)) cur = cur->next;
            return cur;
        }

        std::pair<bucket_node *, bucket_node *> find_range_by_key(const K &key) const {
            bucket_node *first = find_by_key(key), *second = first ? first->next : nullptr;
            while (second && equal_fcn(second->value.first, key)) second = second->next;
            return {first, second};
        }


#pragma endregion
    public: // bucket interface
#pragma region

        // Todo bucket iterator ?

        size_type bucket_count() const { return buckets.size(); }

        size_type max_bucket_count() const { return max_size(); }

        size_type bucket_size(size_type n) const {
            size_type ret = 0;
            bucket_node *cur = buckets[n];
            while (cur) cur = cur->next, ++ret;
            return ret;
        }

        size_type bucket(const K &key) const {
            return bucket_index(key);
        }

#pragma endregion
    public: // hash policy
#pragma region

        float max_load_factor() const { return factor; }

        void max_load_factor(float ml) {
            factor = ml;
        }

        void rehash(size_type count) {
            resize(count);
        }

        void reserve(size_type count) { rehash(std::ceil(static_cast<float>(count) / max_load_factor())); }

#pragma endregion
    };
}

#endif //TINYSTL_HASHTABLE_H
