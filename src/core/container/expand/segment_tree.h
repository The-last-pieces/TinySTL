//
// Created by IMEI on 2022/7/6.
//

#ifndef TINYSTL_SEGMENT_TREE_H
#define TINYSTL_SEGMENT_TREE_H

#include <vector>
#include <functional>

namespace ttl {
    /*
    本线段树维护的元素是一个有如下性质的群
    M = (S,o,*,e),满足以下性质
    1:任意x,y∈S,x o y = y o x ∈ S                              // 可交换
    2:任意x,y,z∈S , (x o y) o z = x o (y o z)                   // 可结合
    3:任意x∈S , 有e o x = x o e = x                             // 单位元
    4:对任意z∈Z+,x∈S, 有 z * x = x o x o x o ... o x (z个x) ,   // 数乘

    常见的例子是,区间求和:
    segment_tree<
        int,                                // 元素类型
        [](int a, int b) { return a + b; }, // 加法运算
        int,                                // 重复次数
        [](int a, int b) { return a * b; }, // 乘法运算
    >
    */

    template<typename T1, typename T2>
    class multiplies {
    public:
        auto operator()(const T1 &v1, const T2 &v2) {
            return v1 * v2;
        }
    };

    template<
            typename T,
            //int FeatureTag = 1 | 2 | 4, // 1为add, 2为mut , 4为set Todo 使用tag来定制轻量级的操作
            typename Merge = std::plus<T>,
            typename RepeatType = int32_t,
            typename Repeat = ttl::multiplies<T, RepeatType>
    >
    class segment_tree {
        static_assert(std::is_integral_v<RepeatType>); // 必须是整数
    private:
#pragma region 辅助类型
        using index_type = int32_t;
        template<class U>
        using Container = std::vector<U>;

        struct tree_index {
            index_type idx;    // 对应的tree_node 在 tree中的下标
            index_type l, r;   // 掌管的区间[l,r]

            // 分裂为[l,mid], [mid+1,r]
            auto get_lr() const {
                index_type mid = l + (r - l) / 2;
                tree_index li{idx * 2 + 1, l, mid};
                tree_index ri{idx * 2 + 2, mid + 1, r};
                return std::make_pair(li, ri);
            }

            // 是否为叶子结点
            bool is_leaf() const { return l == r; }

            // 区间大小
            index_type width() const { return r - l + 1; }

            // 是否非空
            bool is_valid() const { return l <= r; }

            // [l,r]是否包含于[s,t]
            bool is_in(index_type s, index_type t) const {
                return s <= l && r <= t;
            }
        };

        struct tree_node {
            T merged;               // 合并后的结果
            T add;                  // add的懒惰更新
            T set;                  // set的懒惰节点
            bool on_setting;             // 记录是否激活了set标签
            RepeatType mut;         // mut的懒惰节点
        };

        class element_reference {
            friend class segment_tree;

            segment_tree *ptr;
            index_type idx;

        private:
            element_reference(segment_tree *ptr, index_type idx) : ptr(ptr), idx(idx) {}

        public:
            element_reference &operator=(const T &val) {
                ptr->set(idx, idx, val);
                return *this;
            }

            element_reference &operator+=(const T &val) {
                ptr->add(idx, idx, val);
                return *this;
            }

            element_reference &operator*=(RepeatType k) {
                ptr->mut(idx, idx, k);
                return *this;
            }

            operator T() { // NOLINT(google-explicit-constructor)
                return ptr->get(idx);
            }
        };

        class element_iterator : public ttl::iterator<ttl::random_access_iterator_tag, T> {
            friend class segment_tree;

            using difference_type = std::ptrdiff_t;


            element_reference refer;
        private:
            explicit element_iterator(element_reference refer) : refer(refer) {}

        public: // ops
            element_reference operator*() const { return refer; }

            difference_type operator-(const element_iterator &x) const {
                return refer.idx - x.refer.idx;
            }

            element_iterator &operator++() {
                return ++refer.idx, *this;
            }

            element_iterator operator++(int) {
                element_iterator tmp = *this;
                return ++*this, tmp;
            }

            element_iterator &operator--() {
                return --refer.idx, *this;
            }

            element_iterator operator--(int) {
                element_iterator tmp = *this;
                return --*this, tmp;
            }

            element_iterator &operator+=(difference_type n) {
                return refer.idx += n, *this;
            }

            element_iterator operator+(difference_type n) const {
                return element_iterator(*this) += n;
            }

            element_iterator &operator-=(difference_type n) {
                return (*this) += (-n);
            }

            element_iterator operator-(difference_type n) const {
                return element_iterator(*this) += -n;
            }

            element_reference operator[](size_t n) const { return *(*this + difference_type(n)); }

            bool operator==(const element_iterator &rhs) const {
                return refer.ptr == rhs.refer.ptr && refer.idx == rhs.refer.idx;
            }

            bool operator!=(const element_iterator &rhs) const {
                return !(rhs == *this);
            }

            bool operator<(const element_iterator &rhs) const {
                return refer.idx < rhs.refer.idx;
            }

            bool operator>(const element_iterator &rhs) const {
                return rhs < *this;
            }

            bool operator<=(const element_iterator &rhs) const {
                return !(rhs < *this);
            }

            bool operator>=(const element_iterator &rhs) const {
                return !(*this < rhs);
            }
        };

#pragma endregion
    public:
#pragma region 公开类型
        using reference = element_reference;

        using iterator = element_iterator;
#pragma endregion
    private:
#pragma region 辅助类的默认构造

        // 根索引
        tree_index root_index() const {
            return {0, 0, len - 1};
        }

