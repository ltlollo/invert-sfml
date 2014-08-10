#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <chrono>
#include <array>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <algorithm>

#include <unistd.h>

template<bool enable, typename T, typename P, typename... Args>
constexpr typename std::enable_if<enable>::type mesure(T&& msg, P&& f,
                                                       Args&&... args) {
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();
    f(std::forward<Args>(args)...);
    end = std::chrono::high_resolution_clock::now();
    std::cerr << msg << " took: "
              << std::chrono::duration_cast<
                 std::chrono::milliseconds
                 >(end - start).count()
              << "ms\n";
}
template<bool enable, typename T, typename P, typename... Args>
constexpr typename std::enable_if<!enable>::type mesure(T&& msg, P&& f,
                                                        Args&&... args) {
    f(std::forward<Args>(args)...);
}

std::array<int, 3> parse_coord(const std::string& opt);

#endif // UTILS_H
