//
// Created by IMEI on 2022/4/26.
//

#ifndef TINYSTL_ITERATOR_H
#define TINYSTL_ITERATOR_H

#include <cstddef>

namespace ttl {
    /*
     * 迭代器标签
     */
    struct input_iterator_tag {
    };
    struct output_iterator_tag {
    };
    struct forward_iterator_tag : public input_iterator_tag {
    };
    struct bidirectional_iterator_tag : public forward_iterator_tag {
    };
    struct random_access_iterator_tag : public bidirectional_iterator_tag {
    };

    /*
     * 迭代器的类型萃取
     */
    template<typename T>
    struct iterator_traits {
        using iterator_category = typename T::iterator_category;
        using value_type = typename T::value_type;
        using difference_type = typename T::difference_type;
        using pointer = typename T::pointer;
        using reference = typename T::reference;
    };
    template<typename T>
    struct iterator_traits<T *> {
        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;
    };
    template<typename T>
    struct iterator_traits<const T *> {
        using iterator_category = random_access_iterator_tag;
        using value_type = const T;
        using difference_type = ptrdiff_t;
        using pointer = const T *;
        using reference = const T &;
    };

    /*
     * 对象迭代器的默认声明
     */
    template<typename Category,
            typename T,
            typename Distance = ptrdiff_t,
            typename Pointer = T *,
            typename Reference = T &>
    struct iterator {
        using iterator_category = Category;
        using value_type = T;
        using difference_type = Distance;
        using pointer = Pointer;
        using reference = Reference;
    };

    /*
     * 一般迭代器的默认实现
     */
    template<typename Iterator, typename Container>
    class normal_iterator {
    protected:
        Iterator current;

        using traits_type = iterator_traits<Iterator>;
    public:
        using iterator_type = Iterator;
        using iterator_category = typename traits_type::iterator_category;
        using value_type = typename traits_type::value_type;
        using difference_type = typename traits_type::difference_type;
        using reference = typename traits_type::reference;
        using pointer = typename traits_type::pointer;
    public: // constructor
        normal_iterator() noexcept: current{} {}

        explicit normal_iterator(const Iterator &it) noexcept: current(it) {}

        normal_iterator(const normal_iterator &it) noexcept: current(it.current) {}

        template<typename Iter>
        explicit normal_iterator(
                const normal_iterator<Iter, typename std::enable_if<
                        std::is_same_v<Iter, typename Container::pointer>, Container
                >::type> it) noexcept: current(it.base()) {}

        normal_iterator(normal_iterator &&) noexcept = default;

        normal_iterator &operator=(const normal_iterator &) noexcept = default;

        ~normal_iterator() = default;

    public: // ops
        reference operator*() const noexcept { return *current; }

        pointer operator->() const noexcept { return current; }

        // forward
        normal_iterator &operator++() noexcept {
            ++current;
            return *this;
        }

        normal_iterator operator++(int) noexcept {
            return normal_iterator(current++);
        }

        // bi direct
        normal_iterator &operator--() noexcept {
            --current;
            return *this;
        }

        normal_iterator operator--(int) noexcept {
            return normal_iterator(current--);
        }

        // random
        reference operator[](difference_type n) const noexcept {
            return current[n];
        }

        normal_iterator &operator+=(difference_type n) noexcept {
            current += n;
            return *this;
        }

        normal_iterator operator+(difference_type n) const noexcept {
            return normal_iterator(current + n);
        }

        normal_iterator &operator-=(difference_type n) noexcept {
            current -= n;
            return *this;
        }

        normal_iterator operator-(difference_type n) const noexcept {
            return normal_iterator(current - n);
        }

        const Iterator &base() const noexcept { return current; }

        friend bool operator==(const normal_iterator &lhs, const normal_iterator &rhs) {
            return lhs.current == rhs.current;
        }

        friend bool operator!=(const normal_iterator &lhs, const normal_iterator &rhs) {
            return rhs.current != lhs.current;
        }

