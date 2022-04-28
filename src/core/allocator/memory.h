#pragma once
#ifndef TINYSTL_MEMORY_H
#define TINYSTL_MEMORY_H

#include <memory>
/*
 * 内存相关的函数
 */
namespace ttl {
    /*
     * 简易内存配置器
     */
    template<typename T>
    class allocator {
    public:
        // 类型成员
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        // 重新绑定
        template<class U>
        struct rebind {
            typedef allocator<U> other;
        };
    public:
        // 默认函数
        allocator() = default;

        allocator(const allocator<T> &) = default;

        template<typename U>
        explicit allocator(const allocator<U> &) {}

        ~allocator() = default;

    public:
        // 获取一个对象的真实地址(即使重载了&运算符)
        static pointer address(reference x) {
            return reinterpret_cast<pointer>(&const_cast<char &>
            (reinterpret_cast<const volatile char &>(x)));
        }

        static const_pointer address(const_reference x) {
            return reinterpret_cast<const_pointer>(&const_cast<char &>
            (reinterpret_cast<const volatile char &>(x)));
        }

        // 分配足够储存n个T对象的内存片段,不进行构造
        static pointer allocate(size_type n) {
            if (max_size() / sizeof(T) < n) {
                throw std::bad_array_new_length();
            }
            auto ptr = static_cast<pointer>(::operator new(n * sizeof(T)));
            if (ptr == nullptr) {
                throw std::bad_alloc();
            }
            return ptr;
        }

        // 回收allocate分配的地址
        static void deallocate(pointer p, size_type) {
            ::operator delete(p);
        }

        // 最大可分配长度
        constexpr static size_type max_size() {
            return std::numeric_limits<size_type>::max() / sizeof(T);
        }

        // 进行布置构造
        template<class U, class... Args>
        static void construct(U *p, Args &&... args) {
            new(static_cast<const void *>(p)) T(std::forward<Args>(args)...);
        }

        // 进行主动析构
        template<class U>
        static void destroy(U *p) {
            p->~U();
        }
    };

    /*
     * 未初始化内存上的操作
     * Todo POD类型的优化
     * Todo char*的优化 memmove
     */
    template<typename InputIt, typename NoThrowForwardIt>
    NoThrowForwardIt uninitialized_copy(InputIt first, InputIt last, NoThrowForwardIt result) {
        using T = typename std::iterator_traits<InputIt>::value_type;
        NoThrowForwardIt current = result;
        try {
            for (; first != last; ++first, ++current) {
                ::new(const_cast<void *>
                      (static_cast<const volatile void *>
                        (std::addressof(*current)))) T(*first);
            }
            return current;
        } catch (...) {
            for (; result != current; ++result) {
                result->~T();
            }
            throw;
        }
    }

    template<typename InputIt, typename NoThrowForwardIt>
    NoThrowForwardIt uninitialized_copy_n(InputIt first, size_t n, NoThrowForwardIt result) {
        using T = typename std::iterator_traits<InputIt>::value_type;
        NoThrowForwardIt current = result;
        try {
            for (; n > 0; ++first, ++current, --n) {
                ::new(const_cast<void *>
                      (static_cast<const volatile void *>
                        (std::addressof(*current)))) T(*first);
            }
            return current;
        } catch (...) {
            for (; result != current; ++result) {
                result->~T();
            }
            throw;
        }
    }

    template<typename InputIt, typename NoThrowForwardIt>
    NoThrowForwardIt uninitialized_move(InputIt first, InputIt last, NoThrowForwardIt result) {
        using T = typename std::iterator_traits<InputIt>::value_type;
        NoThrowForwardIt current = result;
        try {
            for (; first != last; ++first, ++current) {
                ::new(const_cast<void *>
                      (static_cast<const volatile void *>
                        (std::addressof(*current)))) T(std::move(*first));
            }
            return current;
        } catch (...) {
            for (; result != current; ++result) {
                result->~T();
            }
            throw;
        }
    }

    template<typename InputIt, typename NoThrowForwardIt>
    NoThrowForwardIt uninitialized_move_n(InputIt first, size_t n, NoThrowForwardIt result) {
        using T = typename std::iterator_traits<InputIt>::value_type;
        NoThrowForwardIt current = result;
        try {
            for (; n > 0; ++first, ++current, --n) {
                ::new(const_cast<void *>
                      (static_cast<const volatile void *>
                        (std::addressof(*current)))) T(std::move(*first));
            }
            return current;
        } catch (...) {
            for (; result != current; ++result) {
                result->~T();
            }
            throw;
        }
    }

    template<typename ForwardIt, typename T>
    ForwardIt uninitialized_fill(ForwardIt first, ForwardIt last, const T &x) {
        using V = typename std::iterator_traits<ForwardIt>::value_type;
        ForwardIt current = first;
        try {
            for (; current != last; ++current) {
                ::new(const_cast<void *>
                      (static_cast<const volatile void *>
                        (std::addressof(*current)))) V(x);
            }
            return current;
        } catch (...) {
            for (; first != current; ++first) {
                first->~V();
            }
            throw;
        }
    }

    template<typename ForwardIt, typename T>
    ForwardIt uninitialized_fill_n(ForwardIt first, size_t n, const T &x) {
        using V = typename std::iterator_traits<ForwardIt>::value_type;
        ForwardIt current = first;
        try {
            for (; n > 0; ++current, --n) {
                ::new(const_cast<void *>
                      (static_cast<const volatile void *>
                        (std::addressof(*current)))) V(x);
            }
            return current;
        } catch (...) {
            for (; first != current; ++first) {
                first->~V();
            }
            throw;
        }
    }

    template<typename ForwardIt>
    ForwardIt uninitialized_default_construct(ForwardIt first, ForwardIt last) {
        using V = typename std::iterator_traits<ForwardIt>::value_type;
        ForwardIt current = first;
        try {
            for (; current != last; ++current) {
                ::new(const_cast<void *>
                      (static_cast<const volatile void *>
                        (std::addressof(*current)))) V();
            }
            return current;
        } catch (...) {
            for (; first != current; ++first) {
                first->~V();
            }
            throw;
        }
    }

    template<typename ForwardIt>
    ForwardIt uninitialized_default_construct_n(ForwardIt first, size_t n) {
        using V = typename std::iterator_traits<ForwardIt>::value_type;
        ForwardIt current = first;
        try {
            for (; n > 0; ++current, --n) {
                ::new(const_cast<void *>
                      (static_cast<const volatile void *>
                        (std::addressof(*current)))) V();
            }
            return current;
        } catch (...) {
            for (; first != current; ++first) {
                first->~V();
            }
            throw;
        }
    }

    template<typename T>
    void destroy_at(T *x) {
        x->~T();
    }

    template<typename ForwardIt>
    void destroy(ForwardIt first, ForwardIt last) {
        using V = typename std::iterator_traits<ForwardIt>::value_type;
        if constexpr(std::is_pod<V>::value) {
            return;
        }
        for (; first != last; ++first) ttl::destroy_at(ttl::allocator<V>::address(*first));
    }
}  // namespace ttl

#endif