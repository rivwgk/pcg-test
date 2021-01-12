/*
 */
#include "grid.h"
#include "tile.h"
#include "math/vector.hpp"
#include "math/interpolation.hpp"

#include <algorithm>
#include <random>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdlib>

Grid::Grid(int width, int height, bool zeroed/*=false*/) : m_width{width},
                                                           m_height{height}
{
	m_tiles = new Tile[m_height*m_width];
	if (zeroed)
		for (int y=0; y<m_height; ++y)
			for (int x=0; x<m_width; ++x) {
				(*this)(x,y).height = 0.0;
				(*this)(x,y).temperature = 0.0;
				(*this)(x,y).moisture = 0.0;
			}
}
Grid::Grid(const Grid& other) : m_width{other.m_width},
                                m_height{other.m_height}
{
	m_tiles = new Tile[other.m_height*other.m_width];
	for (int y=0; y<other.m_height; ++y)
		for (int x=0; x<other.m_width; ++x)
			(*this)(x,y) = other(x,y);
}
Grid::~Grid()
{
	delete[] m_tiles;
}
/* ----------------------------------------------------------------- */
Tile&
Grid::operator()(const int x,const int y)
{
	return m_tiles[field_index(x,y)];
}
/* ----------------------------------------------------------------- */
const Tile&
Grid::operator()(const int x,const int y)
const {
	return m_tiles[field_index(x,y)];
}
/* ----------------------------------------------------------------- */
int
Grid::height()
const { return m_height; }
/* ----------------------------------------------------------------- */
int
Grid::width()
const { return m_width; }
/* ----------------------------------------------------------------- */
bool
Grid::inside(const int x,const int y)
const {
	return 0<=y and y<m_height and 0<=x and x<m_width;
}
/* ----------------------------------------------------------------- */
const int
Grid::field_index(const int x,const int y)
const {
	if (y < 0) return field_index(x,m_height+y);
	if (y >= m_height) return field_index(x,y-m_height);
	if (x < 0) return field_index(m_width+x,y);
	if (x >= m_width) return field_index(x-m_width,y);
	return y*m_width+x;
}
/* ================================================================= *\
 * classic diamond square algorithm
 * TODO: documentation
 */
Grid
diamond_square(int exp,int xcells,int ycells,double wiggle)
{
	const int size = 1<<exp;
	const int width = size * xcells;
	const int height = size * ycells;
	const float damping = 0.75;
	std::mt19937 gen{std::random_device{}()};
	Grid grid{width, height};
	auto rand = [&](){ return std::generate_canonical<float,16>(gen); };
	auto diamond = [&](int x, int y, int s, float w, int i)-> void
	{
		grid(x*s+s/2,y*s+s/2).values[i] = std::clamp<double>(
		                            (grid(x*s,y*s).values[i] +
		                             grid(x*s,(y+1)*s).values[i] +
		                             grid((x+1)*s,y*s).values[i] +
		                             grid((x+1)*s,(y+1)*s).values[i])/4 +
		                             2*rand()*w-w, 0.0, 1.0);
	};
	auto square = [&](int x, int y, int s, float w, int i)-> void
	{
		grid(x*s,y*s).values[i] = std::clamp<float>(
		                             (grid((x-1)*s,y*s).values[i] +
		                              grid((x+1)*s,y*s).values[i] +
		                              grid(x*s,(y-1)*s).values[i] +
		                              grid(x*s,(y+1)*s).values[i])/4 +
		                              2*rand()*w-w, 0.0, 1.0);
	};
	// set each of the nodes' value on the initial coarse grid
	for (int y=0; y<=ycells; ++y)
		for (int x=0; x<=xcells; ++x) {
			grid(x*size,y*size).height = rand();
			grid(x*size,y*size).temperature = rand();
			grid(x*size,y*size).moisture = rand();
		}
	// refine the grid
	for (int cellsize=size; cellsize>1; ) {
		// diamond step
		for (int y=0; y<ycells; ++y)
			for (int x=0; x<xcells; ++x)
				diamond(x, y, cellsize, wiggle, 0);
		cellsize /= 2; ycells *= 2; xcells *= 2; wiggle *= damping;
		// square step
		for (int y=0; y<ycells; ++y) {
			if (y%2 == 0)
				for (int x=1; x<xcells; x+=2)
					square(x, y, cellsize, wiggle, 0);
			else
				for (int x=0; x<xcells; x+=2)
					square(x, y, cellsize, wiggle, 0);
		}
		wiggle *= damping;
	}
	return grid;
}
/* ================================================================= *\
 * value noise
 * \par cellsize the width and height of each cell
 * \par xcells how many cells on the x-axis
 * \par ycells how many cells on the y-axis
 * \par n_oct how many octaves of value noise should be used,
 *            default is $1$
 * \par f the interpolation function, default is $id$
 */
