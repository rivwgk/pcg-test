#pragma once

#include "math/interpolation.hpp"
#include "math/matrix.hpp"
#include "tile.h"

class Grid {
public:
	Grid() = delete;
	Grid(size_t height, size_t width, math::Matrix<double> heightmap,
	     math::Matrix<double> temperature, math::Matrix<double> humidity);
	Grid(const Grid& other);
	Grid& operator=(const Grid& other);

	size_t height() const;
	size_t width() const;

	const math::Matrix<double>& heightmap() const;
	const math::Matrix<double>& temperature() const;
	const math::Matrix<double>& humidity() const;
	const math::Matrix<BiomeType>& types() const;

private:
	void classify();

	size_t m_height, m_width;
	math::Matrix<double> m_heightmap, m_temperature, m_humidity;
	math::Matrix<BiomeType> m_type;
};

math::Matrix<double> diamond_square(size_t exp, size_t xcells, size_t ycells,
                                    double wiggle);
math::Matrix<double> value_noise(size_t cellsize, size_t xcells, 
                                 size_t ycells, size_t n_oct=1,
                                 const std::function<double(double)>&f=math::identity<double>);
math::Matrix<double> gradient_noise(size_t cellsize, size_t xcells,
                                    size_t ycells, size_t n_oct=1);
