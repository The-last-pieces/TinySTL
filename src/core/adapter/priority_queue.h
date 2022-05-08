//
// Created by IMEI on 2022/5/5.
//

#ifndef TINYSTL_PRIORITY_QUEUE_H
#define TINYSTL_PRIORITY_QUEUE_H

#include "../container/vector.h"
#include "../algorithm/algorithm.h"

namespace ttl {
    template<
            typename T,
            typename Container = ttl::vector<T>,
            typename Compare = std::less<typename Container::value_type>>
    class priority_queue {
    public:
        using container_type = Container;
        using value_compare = Compare;
        using value_type = typename Container::value_type;
        using size_type = typename Container::size_type;
        using reference = typename Container::reference;
        using const_reference = typename Container::const_reference;
    private:
        container_type arr;
        Compare comp;
    public: // constructor
        priority_queue() : priority_queue(Compare(), Container()) {}

        explicit priority_queue(const Compare &compare)
                : priority_queue(compare, Container()) {}

        priority_queue(const Compare &compare, const Container &cont) // NOLINT(modernize-pass-by-value)
                : comp(compare), arr(cont) {
            ttl::make_heap(arr.begin(), arr.end(), comp);
        }

        priority_queue(const Compare &compare, Container &&cont)
                : comp(compare), arr(std::forward<Container>(cont)) {
            ttl::make_heap(arr.begin(), arr.end(), comp);
        }

        priority_queue(const priority_queue &other) = default;

        priority_queue(priority_queue &&other) noexcept = default;

        template<class InputIt>
        priority_queue(InputIt first, InputIt last, const Compare &compare = Compare())
                : priority_queue(compare, Container(first, last)) {}

        ~priority_queue() = default;

        priority_queue &operator=(const priority_queue &other) = default;

        priority_queue &operator=(priority_queue &&other) noexcept = default;

    public: // visit
        const_reference top() const { return arr.front(); }

    public: // capacity
        bool empty() const { return arr.empty(); }

        size_type size() const { return arr.size(); }

    private:
        void after_push() { ttl::push_heap(arr.begin(), arr.end(), comp); }

    public: // change
        void push(const value_type &value) {
            arr.push_back(value), after_push();
        }

        void push(value_type &&value) {
            arr.push_back(std::forward<value_type>(value)), after_push();
        }

        template<class... Args>
        void emplace(Args &&... args) {
            arr.emplace_back(std::forward<value_type>(args)...), after_push();
        }

        void pop() {
            ttl::pop_heap(arr.begin(), arr.end(), comp), arr.pop_back();
        }

        T poll() {
            T tmp = top();
            return pop(), tmp;
        }

        void swap(priority_queue &other) noexcept {
            std::swap(arr, other.arr);
            std::swap(comp, other.comp);
        }
    };
}

#endif //TINYSTL_PRIORITY_QUEUE_H
