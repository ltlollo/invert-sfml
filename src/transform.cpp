#include "transform.h"

namespace tr {

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

void Drawable::paint(sf::RenderWindow& window) const {
    domesure("painting", [&]() {
        for (size_t i{0} ; i < vertices.size()/3; ++i) {
            window.draw(&vertices[i*3], 3, sf::Triangles);
        }
    });
}

template<typename T>
Drawable::Drawable(T&& vertices, const unsigned x, const unsigned y)
    : vertices{std::forward<T>(vertices)}, vmode(x, y) {
}

template<typename T>
Drawable::Drawable(T&& vertices, const unsigned x, const unsigned y,
                   const sf::Color BG)
    : vertices{std::forward<T>(vertices)}, vmode(x, y), BG{BG} {
}

void Drawable::show() const &&{
    sf::Event event;
    sf::RenderWindow window;
    window.create(vmode, title);
    window.create(vmode, title);    // HACK
    window.clear(BG);
    paint(window);
    window.setFramerateLimit(20);
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Q:
                case sf::Keyboard::Escape:
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

void Drawable::save(const std::string& fname) const &&{
    sf::RenderWindow window;
    window.create(vmode, title);
    window.create(vmode, title);    // HACK
    window.clear(BG);
    paint(window);
    domesure("saving", [&]() {
        window.capture().saveToFile(fname);
    });
}

Transformation::Transformation(const sf::Image& png) : png{png} {
    auto size = png.getSize();
    for (size_t y{0}; y < size.y; ++y) {
        for (size_t x{0}; x < size.x; ++x) {
            tmap.push_back(Coord(x, y));
        }
    }
}

Transformation::Transformation(sf::Image&& png) : png{std::move(png)} {
    auto size = png.getSize();
    for (size_t y{0}; y < size.y; ++y) {
        for (size_t x{0}; x < size.x; ++x) {
            tmap.push_back(Coord(x, y));
        }
    }
}

Transformation::Transformation(sf::Image&& png, std::vector<Coord>&& tmap)
    : png{std::move(png)}, tmap{std::move(tmap)} {
}

Transformation& Transformation::transform(const TransParams& tp, Coord center) {
    const Coord center_ = center;
    const Cmplx a_ = tp.a, b_ = tp.b, c_ = tp.c, d_ = tp.d;
    std::function<Coord(const Coord)> fun =
            [center_, a_, b_, c_, d_](const Coord p) noexcept {
        return __complex_transform(center_, p, a_, b_, c_, d_);
    };
    auto result = work::static_work_balancer(tmap, fun);
    std::swap(tmap, result);
    return *this;
}

Transformation& Transformation::invert(InvParams tp, Coord center) {
    const Coord center_ = center;
    const size_t rsq = tp.radius*tp.radius;
    std::function<Coord(const Coord)> fun =
            [center_, rsq](const Coord p) noexcept {
        return __invert_transform(p, center_, rsq);
    };
    auto result = work::static_work_balancer(tmap, fun);
    std::swap(tmap, result);
    return *this;
}

Transformation& Transformation::invert(InvParams tp) {
    const Coord center{(float)png.getSize().x/2, (float)png.getSize().y/2};
    return invert(tp, center);
}


Transformation& Transformation::invert() {
    const unsigned x = png.getSize().x, y = png.getSize().y;
    const unsigned radius = (x > y ? y/4 : y/4);
    return invert({radius});
}


Transformation& Transformation::transform(const TransParams& tp) {
    const Coord center{(float)png.getSize().x/2, (float)png.getSize().y/2};
    return transform(tp, center);
}


static inline sf::Color getBG(const sf::Image& png) noexcept {
    const size_t x  = png.getSize().x/2, y = png.getSize().x/2;
    auto ca = png.getPixel(0, y),
            cb = png.getPixel(x, 0),
            cc = png.getPixel(png.getSize().x-1, y),
            cd = png.getPixel(x, png.getSize().y-1);
    sf::Color avg(ca.r/4+cb.r/4+cc.r/4+cd.r/4,
                  ca.g/4+cb.g/4+cc.g/4+cd.g/4,
                  ca.b/4+cb.b/4+cc.b/4+cd.b/4);
    return avg;
}

static inline bool outside(const Coord p, const unsigned x, const unsigned y)
noexcept {
    return p.x < 0 || p.x > x || p.y < 0 || p.y > y;
}


static inline bool inside(const Coord p, const unsigned x, const unsigned y)
noexcept {
    return !(outside(p, x, y));
}

Drawable Transformation::draw(const unsigned winx, const unsigned winy) const {
    std::vector<sf::Vertex> vertices;
    domesure("transforming", [&]() {
        for (size_t y{1}; y < png.getSize().y; ++y) {
            for (size_t x{1}; x < png.getSize().x; ++x) {
                auto a = tmap[x-1+(y-1)*png.getSize().x];
                auto b = tmap[x+(y-1)*png.getSize().x];
                auto c = tmap[x-1+y*png.getSize().x];
                auto d = tmap[x+y*png.getSize().x];
                sf::Vertex va(a, png.getPixel(x-1, y-1)),
                        vb(b, png.getPixel(x, y-1)),
                        vc(c, png.getPixel(x-1, y)),
                        vd(d, png.getPixel(x, y));

                if (inside(b, winx, winy) || inside(c, winx, winy) ||
                    inside(a, winx, winy)) {
                    vertices.push_back(vb);
                    vertices.push_back(vc);
                    vertices.push_back(va);
                }
                if (quality &&
                    (inside(a, winx, winy) || inside(d, winx, winy) ||
                     inside(b, winx, winy))) {
                    vertices.push_back(va);
                    vertices.push_back(vd);
                    vertices.push_back(vb);
                }
                if (inside(b, winx, winy) || inside(c, winx, winy) ||
                    inside(d, winx, winy)) {
                    vertices.push_back(vb);
                    vertices.push_back(vc);
                    vertices.push_back(vd);
                }
                if (quality &&
                    (inside(a, winx, winy) || inside(d, winx, winy) ||
                     inside(c, winx, winy))) {
                    vertices.push_back(va);
                    vertices.push_back(vd);
                    vertices.push_back(vc);
                }
            }
        }
    });
    return Drawable{std::move(vertices), winx, winy, getBG(png)};
}

Drawable Transformation::draw() const {
    return draw(png.getSize().x, png.getSize().y);
}

} // namespace tr
