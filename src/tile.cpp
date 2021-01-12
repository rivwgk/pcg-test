#include "tile.h"

Tile::Tile(const Tile& other)
{
	for (size_t i=0; i<3; ++i)
		values[i] = other.values[i];
	for (size_t i=0; i<1; ++i)
		grad[i] = other.grad[i];
}

Tile&
Tile::operator=(const Tile& other)
{
	if (&other == this)
		return *this;

	for (size_t i=0; i<3; ++i)
		values[i] = other.values[i];
	for (size_t i=0; i<1; ++i)
		grad[i] = other.grad[i];

	return *this;
}
