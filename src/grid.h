#pragma once

#include "math/interpolation.hpp"
#include "math/utils.hpp"
#include "tile.h"

class Grid {
public:
	Grid() = delete;
	Grid(long height,
        long width,
        Realfield2D heightmap,
	     Realfield2D temperature,
        Realfield2D humidity);
	Grid(const Grid& other);
	Grid& operator=(const Grid& other);

	long
   height() const;
	long
   width() const;

	const Realfield2D&
   heightmap() const;
	const Realfield2D&
   temperature() const;
	const Realfield2D&
   humidity() const; 
	const Field2D<BiomeType>&
   types() const;

private:
	void classify();

	long m_height, m_width;
	Realfield2D m_heightmap, m_temperature, m_humidity;
	Field2D<BiomeType> m_type;
};

Realfield2D
diamond_square(long exp,
               long xcells, 
               long ycells,
               double wiggle);

Realfield2D
value_noise(long cellsize,
            long xcells, 
            long ycells,
            long n_oct=1,
            const R2RFunction& f = math::identity<double>);

Realfield2D
gradient_noise(long cellsize,
               long xcells,
               long ycells,
               long n_oct=1);
