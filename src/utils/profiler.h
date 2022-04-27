#pragma once
#ifndef TINYSTL_PROFILER_H
#define TINYSTL_PROFILER_H

#include <chrono>
#include <iostream>

namespace ttl {

    /*
     * 通用计时器
     */
    class auto_timer {
        const char *const task_name;
        std::chrono::system_clock::time_point start;

    public:
        // start record when entering scope
        explicit auto_timer(const char *task_name = nullptr) :
                task_name(task_name), start(std::chrono::system_clock::now()) {
        }

        // end record when leaving scope
        ~auto_timer() {
            auto cost = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start);
            if (task_name) std::cout << task_name << ' ';
            std::cout << "cost: " << double(cost.count()) / 1000000.0 << " ms" << std::endl;
        }
    };

}  // namespace ttl

#endif
