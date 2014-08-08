#include "invert.h"

using namespace std;
using namespace inv;
using namespace sf;

inline Coord Inverter::invert_abs_coord(const Coord p) const noexcept {
    const Coord rel{p-center};
    const double teta{atan2(rel.y, rel.x)},
    inverted_radius{rsq/sqrt(rel.x*rel.x+rel.y*rel.y)};
    return Coord(center.x + inverted_radius*cos(teta),
                 center.y + inverted_radius*sin(teta));
}

void Inverter::color_region(const Coord curr) noexcept {
    if ((curr.x-center.x)*(curr.x-center.x)+(curr.y-center.y)*(curr.y-center.y)
        >= gtmodsq_maps_outside) {
        const Coord p = invert_abs_coord(Coord(curr.x, curr.y)),
                a = invert_abs_coord(Coord(curr.x-1, curr.y-1)), // ab
                b = invert_abs_coord(Coord(curr.x, curr.y-1)),   // cp
                c = invert_abs_coord(Coord(curr.x-1, curr.y));
        Vertex vp, va, vb, vc;
        vp.position = p; vp.color = orig_png.getPixel(curr.x, curr.y);
        va.position = a; va.color = orig_png.getPixel(curr.x-1, curr.y-1);
        vb.position = b; vb.color = orig_png.getPixel(curr.x, curr.y-1);
        vc.position = c; vc.color = orig_png.getPixel(curr.x-1, curr.y);

        vertices.push_back(vb);
        vertices.push_back(vc);
        vertices.push_back(va);

        if (quality>0) {
            vertices.push_back(va);
            vertices.push_back(vp);
            vertices.push_back(vb);
        }

        vertices.push_back(vb);
        vertices.push_back(vc);
        vertices.push_back(vp);

        if (quality>0) {
            vertices.push_back(va);
            vertices.push_back(vp);
            vertices.push_back(vc);
        }
    }
}

void Inverter::invert_points() noexcept {
    for (size_t x{1}; x < orig_png.getSize().x; ++x) {
        for (size_t y{1}; y < orig_png.getSize().y; ++y) {
            if (x != center.x+1 || x != center.x-1 ||
                y != center.y+1 || y != center.y-1 ||
                x != center.x   || y != center.y)  {
                color_region(Coord(x, y));
            }
        }
    }
}

size_t Inverter::find_max_radiussq() const noexcept {
    const size_t max_x = center.x >=
                         (orig_png.getSize().x-1)/2 ?
                             center.x : orig_png.getSize().x-1 - center.x;
    const size_t max_y = center.y >=
                         (orig_png.getSize().y-1)/2 ?
                             center.y : orig_png.getSize().y-1 - center.y;
    const size_t max = max_x >= max_y ? max_x : max_y;
    return rsq*rsq/(2*(max+2)*(max+2));
}

Color Inverter::get_background() const noexcept {
    const size_t x  = orig_png.getSize().x/2, y = orig_png.getSize().x/2;
    const auto ca = orig_png.getPixel(0, y),
            cb = orig_png.getPixel(x, 0),
            cc = orig_png.getPixel(orig_png.getSize().x-1, y),
            cd = orig_png.getPixel(x, orig_png.getSize().y-1);
    Color avg(ca.r/4+cb.r/4+cc.r/4+cd.r/4,
              ca.g/4+cb.g/4+cc.g/4+cd.g/4,
              ca.b/4+cb.b/4+cc.b/4+cd.b/4);
    return avg;
}

Inverter::Inverter(const string& iname, const string& oname,
                   const Coord center, const int radius, const bool show)
    : iname(iname), oname(oname), radius(radius), center(center),
      rsq(radius*radius), settings(0, 0, aliasLvl, 3, 0), show{show} {
    orig_png.loadFromFile(iname);
    gtmodsq_maps_outside = find_max_radiussq();
}

Inverter::Inverter(const string& iname, const bool show)
    :  Inverter(iname, iname+"-out.png", show)
{}

Inverter::Inverter(const string& iname, const Coord center,
                   const int radius, const bool show)
    : Inverter(iname, iname+"-out.png", center, radius, show)
{}

Inverter::Inverter(const string& iname, const string& oname,
                   const bool show)
    : Inverter(iname, oname, Coord(0,0), 0, show) {
    center.x = orig_png.getSize().x/2;
    center.y = orig_png.getSize().y/2;
    radius = (center.x > center.y ? center.y/2 : center.x/2);
    rsq = radius*radius;
    gtmodsq_maps_outside = find_max_radiussq();
}

void Inverter::run() {
    VideoMode vmode(orig_png.getSize().x, orig_png.getSize().y);
    window.create(vmode, title);
    window.create(vmode, title);    // HACK: sfml needs this to work properly
    window.clear(get_background());
    window.setVisible(show);
    invert_points();
    for (size_t i{0} ; i < vertices.size()/3; ++i) {
        window.draw(&vertices[i*3], 3, Triangles);
    }
    if (show) {
        window.setFramerateLimit(20);
        while (window.isOpen()) {
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                } else if (event.type == Event::KeyPressed) {
                    switch (event.key.code) {
                    case Keyboard::Q:
                    case Keyboard::Escape:
                        window.close();
                        break;
                    default:
                        break;
                    }
                }
            }
            window.display();
        }
    } else {
        window.capture().saveToFile(oname);
    }
}

void Inverter::set_center(const Coord p) noexcept {
    center = p;
    gtmodsq_maps_outside = find_max_radiussq();
}

Coord Inverter::get_center() const noexcept {
    return center;
}

void Inverter::set_radius(const int d) noexcept {
    radius = d;
    rsq = d*d;
}

int Inverter::get_radius() const noexcept {
    return radius;
}
