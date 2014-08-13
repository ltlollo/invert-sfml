#ifndef INVERTNMP_H
#define INVERTNMP_H

#include <cmath>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "invert.h"

namespace inv {

class InverterNMP {
private:
    const std::string iname, oname;
    sf::Image orig_png;
    int radius;
    Coord center;
    std::size_t rsq;
    std::size_t gtmodsq_maps_outside;
    const sf::ContextSettings settings;
    sf::RenderWindow window;
    sf::Event event;
    std::vector<sf::Vertex> vertices;
    const bool show{false};

    inline Coord invert_abs_coord(const Coord p) const noexcept;
    void color_region(const Coord curr) noexcept;
    void invert_points() noexcept;
    std::size_t find_max_radiussq() const noexcept;
    sf::Color get_background() const noexcept;
    void show_image();

public:
    InverterNMP(const std::string& iname, const std::string& oname,
             const Coord center, const int radius, bool show=false);
    InverterNMP(const std::string& iname, const Coord center, const int radius,
             const bool show = false);
    InverterNMP(const std::string& iname, const std::string& oname,
             const bool show = false);
    explicit InverterNMP(const std::string& iname, const bool show = false);
    void run();
    void set_center(const Coord p) noexcept;
    Coord get_center() const noexcept;
    void set_radius(const int d) noexcept;
    int get_radius() const noexcept;
};
} // namespace inv
#endif // INVERTNMP_H
