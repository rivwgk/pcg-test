#pragma once

#include <SFML/Graphics/Color.hpp>

extern sf::Color
greyscale(double value, double min, double max);
extern sf::Color
rainbow(double value, double min, double max);
extern sf::Color
temperature(double value, double min, double max);

struct HSVColour {
	float h; // \in [0,360[
	float s; // \in [0,100]
	float v; // \in [0,100]
};

// workaround for sf::Color not yet supporting constexpr
struct Colour {
	const unsigned r;
	const unsigned g;
	const unsigned b;

	operator sf::Color() const;
};

extern sf::Color
HSVtoRGB(const HSVColour& hsv);
