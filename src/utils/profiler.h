#pragma once
#ifndef TINYSTL_PROFILER_H
#define TINYSTL_PROFILER_H

#include <chrono>
#include <iostream>

namespace ttl {
    using time_type = unsigned long long;

    /*
     * 基础计时器
     */
    class free_timer {
        std::chrono::system_clock::time_point stamp;
    public:
        void start() {
            stamp = std::chrono::system_clock::now();
        }

        time_type get_ns() {
            return std::chrono::duration_cast<std::chrono::nanoseconds>
                    (std::chrono::system_clock::now() - stamp).count();
        }
    };

    /*
     * 通用计时器
     */
    class auto_timer {
        free_timer timer;

    public:
        // start record when entering scope
        explicit auto_timer(const char *task_name = nullptr) {
            if (task_name) std::cout << task_name << " running , ";
            timer.start();
        }

        // end record when leaving scope
        ~auto_timer() {
            std::cout << "cost: " << double(timer.get_ns()) / 1000000.0 << " ms" << std::endl;
        }
    };
}  // namespace ttl

#endif
