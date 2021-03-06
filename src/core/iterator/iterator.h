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
     * tag萃取
     */
    template<typename T>
    struct iterator_tag_traits {
    };
#pragma region
    template<>
    struct iterator_tag_traits<input_iterator_tag> {
        using type = input_iterator_tag;
    };
    template<>
    struct iterator_tag_traits<std::input_iterator_tag> {
        using type = input_iterator_tag;
    };
    template<>
    struct iterator_tag_traits<output_iterator_tag> {
        using type = input_iterator_tag;
    };
    template<>
    struct iterator_tag_traits<std::output_iterator_tag> {
        using type = output_iterator_tag;
    };
    template<>
    struct iterator_tag_traits<forward_iterator_tag> {
        using type = forward_iterator_tag;
    };
    template<>
    struct iterator_tag_traits<std::forward_iterator_tag> {
        using type = forward_iterator_tag;
    };
    template<>
    struct iterator_tag_traits<bidirectional_iterator_tag> {
        using type = bidirectional_iterator_tag;
    };
    template<>
    struct iterator_tag_traits<std::bidirectional_iterator_tag> {
        using type = bidirectional_iterator_tag;
    };
    template<>
    struct iterator_tag_traits<random_access_iterator_tag> {
        using type = random_access_iterator_tag;
    };
    template<>
    struct iterator_tag_traits<std::random_access_iterator_tag> {
        using type = random_access_iterator_tag;
    };
#pragma endregion

    /*
     * 迭代器的类型萃取
     */
    template<typename T>
    struct iterator_traits {
        using iterator_category = typename iterator_tag_traits<typename T::iterator_category>::type;
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
        normal_iterator(const normal_iterator<Iter, Container> it) noexcept: // NOLINT(google-explicit-constructor)
                current(const_cast<pointer>(it.base())) {}

        normal_iterator(normal_iterator &&) noexcept = default;

        normal_iterator &operator=(const normal_iterator &) noexcept = default;

        ~normal_iterator() = default;

    public: // ops
        reference operator*() const noexcept { return *current; }

        pointer operator->() const noexcept { return current; }

        // forward
        normal_iterator &operator++() noexcept {
            return ++current, *this;
        }

        normal_iterator operator++(int) noexcept {
            return normal_iterator(current++);
        }

        // bi direct
        normal_iterator &operator--() noexcept {
            return --current, *this;
        }

        normal_iterator operator--(int) noexcept {
            return normal_iterator(current--);
        }

        // random
        reference operator[](difference_type n) const noexcept {
            return *(current + n);
        }

        normal_iterator &operator+=(difference_type n) noexcept {
            return current += n, *this;
        }

        normal_iterator operator+(difference_type n) const noexcept {
            return normal_iterator(current + n);
        }

        normal_iterator &operator-=(difference_type n) noexcept {
            return current -= n, *this;
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
     * 将迭代器适配为逆序迭代器
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
            return *--iterator_type(current);
        }

        pointer operator->() const noexcept {
            return cast_to_pointer(--iterator_type(current));
        }

        // forward
        reverse_iterator &operator++() noexcept {
            return --current, *this;
        }

        reverse_iterator operator++(int) noexcept {
            return reverse_iterator(current--);
        }

        // bi direct
        reverse_iterator &operator--() noexcept {
            return ++current, *this;
        }

        reverse_iterator operator--(int) noexcept {
            return reverse_iterator(current++);
        }

        // random
        reference operator[](difference_type n) const noexcept {
            return *(*this + n);
        }

        reverse_iterator &operator+=(difference_type n) noexcept {
            return current -= n, *this;
        }

        reverse_iterator operator+(difference_type n) const noexcept {
            return reverse_iterator(current - n);
        }

        reverse_iterator &operator-=(difference_type n) noexcept {
            return current += n, *this;
        }

        reverse_iterator operator-(difference_type n) const noexcept {
            return reverse_iterator(current + n);
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

    /*
     * 迭代器操作
     */

    namespace {
        template<typename InputIterator, typename Distance>
        inline void advance_imp(InputIterator &i, Distance n, input_iterator_tag) {
            while (n--) ++i;
        }

        template<typename BidirectionalIterator, typename Distance>
        inline void advance_imp(BidirectionalIterator &i, Distance n, bidirectional_iterator_tag) {
            if (n > 0) while (n--) ++i;
            else while (n++) --i;
        }

        template<typename RandomAccessIterator, typename Distance>
        inline void advance_imp(RandomAccessIterator &i, Distance n, random_access_iterator_tag) {
            i += n;
        }

        template<typename InputIterator>
        typename ttl::iterator_traits<InputIterator>::difference_type
        distance_imp(InputIterator first, InputIterator last, input_iterator_tag) {
            typename ttl::iterator_traits<InputIterator>::difference_type ret = 0;
            while (first != last) ++first, ++ret;
            return ret;
        }

        template<typename RandomAccessIterator>
        typename ttl::iterator_traits<RandomAccessIterator>::difference_type
        distance_imp(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
            return last - first;
        }
    }

    template<typename InputIt>
    typename ttl::iterator_traits<InputIt>::difference_type
    distance(InputIt first, InputIt last) {
        return distance_imp(first, last, typename iterator_traits<InputIt>::iterator_category());
    }

    template<typename InputIt, typename Distance>
    void advance(InputIt &it, Distance n) {
        ttl::advance_imp(it, n, typename iterator_traits<InputIt>::iterator_category());
    }

    template<typename InputIt>
    InputIt next(InputIt it, typename std::iterator_traits<InputIt>::difference_type n = 1) {
        ttl::advance(it, n);
        return it;
    }

    template<class BidirIt>
    BidirIt prev(BidirIt it, typename std::iterator_traits<BidirIt>::difference_type n = 1) {
        ttl::advance(it, -static_cast<std::make_signed_t<typename std::iterator_traits<BidirIt>::difference_type>>(n));
        return it;
    }

}

#endif //TINYSTL_ITERATOR_H
