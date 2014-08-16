#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <cmath>
#include <complex>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <work/workers.h>

#include "utils.h"

namespace tr {

constexpr unsigned aliasLvl{8};
constexpr bool show_time{true}, show_size{true}, show_center{true},
quality{true};

using Coord = sf::Vector2f;
using Cmplx = std::complex<float>;

template<typename T, typename P, typename... Args>
constexpr auto domesure(T&& msg, P&& f, Args&&... args) {
    return fun::mesure<show_time, T, Args...>(std::forward<T>(msg),
                                              std::forward<P>(f),
                                              std::forward<Args>(args)...);
}

class Drawable {
    std::vector<sf::Vertex> vertices;
    sf::VideoMode vmode;
    const sf::Color BG{sf::Color::Black};

    void paint(sf::RenderWindow& window) const;

public:
    template<typename T>
    Drawable(T&& vertices, const unsigned x, const unsigned y);
    template<typename T>
    Drawable(T&& vertices, const unsigned x, const unsigned y,
             const sf::Color BG);
    void show() const;
    void show(const std::string title) const;
    void save(const std::string& fname) const &&;
};

struct TransParams {
    const Cmplx a, b, c, d;
};

struct InvParams {
    const size_t radius;
};

class Transformation {
    sf::Image png;
    std::vector<Coord> tmap;

public:
    template<typename T>
    Transformation(T&& png);
    Transformation(sf::Image&& png, std::vector<Coord>&& tmap);
    Transformation& transform(const TransParams& tp, Coord center);
    Transformation& invert(InvParams tp, Coord center);
    Transformation& transform(const TransParams& tp);
    Transformation& invert(InvParams tp);
    Transformation& invert();
    Drawable draw(const unsigned winx, const unsigned winy) const;
    Drawable draw() const;
};

template<typename T>
Transformation::Transformation(T&& png) : png{std::forward<T>(png)} {
    auto size = png.getSize();
    for (size_t y{0}; y < size.y; ++y) {
        for (size_t x{0}; x < size.x; ++x) {
            tmap.push_back(Coord(x, y));
        }
    }
}
}
#endif // TRANSFORM_H
