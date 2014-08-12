#include "invert.h"

using namespace std;
using namespace inv;
using namespace sf;

static inline Coord __complex_transform(const Coord center, const Coord p,
                                        const Cmplx a, const Cmplx b,
                                        const Cmplx c, const Cmplx d) noexcept {
    const Cmplx rel(p.x-center.x,p.y-center.y);
    auto res = (a*rel+b)/(c*rel+d);
    return Coord(center.x+res.real(),center.y+res.imag());
}

static inline Coord __invert_transform(const Coord p, const Coord center,
                                       const size_t rsq) noexcept {
    const Coord rel{p-center};
    const double teta{atan2(rel.y, rel.x)},
    inverted_radius{rsq/sqrt(rel.x*rel.x+rel.y*rel.y)};
    return Coord(center.x + inverted_radius*cos(teta),
                 center.y + inverted_radius*sin(teta));
}

static inline Coord __another_trasform(const Coord p, const Coord center,
                                        const size_t rsq) noexcept {
    const Coord rel{p-center};
    const double teta{atan2(rel.y, rel.x)},
    inverted_radius{rsq/sqrt(rel.x*rel.x+rel.y*rel.y)};
    return Coord(center.x + inverted_radius*tanh(teta),
                 center.y + inverted_radius/tan(teta));
}

void Inverter::reset_tmap() noexcept {
    for (auto& v: tmap) {
        for (auto& it : v) {
            it = Coord(0, 0);
        }
    }
}

string Inverter::get_title() const {
    return iname +
            (show_size ? " size: " + to_string(orig_png.getSize().x) + 'x' +
                         to_string(orig_png.getSize().y):"") +
            (show_center ? " center: " + to_string((int)center.x) + ',' +
                           to_string((int)center.y):"");
}

bool Inverter::outside_image(const Coord p) const noexcept {
    return p.x + delta < 0 || p.x - delta > orig_png.getSize().x ||
            p.y + delta < 0 || p.y - delta > orig_png.getSize().y;
}

