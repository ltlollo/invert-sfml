#include "utils.h"

namespace fun {

template<> void getnum<int>(const char* str, int& num) {
    num = atoi(str);
}

template<> void getnum<float>(const char* str, float& num) {
    num = atof(str);
}

template<>  void getnum<long>(const char* str, long& num) {
    num = atol(str);
}

std::string relative_filepath(const std::string& path) {
    if (path.empty()) {
        throw std::runtime_error("empty path");
    } else if (path[path.size()-1] == '/') {
        throw std::runtime_error("not a file");
    }
    auto pos = std::find(path.rbegin(), path.rend(), '/');
    if (pos == path.rend()) {
        return path;
    }
    auto len = std::distance(pos, path.rend());
    return std::string(path.begin()+len, path.end());
}
} // namespace fun
