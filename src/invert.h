#ifndef INVERT_H
#define INVERT_H

#include <cmath>
#include <complex>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "utils.h"
#include "workers.h"

namespace inv {

constexpr char title[]{"invert-sfml"};
constexpr unsigned aliasLvl{8}, quality{100};
constexpr bool show_time{true}, show_size{true}, show_center{true};

using Coord = sf::Vector2f;
using Cmplx = std::complex<float>;

template<typename T, typename P, typename... Args>
constexpr auto domesure(T&& msg, P&& f, Args&&... args) {
    return fun::mesure<show_time, T, Args...>(std::forward<T>(msg),
                                              std::forward<P>(f),
                                              std::forward<Args>(args)...);
}

class Transform {
private:
    std::vector<std::vector<Coord>> tmap;
    const std::string iname, oname;
    sf::Image orig_png;
    int radius;
    Coord center;
    Cmplx a, b, c, d;
    std::size_t rsq;
    std::size_t gtmodsq_maps_outside;
    const sf::ContextSettings settings;
    sf::RenderWindow window;
    sf::Event event;
    std::vector<sf::Vertex> vertices;
    const bool show{false}, invert_not_transform{true};

    std::size_t find_max_radiussq() const noexcept;
    sf::Color get_background() const noexcept;
    void show_image();
    void invert();
    void transform();
    void reset_tmap() noexcept;
    std::string get_title() const;
    bool outside_image(const Coord p) const noexcept;

public:
    Transform(const std::string& iname, const std::string& oname,
              const Coord center, const int radius, bool show=false);
    Transform(const std::string& iname, const std::string& oname,
              const Cmplx a, const Cmplx b, const Cmplx c, const Cmplx d,
              bool show=false);
    Transform(const std::string& iname, const Cmplx a, const Cmplx b,
              const Cmplx c, const Cmplx d, const bool show = false);
    Transform(const std::string& iname, const Coord center, const int radius,
              const bool show = false);
    Transform(const std::string& iname, const std::string& oname,
              const bool show = false);
    explicit Transform(const std::string& iname, const bool show = false);
    void run() &&;
    void set_center(const Coord p) noexcept;
    Coord get_center() const noexcept;
    void set_radius(const int d) noexcept;
    int get_radius() const noexcept;
};
}
#endif // INVERT_H