Grid
value_noise(int cellsize, int xcells, int ycells, int n_oct/*=1*/,
            const std::function<double(double)>& f/*=math::identity<double>*/)
{
	assert(n_oct < std::log2(cellsize));
	std::mt19937 gen{std::random_device{}()};
	auto rand = [&](){ return std::generate_canonical<float,16>(gen); };
	Grid grid{xcells*cellsize, ycells*cellsize, true};

	for (int k=0; k < n_oct; ++k) {
		const int csize = (cellsize>>k);
		for (int y=0; y < (ycells*(1<<k)); ++y)
			for (int x=0; x < (xcells*(1<<k)); ++x)
				grid(x*csize,y*csize).grad[0] = rand();

		for (int y=0; y < ycells*cellsize; ++y)
			for (int x=0; x < xcells*cellsize; ++x) {
				int xc = x / csize;
				int yc = y / csize;
				int u = x % csize;
				int v = y % csize;
				grid(x,y).values[0] += math::interpolate2d(
				         grid(xc*csize, yc*csize).grad[0],
				         grid(xc*csize, (yc+1)*csize).grad[0],
				         grid((xc+1)*csize, yc*csize).grad[0],
				         grid((xc+1)*csize, (yc+1)*csize).grad[0],
				         u,v,csize,f) / (1<<k);
			}
	}
	return grid;
}
/* ================================================================= *\
 * classic 2D perlin noise
 * \par cellsize the width and height of each cell
 * \par xcells how many cells on the x-axis
 * \par ycells how many cells on the y-axis
 * \par n_oct how many octaves of noise should be used,
 *            default is $1$
 */
Grid
gradient_noise(int cellsize, int xcells, int ycells, int n_oct/*=1*/)
{
	assert(n_oct < std::log2(cellsize));
	std::mt19937 gen{std::random_device{}()};
	auto rand = [&](){ return 2*std::generate_canonical<double,16>(gen)-1; };

	Grid grid{xcells*cellsize,ycells*cellsize};
	for (int y=0; y<ycells*cellsize; ++y)
		for (int x=0; x<xcells*cellsize; ++x)
			grid(x,y).height = 0.5;

	math::Vector<double,2> uv00, uv01, uv10, uv11;
	for (int k=0; k<n_oct; ++k) {
		const int csize = cellsize >> k;

		/* generate (new) random gradients for each cell */
		for (int y=0; y<(ycells*1<<k); ++y)
			for (int x=0; x<(xcells*1<<k); ++x) {
				grid(x*csize,y*csize).grad[0] = rand();
				grid(x*csize,y*csize).grad[1] = rand();
				math::normalize<double,2>(grid(x*csize,y*csize).grad);
			}

		for (int y=0; y < ycells*cellsize; ++y)
			for (int x=0; x < xcells*cellsize; ++x) {
				int xc = x / csize;
				int yc = y / csize;
				int u = x % csize;
				int v = y % csize;
				uv00[0]=1.0*u/csize  , uv00[1]=1.0*v/csize;
				uv01[0]=1.0*u/csize  , uv01[1]=1.0*v/csize-1;
				uv10[0]=1.0*u/csize-1, uv10[1]=1.0*v/csize;
				uv11[0]=1.0*u/csize-1, uv11[1]=1.0*v/csize-1;
				grid(x,y).height += math::interpolate2d(
				         math::dot<double,2>(grid(xc*csize,yc*csize).grad,uv00),
				         math::dot<double,2>(grid(xc*csize,(yc+1)*csize).grad,uv01),
				         math::dot<double,2>(grid((xc+1)*csize,yc*csize).grad,uv10),
				         math::dot<double,2>(grid((xc+1)*csize,(yc+1)*csize).grad,uv11),
				          u, v, csize, [](double t)-> double {return t*t*(-2*t+3);})
				          / (sqrt(2.0)*(1<<k));
			}
	}
	return grid;
}
/* ================================================================= *\
 * wavelet noise, for a detailed description see:
 * Cook, R., DeRose, T.: Wavelet Noise. ACM Trans. Graph., 24:803-811
 * 07 2005.
 * TODO
 */
Grid
wavelet_noise(int cellsize, int xcells, int ycells, int n_oct/*=1*/)
{
	assert(n_oct < std::log2(cellsize));
	std::mt19937 gen{std::random_device{}()};
	Grid grid{xcells*cellsize,ycells*cellsize};
	auto rand = [&](){ return -1+2*std::generate_canonical<double,16>(gen); };

	return grid;
}
/* ================================================================= *\
 * Agent based landscape generation based on:
 * Doran, J., Parberry, I.: Controlled procedural terrain generation
 * using software agents. IEEE Transactions on Computational Intelli-
 * gence and AI in Games 2(2), 111-119(2010)
 * TODO
 */
Grid
doran_parberry(int width,int height)
{
	return Grid{width,height};
}
/* ================================================================= *\
 * TODO
 */
void
classify_grid(Grid& grid)
{
}
