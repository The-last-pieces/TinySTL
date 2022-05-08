//
// Created by IMEI on 2022/5/4.
//

#ifndef TINYSTL_UNORDERED_MAP_H
#define TINYSTL_UNORDERED_MAP_H

#include <cmath>
#include "./private/hashtable.h"

namespace ttl {
    template<
            typename K, typename V,
            typename HashFcn = std::hash<K>,
            typename KeyEqualFcn = std::equal_to<>>
    class unordered_map {
        using base_map = hashtable<K, V, HashFcn, KeyEqualFcn>;
    public:
        using hasher = HashFcn;
        using key_equal = KeyEqualFcn;
        using value_type = typename base_map::value_type;

        using pointer = value_type *;
        using const_pointer = const value_type *;
        using reference = value_type &;
        using const_reference = const value_type &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
    public:
        using iterator = typename base_map::iterator;
        using const_iterator = typename base_map::const_iterator;
    private:
        base_map table; // 只使用其emplace_unique进行插入

        static const size_type default_size = 11;
    public: // constructor
#pragma region

        explicit unordered_map(size_type bucket_count = default_size,
                               const hasher &hash = hasher(),
                               const key_equal &equal = key_equal()
        ) : table(bucket_count, hash, equal) {}

        template<class InputIt>
        unordered_map(InputIt first, InputIt last,
                      size_type bucket_count = default_size,
                      const hasher &hash = hasher(),
                      const key_equal &equal = key_equal()
        ): table(bucket_count, hash, key_equal()) {
            while (first != last) emplace(*first++);
        }

        unordered_map(std::initializer_list<value_type> init,
                      size_type bucket_count = default_size,
                      const hasher &hash = hasher(),
                      const key_equal &equal = key_equal()
        ) : unordered_map(init.begin(), init.end(), bucket_count, hash, key_equal()) {
        }

        unordered_map(const unordered_map &) = default;

        unordered_map(unordered_map &&) noexcept = default;

        ~unordered_map() = default;

        unordered_map &operator=(const unordered_map &) = default;

        unordered_map &operator=(unordered_map &&) noexcept = default;

#pragma endregion
    public: // iterators
#pragma region

        iterator begin() { return table.begin(); }

        const_iterator begin() const { return table.begin(); }

        const_iterator cbegin() const { return table.cbegin(); }

        iterator end() { return table.end(); }

        const_iterator end() const { return table.cend(); }

        const_iterator cend() const { return table.cend(); }

#pragma endregion
    public: // capacity
#pragma region

        bool empty() const { return table.empty(); }

        size_type size() const { return table.size(); }

        size_type max_size() const { return table.max_size(); }

#pragma endregion
    public: // change
#pragma region

        void clear() { table.clear(); }

        std::pair<iterator, bool> insert(const value_type &value) { return emplace(value); }

        std::pair<iterator, bool> insert(value_type &&value) { return emplace(std::forward<value_type>(value)); }

        template<typename InputIt>
        void insert(InputIt first, InputIt last) { while (first != last) emplace(*first++); }

        void insert(std::initializer_list<value_type> init) { insert(init.begin(), init.end()); }

        template<typename... Args>
        std::pair<iterator, bool> emplace(Args &&... args) {
            return table.template emplace_unique(std::forward<Args>(args)...);
        }

        iterator erase(const_iterator pos) { return table.erase(pos); }

        iterator erase(const_iterator first, const_iterator last) { return table.erase(first, last); }

        size_type erase(const K &key) { return table.erase(key); }

        void swap(unordered_map &other) noexcept {
            table.swap(other.table);
        }

#pragma endregion
    public: // find & visit
#pragma region

        V &at(const K &key) {
            auto it = table.find(key);
            if (it == table.end()) throw std::out_of_range("unordered_map visit outside");
            return it->second;
        }

        const V &at(const K &key) const {
            auto it = table.find(key);
            if (it == table.end()) throw std::out_of_range("unordered_map visit outside");
            return it->second;
        }

        V &operator[](const K &key) {
            auto ret = table.emplace_unique(key, V{});
            return ret.first->second;
        }

        V &operator[](K &&key) {
            auto ret = table.emplace_unique(std::forward<K>(key), V{});
            return ret.first->second;
        }

        iterator find(const K &key) { return table.find(key); }

        const_iterator find(const K &key) const { return table.find(key); }

        bool contains(const K &key) const { return find(key) != end(); }

        std::pair<iterator, iterator> equal_range(const K &key) { return table.equal_range(key); }

        std::pair<const_iterator, const_iterator> equal_range(const K &key) const { return table.equal_range(key); }

#pragma endregion
    public: // bucket interface
#pragma region

        // Todo bucket iterator ?

        size_type bucket_count() const { return table.bucket_count(); }

        size_type max_bucket_count() const { return table.max_bucket_count(); }

        size_type bucket_size(size_type n) const { return table.bucket_size(n); }

        size_type bucket(const K &key) const { return table.bucket(key); }

#pragma endregion
    public: // hash policy
#pragma region

        float max_load_factor() const { return table.max_load_factor(); }

        void max_load_factor(float ml) { table.max_load_factor(ml); }

        void rehash(size_type count) { table.rehash(count); }

        void reserve(size_type count) { table.reserve(count); }

#pragma endregion
    public: // operator
        friend bool operator==(const unordered_map &lhs, const unordered_map &rhs) {
            return lhs.table == rhs.table;
        }

        friend bool operator!=(const unordered_map &lhs, const unordered_map &rhs) {
            return !(rhs == lhs);
        }
    };

}

#endif //TINYSTL_UNORDERED_MAP_H
