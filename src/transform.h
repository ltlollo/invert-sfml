#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <cmath>
#include <complex>

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include <extra/task.h>
#include <extra/utils.h>
#include "utils.h"

namespace tr {

constexpr bool show_time{false}, show_size{true}, quality{true};
using Coord = sf::Vector2f;
using Cmplx = std::complex<float>;

class Drawable {
    std::vector<sf::Vertex> vertices;
    const sf::VideoMode vmode;
    const sf::Color BG{sf::Color::Black};

    void paint(sf::RenderWindow& window) const;

public:
    template<typename T>
    Drawable(T&& vertices, const unsigned x, const unsigned y);
    template<typename T>
    Drawable(T&& vertices, const unsigned x, const unsigned y,
             const sf::Color BG);
    void show() const;
    void show(const std::string& title) const;
    void save(const std::string& fname) const &&;
};

struct TransParams {
    Cmplx a, b, c, d;
    TransParams fromArray(const std::array<float, 8>& arr) && {
        a = {arr[0], arr[1]}; b = {arr[2], arr[3]}; c = {arr[4], arr[5]};
        d = {arr[6], arr[7]};
        return *this;
    }
};

struct InvParams {
    size_t radius;
};

class Transformation {
    sf::Image png;
    std::vector<Coord> tmap;

public:
    Transformation(sf::Image&& png);
    Transformation(const sf::Image& png);
    Transformation(const std::string& file);
    Transformation(sf::Image&& png, std::vector<Coord>&& tmap);
    Transformation& transform(const TransParams& tp, Coord center);
    Transformation& transform(const TransParams& tp);
    Transformation& invert(InvParams tp, Coord center);
    Transformation& invert(InvParams tp);
    Transformation& invert();
    Drawable draw(const unsigned winx, const unsigned winy) const;
    Drawable draw() const;
};
}
#endif // TRANSFORM_H
