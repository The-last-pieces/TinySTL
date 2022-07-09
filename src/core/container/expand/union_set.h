//
// Created by IMEI on 2022/7/8.
//

#ifndef TINYSTL_UNION_SET_H
#define TINYSTL_UNION_SET_H

#include <unordered_map>
#include <vector>

namespace ttl {

    // 并查集 Todo 支持传递T给需要identity_type参数的地方
    template<typename T, typename Hash  = std::hash<T>>
    class union_set {
    public:
        // 将每个对象映射到一个唯一id中
        using identity_type = size_t;
    private:
        std::vector<identity_type> parent; // 对象的父节点
        std::unordered_map<T, identity_type> identity; // 对象到唯一id的映射
    public:
        // 获取对象所在集合的根节点id
        identity_type get_parent(identity_type x) {
            auto &px = parent[x];
            if (px != x) px = get_parent(px);
            return px;
        }

        // 合并两个集合
        void link(identity_type a, identity_type b) {
            auto pa = get_parent(a);
            auto pb = get_parent(b);
            if (pa != pb) parent[pa] = pb;
        }

        // 判断是否连通
        bool is_connected(identity_type a, identity_type b) {
            return get_parent(a) == get_parent(b);
        }

        // 获取对象id
        identity_type id(const T &x) {
            auto it = identity.find(x);
            if (it == identity.end()) {
                identity_type id = identity.size();
                identity[x] = id;
                parent.push_back(id);
                return id;
            } else {
                return it->second;
            }
        }

        // 预定对象个数
        void reserve(size_t n) {
            parent.reserve(n);
        }

    private:
        // 获取唯一id, IsId表示这个值已经是一个id了 Todo 优化模板
//        template<bool IsId, class U>
//        constexpr identity_type get_identity(const U &x) {
//            if constexpr (IsId) {
//                static_assert(std::is_convertible_v<U, identity_type>, "U must be convertible to identity_type");
//                return x;
//            } else {
//                auto it = identity.find(x);
//                if (it == identity.end()) {
//                    identity_type id = identity.size();
//                    identity[x] = id;
//                    parent.push_back(id);
//                    return id;
//                } else {
//                    return it->second;
//                }
//            }
//        }
    };

}

#endif //TINYSTL_UNION_SET_H
