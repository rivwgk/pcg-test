#pragma once

#include <SFML/Graphics/Color.hpp>

extern sf::Color
greyscale(double value, double min, double max);
extern sf::Color
rainbow(double value, double min, double max);
extern sf::Color
temperature(double value, double min, double max);

struct HSVColor {
   float h; // \in [0,360[
   float s; // \in [0,100]
   float v; // \in [0,100]
};

extern sf::Color
HSVtoRGB(const HSVColor& hsv);
