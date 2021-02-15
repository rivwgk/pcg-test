#pragma once

#include "math/vector.hpp"
#include "colourscales.h"

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
	Mountain,
	Glacier,
	Deep_Ocean,
	Ocean,
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

	static constexpr Colour BiomeColours[static_cast<char>(BiomeType::_size)]={
		Colour{0xca, 0xfa, 0xff}, // Nival_Desert
		Colour{0x8e, 0xc7, 0xc7}, // Nival_Shrubland
		Colour{0x18, 0x9a, 0x51}, // Nival_Forest
		Colour{0x97, 0xf5, 0xff}, // Nival_Coast
		Colour{0x53, 0x9c, 0x48}, // Temperate_Forest
		Colour{0xd1, 0xce, 0x65}, // Temperate_Coast
		Colour{0xed, 0xcc, 0x54}, // Temperate_Desert
		Colour{0x85, 0xd7, 0x13}, // Temperate_Shrubland
		Colour{0x5b, 0x6f, 0x2f}, // Temperate_Swamp
		Colour{0x06, 0x78, 0x00}, // Tropical_Forest
		Colour{0xdc, 0xd6, 0x00}, // Tropical_Coast
		Colour{0xfd, 0xeb, 0x31}, // Tropical_Desert
		Colour{0x69, 0xcd, 0x22}, // Tropical_Shrubland
		Colour{0x11, 0x4b, 0x0e}, // Tropical_Swamp
		Colour{0x99, 0x99, 0x99}, // Mountain
		Colour{0xca, 0xde, 0xe0}, // Glacier
		Colour{0x00, 0x28, 0x70}, // Deep_Ocean
		Colour{0x00, 0x28, 0xa0}, // Ocean
		Colour{0x00, 0x50, 0xb0}, // Shallow_Ocean
		Colour{0x38, 0x7f, 0xb5}, // River
	};
};