void Inverter::transform() {
    const Coord center_ = center;
    const Cmplx a_ = a, b_ = b, c_ = c, d_ = d;
    function<Coord(const Coord)> fun =
            [center_, a_, b_, c_, d_](const Coord p) noexcept {
        return __complex_transform(center_, p, a_, b_, c_, d_);
    };
    vector<Coord> data;
    for (size_t y{0}; y < orig_png.getSize().y; ++y) {
        for (size_t x{0}; x < orig_png.getSize().x; ++x) {
            data.push_back(Coord(x, y));
        }
    }
    auto result = work::static_work_balancer(data, fun);

    for (size_t i{0}; i < data.size(); ++i) {
        tmap[data[i].y][data[i].x] = result[i];
    }
    Vertex vp, va, vb, vc;
    for (size_t y{1}; y < orig_png.getSize().y; ++y) {
        for (size_t x{1}; x < orig_png.getSize().x; ++x) {

            if (outside_image(tmap[y][x])   || outside_image(tmap[y][x-1]) ||
                outside_image(tmap[y-1][x]) || outside_image(tmap[y-1][x-1])) {
                continue;
            }

            vp = tmap[y][x];
            vp.color = orig_png.getPixel(x, y);
            va.position = tmap[y-1][x-1];
            va.color = orig_png.getPixel(x-1, y-1);
            vb.position = tmap[y-1][x];
            vb.color = orig_png.getPixel(x, y-1);
            vc.position = tmap[y][x-1];
            vc.color = orig_png.getPixel(x-1, y);


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
}

void Inverter::invert() {
    const Coord center_ = center;
    const size_t rsq_ = rsq;
    function<Coord(const Coord)> fun =
            [center_, rsq_](const Coord p) noexcept {
        return __invert_transform(p, center_, rsq_);
    };
    vector<Coord> data;
    for (size_t y{0}; y < orig_png.getSize().y; ++y) {
        for (size_t x{0}; x < orig_png.getSize().x; ++x) {
            if (x != center.x || y != center.y ||
                (x-center.x)*(x-center.x)+(y-center.y)*(y-center.y) >=
                gtmodsq_maps_outside-1) {
                data.push_back(Coord(x, y));
            }
        }
    }
    auto result = work::static_work_balancer(data, fun);

    for (size_t i{0}; i < data.size(); ++i) {
        tmap[data[i].y][data[i].x] = result[i];
    }
    Vertex vp, va, vb, vc;
    for (size_t y{1}; y < orig_png.getSize().y; ++y) {
        for (size_t x{1}; x < orig_png.getSize().x; ++x) {
            if ((x != center.x   || y != center.y    ||
                 x != center.x+1 || y != center.y+1) &&
                (x-center.x)*(x-center.x)+(y-center.y)*(y-center.y) >=
                gtmodsq_maps_outside) {
                vp = tmap[y][x];
                vp.color = orig_png.getPixel(x, y);
                va.position = tmap[y-1][x-1];
                va.color = orig_png.getPixel(x-1, y-1);
                vb.position = tmap[y-1][x];
                vb.color = orig_png.getPixel(x, y-1);
                vc.position = tmap[y][x-1];
                vc.color = orig_png.getPixel(x-1, y);

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
    }
}

size_t Inverter::find_max_radiussq() const noexcept {
    const size_t max_x = center.x >= (orig_png.getSize().x-1)/2 ?
                             center.x : orig_png.getSize().x-1 - center.x;
    const size_t max_y = center.y >= (orig_png.getSize().y-1)/2 ?
                             center.y : orig_png.getSize().y-1 - center.y;
    const size_t max = max_x >= max_y ? max_x : max_y;
    return rsq*rsq/(2*(max+2)*(max+2));
}

Color Inverter::get_background() const noexcept {
    const size_t x  = orig_png.getSize().x/2, y = orig_png.getSize().x/2;
    const auto
            ca = orig_png.getPixel(0, y),
            cb = orig_png.getPixel(x, 0),
            cc = orig_png.getPixel(orig_png.getSize().x-1, y),
            cd = orig_png.getPixel(x, orig_png.getSize().y-1);
    Color avg(ca.r/4+cb.r/4+cc.r/4+cd.r/4,
              ca.g/4+cb.g/4+cc.g/4+cd.g/4,
              ca.b/4+cb.b/4+cc.b/4+cd.b/4);
    return avg;
}

void Inverter::show_image() {
    window.setTitle(get_title());
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
}

Inverter::Inverter(const string& iname, const string& oname,
                   const Coord center, const int radius, const bool show)
    : iname(iname), oname(oname), radius(radius), center(center),
      rsq(radius*radius), settings(0, 0, aliasLvl, 3, 0), show{show},
      invert_not_transform{true} {
    orig_png.loadFromFile(iname);
    gtmodsq_maps_outside = find_max_radiussq();
    tmap = vector<vector<Coord>>(orig_png.getSize().y, vector<Coord
                                 >(orig_png.getSize().x,Coord(0, 0)));
}

Inverter::Inverter(const string& iname, const string& oname,
                   const Cmplx a, const Cmplx b, const Cmplx c, const Cmplx d,
                   bool show)
    : iname(iname), oname(oname), a{a}, b{b}, c{c}, d{d},
      settings(0, 0, aliasLvl, 3, 0), show{show},
      invert_not_transform{false} {
    orig_png.loadFromFile(iname);
    tmap = vector<vector<Coord>>(orig_png.getSize().y, vector<Coord
                                 >(orig_png.getSize().x,Coord(0, 0)));
    center.x = orig_png.getSize().x/2;
    center.y = orig_png.getSize().y/2;
}

Inverter::Inverter(const string& iname, const bool show)
    :  Inverter(iname, iname+"-out.png", show)
{}

Inverter::Inverter(const string& iname,
                   const Cmplx a, const Cmplx b, const Cmplx c, const Cmplx d,
                   const bool show)
    : Inverter(iname, iname+"-out.png", a, b, c, d, show)
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
    if (invert_not_transform) {
        domesure("inverting", [&](){
            invert();
        });
    } else {
        domesure("transforming", [&](){
            transform();
        });
    }
    domesure("drawing", [&]() {
        for (size_t i{0} ; i < vertices.size()/3; ++i) {
            window.draw(&vertices[i*3], 3, Triangles);
        }
    });
    if (show) {
        show_image();
    } else {
        domesure("saving", [&]() {
            window.capture().saveToFile(oname);
        });
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
