#pragma once

#include "math/interpolation.hpp"
#include "tile.h"

class Grid {
public:
	Grid() = delete;
	Grid(int width,int height,bool zeroed=false);
	Grid(const Grid& other);
	~Grid();

	Tile& operator()(const int x,const int y);
	const Tile& operator()(const int x,const int y) const;
	int width() const;
	int height() const;
	bool inside(const int x,const int y) const;
private:
	const int field_index(const int x,const int y) const;
	const int m_width,m_height;
	Tile* m_tiles;
};

Grid diamond_square(int exp, int xcells, int ycells, float wiggle);
Grid value_noise(int cellsize, int xcells ,int ycells, int n_oct=1,
                 const std::function<double(double)>&f=math::identity<double>);
Grid gradient_noise(int cellsize, int xcells, int ycells, int n_oct=1);
Grid wavelet_noise(int cellsize, int xcells, int ycells, int n_oct=1);
