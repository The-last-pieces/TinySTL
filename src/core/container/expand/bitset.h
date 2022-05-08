//
// Created by IMEI on 2022/5/5.
//

#ifndef TINYSTL_BITSET_H
#define TINYSTL_BITSET_H

#include <cstddef>
#include <cstring>
#include <cassert>
#include <memory>

namespace ttl {
    template<size_t N>
    class bitset {
    private:
        using value_type = unsigned long long;
        using size_type = size_t;

        // 一个char的bit数
        static const size_t char_bit = CHAR_BIT;
        // 一个node的char数
        static const size_t node_size = sizeof(value_type);
        // 一个node的bit数
        static const size_t node_bit = node_size * char_bit;
        // 最后一个node的bit数
        static const size_t last_node_bit = N % node_bit ? N % node_bit : node_bit;
        // 除了最后一个node的bit数
        static const size_t pre_nodes_bit = N - last_node_bit;
        // 实际节点的个数
        static const size_t node_count = pre_nodes_bit / node_bit + 1;
        // 实际节点的字节数
        static const size_t node_byte_count = node_size * node_count;
        // 前置节点的掩码
        static const value_type mask_full_node = value_type(-1);
        // 最后一个节点的掩码
        static const value_type mask_last_node =
                last_node_bit == node_bit ? mask_full_node : (value_type(1) << last_node_bit) - 1;
    public: // helper
#pragma region

        class reference {
            friend class bitset;

            bitset *ptr;
            size_type i;
        private:
            reference(bitset *who, size_type idx) : ptr(who), i(idx) {}

        public:
            reference() = delete;

            reference(const reference &) = default;

            ~reference() = default;

            reference &operator=(const reference &) = default;

        public:
            reference &operator=(bool x) noexcept {
                ptr->set(i, x);
                return *this;
            }

            operator bool() const noexcept { return ptr->test(i); } // NOLINT(google-explicit-constructor)

            bool operator~() const noexcept { return !bool(*this); }

            reference &flip() noexcept {
                ptr->flip(i);
                return *this;
            }
        };

#pragma endregion
    private:
        value_type data[node_count];
    public: // constructors
#pragma region

        bitset() noexcept: data{} {}

        bitset(value_type val) noexcept: data{val} { last(); }  // NOLINT(google-explicit-constructor)

        bitset(const char *val) noexcept: data{} { // NOLINT(google-explicit-constructor)
            for (size_type i = 0; *val; ++val, ++i)if (*val == '1') set(i);
        }

        ~bitset() = default;

        bitset &operator=(const bitset &oth) noexcept {
            if (this != &oth) memcpy(data, oth.data, node_byte_count);
        }

#pragma endregion
    public: // visit
#pragma region

        reference operator[](size_t i) {
            check_pos(i);
            return {this, i};
        }

        bool operator[](size_t i) const {
            return test(i);
        }

        bool test(size_type i) const {
            check_pos(i);
            return get_aux(i);
        }

        bool all() const {
            for (size_type i = 0; i < node_count - 1; ++i) {
                if (data[i] != mask_full_node) return false;
            }
            return last() == mask_last_node;
        }

        bool any() const {
            for (size_type i = 0; i < node_count - 1; ++i) {
                if (data[i] != 0) return true;
            }
            return last() != 0;
        }

        bool none() const {
            for (size_type i = 0; i < node_count - 1; ++i) {
                if (data[i] != 0) return false;
            }
            return last() == 0;
        }

        size_type count() const {
            size_t ret = 0;
            for (size_type i = 0; i < node_count - 1; ++i) {
                // ttl::popcount
                ret += ttl::popcount64(data[i]);
            }
            return ret + ttl::popcount64(last());
        }

        size_type size() const { return N; }

#pragma endregion
    private: // change helper
#pragma region

        // 确保0<offset<char_bit
        // 将this[i]移动到this[i+offset]中
        void do_left_move(size_t offset) {
            auto *ptr = reinterpret_cast<unsigned char *>(data);
            size_type ro = offset % char_bit, lo = char_bit - ro;
            for (size_type i = node_byte_count - 1;;) {
                ptr[i] = (ptr[i - 1] >> lo) | (ptr[i] << ro);
                if (i-- == 1) break;
            }
            ptr[0] <<= ro;
        }

        // 将第i个byte块移动到i+n位置处
        void do_left_by_char(size_t n) {
            if (n == 0) return;
            // 从前往后复制
            auto *ptr = reinterpret_cast<unsigned char *> (data);
            for (size_type i = node_byte_count - 1; i >= n; --i) {
                ptr[i] = ptr[i - n];
            }
        }

