#pragma once

#include <SFML/Graphics.hpp>
#include "math/vector.hpp"

enum class BiomeType: char {
	Nival_Desert,
	Nival_Shrubland,
	Nival_Forest,
	Nival_Coast,
	Temperate_Forest,
	Temperate_Coast,
	Temperate_Desert,
	Temperate_Shrubland,
	Temperate_Swamp,
	Tropical_Forest,
	Tropical_Coast,
	Tropical_Desert,
	Tropical_Shrubland,
	Tropical_Swamp,
	Mountains,
	Deep_Ocean,
	Shallow_Ocean,
	River,
	_size,
};

struct Tile {
	Tile() = default;
	Tile(const Tile& other);
	Tile& operator=(const Tile& other);

	union {
		struct {
			double height;
			double temperature;
			double moisture;
		};
		double values[3];
	};
	math::Vector<double,2> grad;
	BiomeType type;

 //static sf::Color BiomeColours[static_cast<char>(BiomeType::_Last)] = {
 //   sf::Color{200,200,220},
 //};
};
