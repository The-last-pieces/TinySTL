#pragma once
#ifndef MY_PROFILER_H
#define MY_PROFILER_H

#include <chrono>
#include <iostream>

namespace tstd {

    using namespace std::chrono;

    /*
     * 通用计时器
     */
    class auto_timer {
        system_clock::time_point start;

    public:
        // start record when entering scope
        auto_timer() : start(system_clock::now()) {}

        // end record when leaving scope
        ~auto_timer() {
            auto cost = duration_cast<nanoseconds>(system_clock::now() - start);
            std::cout << "Time cost: " << double(cost.count()) / 1000000.0 << " ms" << std::endl;
        }
    };

}  // namespace tstd

#endif  // !MY_PROFILER_H
