//
// Created by IMEI on 2022/7/8.
//

#ifndef TINYSTL_LRU_CACHE_H
#define TINYSTL_LRU_CACHE_H

#include <unordered_map>
#include <list>

namespace ttl {

    // 淘汰最久未使用的缓存容器.
    // 相比与一般的缓存容器,LRU缓存能有效节约内存的使用量
    template<typename K, typename V, typename Hash = std::hash<K>>
    class lru_cache {
        using value_type = std::pair<K, V>;
        using iterator = typename std::list<value_type>::iterator;
        using map_iterator = typename std::unordered_map<K, iterator, Hash>::iterator;
    private:
        std::list<value_type> queue; // 使用list充当队列
        std::unordered_map<K, iterator, Hash> hmp; // 建立K到迭代器的映射
        size_t max_size = 1024;
    public:
        void put(const K &key, const V &value) {
            auto it = hmp.find(key);
            if (it == hmp.end()) {
                queue.emplace_front(key, value);
                hmp[key] = queue.begin();
                shift_to_fix_size();
            } else {
                it->second->second = value;
                move_iterator_to_front(it);
            }
        }

        V get_or_default(const K &key, const V &def = {}) {
            auto it = hmp.find(key);
            if (it == hmp.end()) return def;
            return move_iterator_to_front(it)->second;
        }

        void erase(const K &key) {
            auto it = hmp.find(key);
            if (it == hmp.end()) return;
            queue.erase(it->second);
            hmp.erase(it);
        }

        bool count(const K &key) {
            return hmp.count(key);
        }

        void clear() {
            queue.clear();
            hmp.clear();
        }

        void set_max_size(size_t size) {
            max_size = size;
            shift_to_fix_size();
        }

    private:
        iterator move_iterator_to_front(map_iterator it) {
            queue.push_front(std::move(*(it->second))), queue.erase(it->second);
            return it->second = queue.begin();
        }

        void shift_to_fix_size() {
            while (queue.size() > max_size) {
                hmp.erase(queue.back().first);
                queue.pop_back();
            }
        }
    };
}

#endif //TINYSTL_LRU_CACHE_H
