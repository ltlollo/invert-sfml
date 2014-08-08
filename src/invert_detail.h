#ifndef INVERT_DETAIL_H
#define INVERT_DETAIL_H

#include <type_traits>   // std::is_integral

namespace detail_inv {
constexpr double pi{3.14159265358979323846}, radDeg{180/pi};
template<typename T> struct CoordGeneric {
    T x, y;
    constexpr CoordGeneric(const T x, const T y) : x(x), y(y) {
        static_assert(std::is_integral<T>::value, "Non integral type.");
    }
    bool operator==(const CoordGeneric p) const noexcept {
        return x == p.x && y == p.y;
    }
    bool operator!=(const CoordGeneric p) const noexcept {
        return x != p.x || y != p.y;
    }
    CoordGeneric operator-(const CoordGeneric rhs) const noexcept {
        return CoordGeneric(x-rhs.x, y-rhs.y);
    }
    T modsq() const noexcept {
        return x*x + y*y;
    }
};

}

#endif // INVERT_DETAIL_H