        // 确保0<offset<char_bit
        // 将this[i]移动到this[i-offset]中
        void do_right_move(size_t offset) {
            auto *ptr = reinterpret_cast<unsigned char *>(data);
            size_type lo = offset % char_bit, ro = char_bit - lo;
            for (size_type i = 0; i < node_byte_count - 1; ++i) {
                ptr[i] = (ptr[i] >> lo) | (ptr[i + 1] << ro);
            }
            ptr[node_byte_count - 1] >>= lo;
        }

        // 将第i个byte块移动到i-n位置处
        void do_right_by_char(size_t n) {
            memcpy((char *) (data), (char *) (data) + n, node_byte_count - n);
        }

#pragma endregion
    public: // change
#pragma region

        bitset &operator&=(const bitset &other) {
            for (size_type i = 0; i < node_count; ++i) data[i] &= other.data[i];
            return last(), *this;
        }

        bitset &operator|=(const bitset &other) {
            for (size_type i = 0; i < node_count; ++i) data[i] |= other.data[i];
            return last(), *this;
        }

        bitset &operator^=(const bitset &other) {
            for (size_type i = 0; i < node_count; ++i) data[i] ^= other.data[i];
            return last(), *this;
        }

        bitset operator~() const {
            return bitset(*this).flip();
        }

        bitset &operator<<=(size_t offset) {
            if (offset == 0) return *this;
            if (offset >= N) return reset();
            if (offset % char_bit) do_left_move(offset % char_bit);
            size_type char_cnt = offset / char_bit;
            do_left_by_char(char_cnt);
            // 低位补0
            size_type start = char_cnt * char_bit, finish = offset;
            memset(data, 0, char_cnt);
            while (start < finish) set(start, false), ++start;

            return last(), *this;
        }

        bitset &operator>>=(size_t offset) {
            if (offset == 0) return *this;
            if (offset >= N) return reset();
            if (offset % char_bit) do_right_move(offset % char_bit);
            size_type char_cnt = offset / char_bit;
            do_right_by_char(char_cnt);

            // 高位补0
            size_type start = N - offset, index = (start - 1) / char_bit + 1, finish = index * char_bit;
            memset((char *) (data) + index, 0, node_byte_count - index);
            while (start < finish) set(start, false), ++start;

            return last(), *this;
        }

        void set(size_type i, bool value = true) {
            check_pos(i);
            set_aux(i, value);
        }

        bitset &reset() {
            memset(data, 0, node_byte_count);
            return *this;
        }

        bitset &reset(size_type i) {
            set(i, false);
            return *this;
        }

        bitset &flip() {
            for (auto &v: data) v = ~v;
            return last(), *this;
        }

        bitset &flip(size_type i) {
            set(i, !test(i));
            return *this;
        }

#pragma endregion
    public: // convert
#pragma region

        std::string to_string() const {
            std::string ret(N, '0');
            for (size_type i = 0, j = N - 1; i < N; ++i, --j) if (test(i)) ret[j] = '1';
            return ret;
        }

#pragma endregion
    public: // operator
#pragma region

        friend bool operator==(const bitset &lhs, const bitset &rhs) noexcept {
            return 0 == memcmp(lhs.data, rhs.data, node_byte_count - 1) && lhs.last() == rhs.last();
        }

        bool operator!=(const bitset &rhs) const noexcept {
            return !(*this == rhs);
        }

        bitset operator<<(size_type offset) {
            return bitset(*this) <<= offset;
        }

        bitset operator>>(size_type offset) {
            return bitset(*this) >>= offset;
        }

#pragma endregion
    private: // else
#pragma region

        value_type &last() {
            return data[node_count - 1] &= mask_last_node;
        }

        value_type last() const {
            return data[node_count - 1] & mask_last_node;
        }

        void set_aux(size_type i, bool value) {
            if (value) {
                data[i / node_bit] |= make_mask(i % node_bit);
            } else {
                data[i / node_bit] &= ~make_mask(i % node_bit);
            }
        }

        bool get_aux(size_type i) const {
            return data[i / node_bit] & make_mask(i % node_bit);
        }

        static void check_pos(size_type i) {
            if (i >= N) throw std::out_of_range("bitset visit outer");
        }

        static value_type make_mask(size_type i) {
            return 1ull << (i); // (b_width - 1) -
        }

#pragma endregion
    };
}

#endif //TINYSTL_BITSET_H