        friend bool operator<(const normal_iterator &lhs, const normal_iterator &rhs) {
            return lhs.current < rhs.current;
        }

        friend bool operator>(const normal_iterator &lhs, const normal_iterator &rhs) {
            return lhs.current > rhs.current;
        }

        friend bool operator<=(const normal_iterator &lhs, const normal_iterator &rhs) {
            return lhs.current <= rhs.current;
        }

        friend bool operator>=(const normal_iterator &lhs, const normal_iterator &rhs) {
            return lhs.current >= rhs.current;
        }

        friend difference_type operator-(const normal_iterator &lhs, const normal_iterator &rhs) {
            return lhs.current - rhs.current;
        }

        friend normal_iterator operator+(difference_type n, const normal_iterator &rhs) {
            return rhs + n;
        }
    };

    /*
     * 将迭代器保证为逆序迭代器
     */
    template<typename Iterator>
    class reverse_iterator : public ttl::iterator<
            typename ttl::iterator_traits<Iterator>::iterator_category,
            typename ttl::iterator_traits<Iterator>::value_type,
            typename ttl::iterator_traits<Iterator>::difference_type,
            typename ttl::iterator_traits<Iterator>::pointer,
            typename ttl::iterator_traits<Iterator>::reference> {

    protected:
        Iterator current;

        using traits_type = ttl::iterator_traits<Iterator>;
    public:
        using iterator_type = Iterator;
        using iterator_category = typename traits_type::iterator_category;
        using value_type = typename traits_type::value_type;
        using difference_type = typename traits_type::difference_type;
        using reference = typename traits_type::reference;
        using pointer = typename traits_type::pointer;
    public:
        reverse_iterator() : current() {};

        explicit reverse_iterator(iterator_type x) : current(x) {};

        reverse_iterator(const reverse_iterator &oth) : current(oth.current) {};

        reverse_iterator &operator=(const reverse_iterator &oth) {
            current = oth.current;
            return *this;
        }

    private:
        static pointer cast_to_pointer(pointer t) { return t; }

        template<typename T>
        static pointer cast_to_pointer(T t) { return t.operator->(); }


    public: // ops
        reference operator*() const noexcept {
            iterator_type temp = current;
            return *--temp;
        }

        pointer operator->() const noexcept {
            iterator_type temp = current;
            --temp;
            return cast_to_pointer(temp);
        }

        // forward
        reverse_iterator &operator++() noexcept {
            --current;
            return *this;
        }

        reverse_iterator operator++(int) noexcept {
            return normal_iterator(current--);
        }

        // bi direct
        reverse_iterator &operator--() noexcept {
            ++current;
            return *this;
        }

        reverse_iterator operator--(int) noexcept {
            return normal_iterator(current++);
        }

        // random
        reference operator[](difference_type n) const noexcept {
            return *(*this + n);
        }

        reverse_iterator &operator+=(difference_type n) noexcept {
            current -= n;
            return *this;
        }

        reverse_iterator operator+(difference_type n) const noexcept {
            return normal_iterator(current - n);
        }

        reverse_iterator &operator-=(difference_type n) noexcept {
            current += n;
            return *this;
        }

        reverse_iterator operator-(difference_type n) const noexcept {
            return normal_iterator(current + n);
        }

        const Iterator &base() const noexcept { return current; }

    public: // compares
        friend bool operator==(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            return lhs.current == rhs.current;
        }

        friend bool operator!=(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            return !(rhs == lhs);
        }

        friend bool operator<(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            return rhs.current < lhs.current;
        }

        friend bool operator>(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            return rhs < lhs;
        }

        friend bool operator<=(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            return !(rhs < lhs);
        }

        friend bool operator>=(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            return !(lhs < rhs);
        }

        friend difference_type operator-(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            return rhs.current - lhs.current;
        }

        friend reverse_iterator operator+(difference_type n, const reverse_iterator &rhs) {
            return rhs - n;
        }

    };
}

#endif //TINYSTL_ITERATOR_H
