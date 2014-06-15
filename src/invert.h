#ifndef INVERT_H
#define INVERT_H

#include <stdexcept>       // std::runtime_error
#include <tuple>           // std::tuple
#include <cmath>           // std::atan2, std::sincos [long]double->T
#include <png++/png.hpp>   // png::image
#include "invert_detail.h" // detail_inv::CoordGeneric

namespace
{
typedef detail_inv::CoordGeneric<int> Coord;
constexpr int delta{2};
} // end of anonymous namespace

namespace inv
{

class Inverter
{
private:
    const std::string iname, oname;
    const png::image<png::rgb_pixel> orig_png;
    mutable png::image<png::rgb_pixel> new_png;
    int radius;
    Coord center;
    std::size_t rsq;
    std::size_t gtmodsq_maps_outside;

    bool outside_inv_circle(const Coord p) const noexcept;
    double dir(const Coord a, const Coord b, const Coord c) const noexcept;
    bool in_trinagle(const Coord p,
                     const Coord a,
                     const Coord b,
                     const Coord c
                    ) const noexcept;

    bool in_rectangle(const Coord p,
                      const Coord a,
                      const Coord b,
                      const Coord c,
                      const Coord d
                     ) const noexcept;
    template<typename T> std::tuple<T, T, T, T>
    min_max_x_y(const detail_inv::CoordGeneric<T> a,
                const detail_inv::CoordGeneric<T> b,
                const detail_inv::CoordGeneric<T> c,
                const detail_inv::CoordGeneric<T> d
               ) const noexcept;
    template<typename T> double
    get_angle(const detail_inv::CoordGeneric<T> rel_p) const noexcept;
    Coord invert_abs_coord(const Coord p) const noexcept;
    template <typename T> T avg_pixel(T f, T s) const noexcept;
    void color_region(const Coord curr) noexcept;
    void invert_points() noexcept;
    std::size_t find_max_radiussq() const noexcept;

public:
    Inverter(const std::string& iname, const std::string& oname,
             const Coord center, const int radius
            );
    Inverter(const std::string& iname, const Coord center, const int radius);
    explicit Inverter(const std::string& iname);
    void run();
    void operator()();
    void set_center(const Coord p); // TODO: make setters safe/sane
    void set_radius(const int d);
    Coord get_center() const noexcept;
    int get_radius() const noexcept;
};

#include "invert_impl.h"

} // end of inv namespace

#endif // INVERT_H
