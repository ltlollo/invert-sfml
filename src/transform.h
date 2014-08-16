#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <cmath>
#include <complex>
#include <memory>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "utils.h"
#include "workers.h"

namespace tr {

constexpr char title[]{"invert-sfml"};
constexpr unsigned aliasLvl{8}, quality{100};
constexpr bool show_time{true}, show_size{true}, show_center{true};

using Coord = sf::Vector2f;
using Cmplx = std::complex<float>;


inline Coord __complex_transform(const Coord center, const Coord p,
                                 const Cmplx a, const Cmplx b,
                                 const Cmplx c, const Cmplx d) noexcept;

template<typename T, typename P, typename... Args>
constexpr auto domesure(T&& msg, P&& f, Args&&... args) {
    return fun::mesure<show_time, T, Args...>(std::forward<T>(msg),
                                              std::forward<P>(f),
                                              std::forward<Args>(args)...);
}

class Drawable {
    std::vector<sf::Vertex> vertices;
    const unsigned x, y;
    const sf::Color BG{sf::Color::Black};
public:
    template<typename T>
    Drawable(T&& vertices, const unsigned x, const unsigned y);
    template<typename T>
    Drawable(T&& vertices, const unsigned x, const unsigned y,
             const sf::Color BG);
    void show() const;
    void save(const std::string& fname) const;
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
    Transformation(const sf::Image& png);
    Transformation(sf::Image&& png);
    Transformation(sf::Image&& png, std::vector<Coord>&& tmap);
    Transformation& transform(const TransParams& tp, Coord center);
    Transformation& invert(InvParams tp, Coord center);
    Transformation& transform(const TransParams& tp);
    Transformation& invert(InvParams tp);
    Transformation& invert();
    Drawable draw(const unsigned winx, const unsigned winy) const;
    Drawable draw() const;
};
}
#endif // TRANSFORM_H
