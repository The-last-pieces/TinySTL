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
     * 对象迭代器的默认实现
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
}

#endif //TINYSTL_ITERATOR_H
