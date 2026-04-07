//
// Created by ronan2140 on 4/7/26.
//

#ifndef LOSTINSPACE_TIMER_HPP
#define LOSTINSPACE_TIMER_HPP



#include <iostream>
#include <chrono>
#include <string>

class Timer {
public:
    explicit Timer(std::string name)
        : m_name(std::move(name)), m_start(std::chrono::steady_clock::now()) {}

    ~Timer() {
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start).count();
        std::cout << "[TIMER] " << m_name << " took " << duration << " ms" << std::endl;
    }

private:
    std::string m_name;
    std::chrono::time_point<std::chrono::steady_clock> m_start;
};


#endif //LOSTINSPACE_TIMER_HPP
