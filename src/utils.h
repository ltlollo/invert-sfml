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

namespace fun {

template<typename T> T getnum(const char*) noexcept;
template<typename T> T getnum(const std::string& str) noexcept {
    return getnum<T>(str.c_str());
}

template<typename T> void getnum(const char*, T&) noexcept;
template<typename T> void getnum(const std::string& str, T& num) noexcept {
    getnum(str.c_str(), num);
}

template<unsigned N, typename T, char sepa = ','>
std::array<T, N> parse_coord(const std::string& opt) {
    static_assert(N > 0, "cannot parse zero arguments");
    static_assert(sepa == ','|| sepa == ':' || sepa == ';' || sepa == '#',
                  "unsupported separator");
    std::array<T, N> res;
    auto it = std::begin(opt);
    for (unsigned i = 0; i < N-1; ++i) {
        auto it_ = std::find(it, std::end(opt), sepa);
        if (it_ == std::end(opt)) {
            throw std::runtime_error("not enough args");
        }
        auto str = std::string(it, it_);
        if (str.empty()) {
            throw std::runtime_error("empty arg " + std::to_string(i));
        }
        getnum(str, res[i]);
        it = it_+sizeof(sepa);
    }
    if (std::find(it, std::end(opt), sepa) != std::end(opt)) {
        throw std::runtime_error("too much args");
    }
    if (it == std::end(opt)) {
        throw std::runtime_error("not enough args");
    }
    auto pos = N-1;
    auto str = std::string(it, std::end(opt));
    if (str.empty()) {
        throw std::runtime_error("empty arg " + std::to_string(pos));
    }
    getnum(str, res[pos]);
    return res;
}

template<typename T>
std::enable_if_t<std::is_signed<T>::value, std::make_unsigned_t<T>
> absolute(const T x) {
    return x > 0 ? x : -x;
}

std::string filename_from_path(const std::string& path);

} // namespace fun
#endif // UTILS_H
