#include "utils.h"

std::array<int, 3> parse_coord(const std::string& opt)
{
    std::array<int, 3> res;
    char sepa{','};
    auto it = std::begin(opt);
    for (unsigned i = 0; i < res.size()-1; ++i) {
        auto it_ = std::find(it, std::end(opt), sepa);
        if (it_ == std::end(opt)) {
            throw std::runtime_error("not enough args");
        }
        auto str = std::string(it, it_);
        if (str.empty()) {
            throw std::runtime_error("empty arg " + std::to_string(i));
        }
        res[i] = atoi(str.c_str());
        it = it_+sizeof(sepa);
    }
    if (std::find(it, std::end(opt), sepa) != std::end(opt)) {
        throw std::runtime_error("too much args");
    }
    if (it == std::end(opt)) {
        throw std::runtime_error("not enough args");
    }
    auto pos = res.size()-1;
    auto str = std::string(it, std::end(opt));
    if (str.empty()) {
        throw std::runtime_error("empty arg " + std::to_string(pos));
    }
    res[pos] = atoi(str.c_str());
    return res;
}
