#include "invert.h" // inv::Inverter

using namespace std;
using namespace inv;

inline bool Inverter::outside_inv_circle(const Coord p) const noexcept
{
    return (p-center).modsq() > rsq;
}

inline double Inverter::dir(const Coord a,
                            const Coord b,
                            const Coord c
                            ) const noexcept
{
    return (a.x-c.x)*(b.y-c.y)-(b.x-c.x)*(a.y-c.y);
}

inline bool Inverter::in_trinagle(const Coord p, const Coord a,
                                  const Coord b, const Coord c
                                  ) const noexcept
{
    const bool t1{dir(p, a, b) < 0.0}, t2{dir(p, b, c) < 0.0},
    t3{dir(p, c, a) < 0.0};
    return (t1 == t2) && (t2 == t3);
}

inline bool Inverter::in_rectangle(const Coord p,
                                   const Coord a,
                                   const Coord b,
                                   const Coord c,
                                   const Coord d
                                   ) const noexcept
{
    const bool t1{in_trinagle(p, a, b, c)}, t2{in_trinagle(p, a, b, d)},
    t3{in_trinagle(p, b, c, d)};
    return t1 || t2 || t3;
}

inline Coord Inverter::invert_abs_coord(const Coord p) const noexcept
{
    const Coord rel{p-center};
    const double teta{get_angle(rel)}, inverted_radius{rsq/sqrt(rel.modsq())};
    return Coord(center.x + inverted_radius*cos(teta),
                 center.y + inverted_radius*sin(teta)
                 );
}

void Inverter::color_region(const Coord curr) noexcept
{
    const auto inside_border = [&](const Coord p)noexcept->bool
    {
        return ((p.x >= 0 && static_cast<size_t>(p.x) < new_png.get_width()) &&
                (p.y >= 0 && static_cast<size_t>(p.y) < new_png.get_height())
                );
    };

    if ((curr-center).modsq() >= gtmodsq_maps_outside)
    {
        const Coord a = invert_abs_coord(curr),
                b = invert_abs_coord(Coord(curr.x+delta, curr.y      )),
                c = invert_abs_coord(Coord(curr.x,       curr.y+delta)),
                d = invert_abs_coord(Coord(curr.x+delta, curr.y+delta));
        const auto min_max = move(min_max_x_y(a, b, c, d));
        const auto min_x = get<0>(min_max), max_x = get<1>(min_max),
                min_y = get<2>(min_max), max_y = get<3>(min_max);

        for (auto y = min_y; y <= max_y; ++y)
        {
            for (auto x = min_x; x <= max_x; ++x)
            {
                if (inside_border(Coord(x, y)) &&
                        in_rectangle(Coord(x, y), a, b, c, d)
                        )
                {
                    new_png[y][x].red = orig_png[curr.y][curr.x].red;
                    new_png[y][x].green = orig_png[curr.y][curr.x].green;
                    new_png[y][x].blue = orig_png[curr.y][curr.x].blue;
                }
            }
        }
    }
}

void Inverter::invert_points() noexcept
{
    for (size_t y{0}; y < orig_png.get_height()-delta; ++y)
        for (size_t x{0}; x < orig_png.get_width()-delta; ++x)
        {  // WARN +delta coordinates can and will be the center
            const Coord current(x, y);
            if (current != center &&
                    current != Coord(center.x, center.y-delta) &&
                    current != Coord(center.x-delta, center.y) &&
                    current != Coord(center.x-delta, center.y-delta)
                    ) color_region(current);
        }
}

size_t Inverter::find_max_radiussq() const noexcept
{
    const size_t max_x = static_cast<size_t>(center.x) >=
            (orig_png.get_width()-1)/2 ? center.x : orig_png.get_width()-1 - center.x;
    const size_t max_y = static_cast<size_t>(center.y) >=
            (orig_png.get_height()-1)/2 ? center.y : orig_png.get_height()-1 - center.y;
    const size_t max = max_x >= max_y ? max_x : max_y;
    return rsq*rsq/(2*max*max);
}

Inverter::Inverter(const std::string& iname, const std::string& oname,
                   const Coord center, const int radius
                   )
    : iname(iname), oname(oname), orig_png(iname),
      new_png(orig_png.get_width(), orig_png.get_height()),
      radius(radius), center{center},
      rsq(radius*radius), gtmodsq_maps_outside{find_max_radiussq()}
{
    if (orig_png.get_height() < 8 + delta ||
            orig_png.get_height() < 8 + delta
            ) throw(runtime_error("picture too small"));
    if (radius < 0) throw(runtime_error("negative radius"));
    if (center.x < 0 || static_cast<size_t>(center.x) >= orig_png.get_width() ||
        center.y < 0 || static_cast<size_t>(center.y) >= orig_png.get_height()
            ) throw(runtime_error("inversion circle outiside the image"));
}

Inverter::Inverter(const std::string& iname,
                   const Coord center,
                   const int radius
                   )
    : Inverter(iname, iname + "-out.png", center, radius)
{}

Inverter::Inverter(const std::string& iname)
    : Inverter(iname, iname + "-out.png", Coord(0,0), 0)
{
    center = Coord(orig_png.get_width()/2, orig_png.get_height()/2);
    set_radius(center.x > center.y ? center.y/2 : center.x/2);
}

void Inverter::run()
{
    invert_points();
    new_png.write(oname);
}

void Inverter::operator()()
{
    run();
}

void Inverter::set_center(const Coord p)
{
    center = p;
    gtmodsq_maps_outside = find_max_radiussq();
}

void Inverter::set_radius(const int d)
{
    radius = d;
    rsq = d*d;
    gtmodsq_maps_outside = find_max_radiussq();
}

Coord Inverter::get_center() const noexcept
{
    return center;
}

int Inverter::get_radius() const noexcept
{
    return radius;
}