        // 空结点
        tree_node empty_node() const {
            return {epsilon, epsilon, epsilon, false, 1};
        }

#pragma endregion
    private:
#pragma region 成员变量
        // 数学性质(加法,整数乘,加法幺元)
        Merge merge;            // 如何合并两个区间的结果
        Repeat repeat;          // 如何重复两个区间合并的结果
        const T epsilon;        // 区间为空时的结果

        // 语言性质(需要可拷贝,可赋值)
        Container<T> elements;      // 真正的元素
        Container<tree_node> trees; // 树状节点,size一般取4*len
        index_type len;             // 等价于elements.size
#pragma endregion
    public:
#pragma region 生命周期

        explicit segment_tree(
                const Container<T> &input,
                Merge merge = {},
                Repeat repeat = {},
                const T &epsilon = {}
        ) : merge(merge), repeat(repeat), epsilon(epsilon),
            len((index_type) input.size()),
            elements(input),
            trees(input.size() * 4, empty_node()) {
            build(root_index());
        }

#pragma endregion
    public:
#pragma region 迭代器

        iterator begin() { return iterator(element_reference(this, 0)); }

        iterator end() { return iterator(element_reference(this, len)); }

        element_reference operator[](size_t idx) {
            return element_reference(this, idx);
        }

#pragma endregion
    public:
#pragma region 公开方法

        // 获取[l,r]的merge结果
        T get(index_type l, index_type r) {
            return get(root_index(), l, r);
        }

        // 获取位于idx的元素
        T get(index_type idx) { return get(idx, idx); }

        // 使得[l,r]每个元素都加val
        void add(index_type l, index_type r, const T &val) {
            return add(root_index(), l, r, val);
        }

        // 使得[l,r]每个元素都乘k
        void mut(index_type l, index_type r, RepeatType k) {
            return mut(root_index(), l, r, k);
        }

        // 使得[l,r]每个元素都等于val
        void set(index_type l, index_type r, const T &val) {
            return set(root_index(), l, r, val);
        }

#pragma endregion
    private:
#pragma region 辅助实现

        // 使得tree[root].merge = data[root.l, root.r]的merge和
        void build(tree_index index) {
            if (index.is_leaf()) {
                trees[index.idx].merged = elements[index.l];
            } else {
                auto [l, r] = index.get_lr();
                build(l), build(r);
                update(index);
            }
        }

        // 获取[s,t]的merge结果
        T get(tree_index index, index_type s, index_type t) {
            if (index.is_in(s, t)) {
                return trees[index.idx].merged;
            } else {
                push_down(index);
                auto [l, r] = index.get_lr();
                T result = epsilon;
                if (s <= l.r) result = merge(result, get(l, s, t));
                if (t >= r.l) result = merge(result, get(r, s, t));
                return result;
            }
        }

        // 使得[s,t]每个元素都加val
        void add(tree_index index, index_type s, index_type t, const T &val) {
            if (index.is_in(s, t)) {
                auto &rf = trees[index.idx];
                rf.add = merge(rf.add, val);
                rf.merged = merge(rf.merged, repeat(val, index.width()));
            } else {
                push_down(index);
                auto [l, r] = index.get_lr();
                if (s <= l.r) add(l, s, t, val);
                if (t >= r.l) add(r, s, t, val);
                update(index);
            }
        }

        // 使得[s,t]每个元素都乘k
        void mut(tree_index index, index_type s, index_type t, RepeatType k) {
            if (index.is_in(s, t)) {
                auto &rf = trees[index.idx];
                rf.add = repeat(rf.add, k);
                rf.mut *= k;
                rf.merged = repeat(rf.merged, k);
            } else {
                push_down(index);
                auto [l, r] = index.get_lr();
                if (s <= l.r) mut(l, s, t, k);
                if (t >= r.l) mut(r, s, t, k);
                update(index);
            }
        }

        // 使得[s,t]每个元素都等于val
        void set(tree_index index, index_type s, index_type t, const T &val) {
            if (index.is_in(s, t)) {
                auto &rf = trees[index.idx];
                rf.merged = repeat(val, index.width());
                set_val(rf, val);
            } else {
                push_down(index);
                auto [l, r] = index.get_lr();
                if (s <= l.r) set(l, s, t, val);
                if (t >= r.l) set(r, s, t, val);
                update(index);
            }
        }

        // 从左右结点获取正确的值
        void update(tree_index index) {
            trees[index.idx].merged = merge(trees[index.l].merged, trees[index.r].merged);
        }

        // 将惰性修改的值传递给左右结点,并更新merged
        void push_down(tree_index index) {
            auto &root = trees[index.idx];
            auto [l, r] = index.get_lr();
            auto &left = trees[l.idx], &right = trees[r.idx];
            if (root.on_setting) {
                left.merged = repeat(root.set, l.width());
                set_val(left, root.set);

                right.merged = repeat(root.set, r.width());
                set_val(right, root.set);

                root.on_setting = false;
            } else {
                left.merged = merge(
                        repeat(left.merged, root.mut),
                        repeat(root.add, l.width())
                );
                left.mut *= root.mut;
                left.add = merge(repeat(left.add, root.mut), root.add);

                right.merged = merge(
                        repeat(right.merged, root.mut),
                        repeat(root.add, r.width())
                );
                right.mut *= root.mut;
                right.add = merge(repeat(right.add, root.mut), root.add);

                root.add = epsilon, root.mut = 1;
            }
        }

        // 使得某个树结点的set为val
        void set_val(tree_node &node, const T &val) const {
            node.set = val, node.on_setting = true;
            node.add = epsilon, node.mut = 1;
        }

#pragma endregion
    };
}

#endif //TINYSTL_SEGMENT_TREE_H
