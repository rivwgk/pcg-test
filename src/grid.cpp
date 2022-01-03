/*
 */
#include "grid.h"
#include "tile.h"
#include "math/interpolation.hpp"
#include "math/utils.hpp"

#include <algorithm>
#include <random>
#include <cassert>
#include <cmath>
#include <cstdlib>

Grid::Grid(long height, long width, Realfield2D heightmap,
           Realfield2D temperature, Realfield2D humidity)
 : m_height{height}, m_width{width}, m_heightmap{heightmap},
   m_temperature{temperature}, m_humidity{humidity}, m_type{height,width}
{
	classify();
}
Grid::Grid(const Grid& other)
 : m_height{other.m_height}, m_width{other.m_width},
	m_heightmap{other.m_heightmap}, m_temperature{other.m_temperature},
	m_humidity{other.m_humidity}, m_type{other.m_type}
{
}
Grid&
Grid::operator=(const Grid& other)
{
	if (&other == this)
		return *this;
	m_height = other.m_height;
	m_width = other.m_width;
	m_heightmap = other.m_heightmap;
	m_temperature = other.m_temperature;
	m_humidity = other.m_humidity;
	m_type = other.m_type;
	return *this;
}

long
Grid::height()
const {
	return m_height;
}

long
Grid::width()
const {
	return m_width;
}

const Realfield2D&
Grid::heightmap()
const {
	return m_heightmap;
}

const Realfield2D&
Grid::temperature()
const {
	return m_temperature;
}

const Realfield2D&
Grid::humidity()
const {
	return m_humidity;
}

const Field2D<BiomeType>&
Grid::types()
const {
	return m_type;
}

/* =================================================================
 * classic diamond square algorithm
 * TODO: documentation
 */
Realfield2D
diamond_square(long exp, long xcells, long ycells, double wiggle)
{
	const long size = 1<<exp;
	const long width = size * xcells;
	const long height = size * ycells;
	const double damping = 0.70;
	std::mt19937 gen{std::random_device{}()};
	Realfield2D m{height, width};
	auto rand = [&](){ return std::generate_canonical<double,28>(gen); };
	auto diamond = [&](long x, long y, long s, double w)-> void
	{
		const long xc = (x*s) % m.cols();
		const long yc = (y*s) % m.rows();
		const long xnc = ((x+1)*s) % m.cols();
		const long ync = ((y+1)*s) % m.rows();
		m(y*s+s/2,x*s+s/2) = std::clamp<double>((m(yc,xc) + m(ync,xc) + m(yc,xnc) + m(ync,xnc))/4
		                                       + 2*rand()*w-w, 0.0, 1.0);
	};
	auto square = [&](long x, long y, long s, double w)-> void
	{
		const long xpc = ((x-1)*s+m.cols()) % m.cols();
		const long ypc = ((y-1)*s+m.rows()) % m.rows();
		const long xnc = ((x+1)*s) % m.cols();
		const long ync = ((y+1)*s) % m.rows();
		m(y*s,x*s) = std::clamp<double>((m(y*s,xpc) + m(y*s,xnc) + m(ypc,x*s) + m(ync,x*s))/4
		                               + 2*rand()*w-w, 0.0, 1.0);
	};

	// set each of some nodes' value on an initial coarse grid
	for (long y = 0; y < ycells; ++y)
		for (long x = 0; x < xcells; ++x)
			m(y*size,x*size) = rand();

	for (long cellsize = size; cellsize > 1; ) {
		for (long y = 0; y < ycells; ++y)
			for (long x = 0; x < xcells; ++x)
				diamond(x, y, cellsize, wiggle);

		cellsize /= 2;
		ycells *= 2;
		xcells *= 2;
		wiggle *= damping;

		for (long y=0; y<ycells; ++y) {
			if (y%2 == 0)
				for (long x=1; x<xcells; x+=2)
					square(x, y, cellsize, wiggle);
			else
				for (long x=0; x<xcells; x+=2)
					square(x, y, cellsize, wiggle);
		}
		wiggle *= damping;
	}
	return m;
}

/* =================================================================
 * value noise
 * \par cellsize the width and height of each cell
 * \par xcells how many cells on the x-axis
 * \par ycells how many cells on the y-axis
 * \par n_oct how many octaves of noise should be used,
 *            default is $1$
 * \par f the interpolation function, default is $id$
 */
Realfield2D
value_noise(long cellsize, long xcells, long ycells, long n_oct/*=1*/,
            const R2RFunction& f/*=math::identity<double>*/)
{
	assert(n_oct < std::log2(cellsize));
	std::mt19937 gen{std::random_device{}()};
	auto rand = [&](){ return std::generate_canonical<double,28>(gen); };
	Realfield2D noise{ycells*cellsize, xcells*cellsize};
	Realfield2D m{ycells*cellsize, xcells*cellsize};
	noise = 0.0;

	for (long k=0; k < n_oct; ++k) {
		const long csize = (cellsize>>k);
		for (long i=0; i < (ycells*(1<<k)); ++i)
			for (long j=0; j < (xcells*(1<<k)); ++j)
				m(i*csize,j*csize) = rand();

		for (long i=0; i < ycells*cellsize; ++i)
			for (long j=0; j < xcells*cellsize; ++j) {
				long ic = i / csize;
				long jc = j / csize;
				long u = i % csize;
				long v = j % csize;
				m(i,j) = math::interpolate2d(m(ic*csize, jc*csize),
				                             m(ic*csize, (jc+1)*csize % m.cols()),
				                             m((ic+1)*csize % m.rows(), jc*csize),
				                             m((ic+1)*csize % m.rows(), (jc+1)*csize % m.cols()),
				                             u, v, csize, f);
			}
		m /= (1 << k);
		noise += m;
	}
	return noise;
}

/* =================================================================
 * classic 2D perlin noise
 * \par cellsize the width and height of each cell
 * \par xcells how many cells on the x-axis
 * \par ycells how many cells on the y-axis
 * \par n_oct how many octaves of noise should be used,
 *            default is $1$
 */
Realfield2D
gradient_noise(long cellsize, long xcells, long ycells, long n_oct/*=1*/)
{
	assert(n_oct < std::log2(cellsize));
	std::mt19937 gen{std::random_device{}()};
	auto rand = [&](){ return 2*std::generate_canonical<double,28>(gen)-1; };

	Realfield2D noise{ycells*cellsize,xcells*cellsize};
	Realfield2D m{ycells*cellsize,xcells*cellsize};
	Vectorfield2D grad{ycells*cellsize,xcells*cellsize};
	noise = 0.0;

   Eigen::Vector2d uv00, uv01, uv10, uv11;
	for (long k=0; k < n_oct; ++k) {
		const long csize = cellsize >> k;

		for (long i=0; i < (ycells*(1<<k)); ++i)
			for (long j=0; j < (xcells*(1<<k)); ++j) {
				grad(i*csize,j*csize)[0] = rand();
				grad(i*csize,j*csize)[1] = rand();
            grad(i*csize,j*csize).normalize();
			}

		for (long i=0; i < ycells*cellsize; ++i)
			for (long j=0; j < xcells*cellsize; ++j) {
				long ic = i / csize;
				long jc = j / csize;
				double u = i % csize;
				double v = j % csize;
				uv00[0]=u/csize    ; uv00[1]=v/csize;
				uv01[0]=u/csize    ; uv01[1]=v/csize-1.0;
				uv10[0]=u/csize-1.0; uv10[1]=v/csize;
				uv11[0]=u/csize-1.0; uv11[1]=v/csize-1.0;
				m(i,j) = math::interpolate2d(grad(ic*csize,jc*csize).dot(uv00),
				                             grad(ic*csize,(jc+1)*csize % m.cols()).dot(uv01),
				                             grad((ic+1)*csize % m.rows(),jc*csize).dot(uv10),
				                             grad((ic+1)*csize % m.rows(),(jc+1)*csize % m.cols()).dot(uv11),
				                             u, v, csize, math::cubic_interpolant<double>) / sqrt(2.0);
			}
		m /= 1 << k;
		noise += m;
	}
	return noise;
}

/* =================================================================
 * Agent based landscape generation based on:
 * Doran, J., Parberry, I.: Controlled procedural terrain generation
 * using software agents. IEEE Transactions on Computational Intelli-
 * gence and AI in Games 2(2), 111-119(2010)
 * TODO
 */
Grid
doran_parberry(long width, long height)
{
	Realfield2D heightfield{height,width};
	Realfield2D temperature{height,width};
	Realfield2D humidity{height,width};
	return Grid{width,height,heightfield,temperature,humidity};
}

void
Grid::classify()
{
	for (long i=0; i < height(); ++i) {
		for (long j=0; j < width(); ++j) {
			if (m_heightmap(i,j) < 0.1)
				m_type(i,j) = BiomeType::Deep_Ocean;
			else if (m_heightmap(i,j) < 0.3)
				m_type(i,j) = BiomeType::Ocean;
			else if (m_heightmap(i,j) < 0.4) {
				m_type(i,j) = BiomeType::Shallow_Ocean;
			} else if (m_heightmap(i,j) < 0.95) {
				if (m_humidity(i,j) <= 0.1) {
					if (m_temperature(i,j) < .12)
						m_type(i,j) = BiomeType::Nival_Desert;
					else if (m_temperature(i,j) <= 0.8)
						m_type(i,j) = BiomeType::Temperate_Desert;
					else
						m_type(i,j) = BiomeType::Tropical_Desert;
				} else if (m_humidity(i,j) <= 0.4) {
					if (m_temperature(i,j) < .12)
						m_type(i,j) = BiomeType::Nival_Shrubland;
					else if (m_temperature(i,j) <= 0.8)
						m_type(i,j) = BiomeType::Temperate_Shrubland;
					else
						m_type(i,j) = BiomeType::Tropical_Shrubland;
				} else if (m_humidity(i,j) <= 0.7) {
					if (m_temperature(i,j) < .12)
						m_type(i,j) = BiomeType::Nival_Shrubland;
					else if (m_temperature(i,j) <= 0.8)
						m_type(i,j) = BiomeType::Temperate_Forest;
					else
						m_type(i,j) = BiomeType::Tropical_Forest;
				} else {
					if (m_temperature(i,j) < .12)
						m_type(i,j) = BiomeType::Nival_Forest;
					else if (m_temperature(i,j) <= 0.8)
						m_type(i,j) = BiomeType::Temperate_Swamp;
					else
						m_type(i,j) = BiomeType::Tropical_Swamp;
				}
			} else {
				if (m_humidity(i,j) >= 0.8)
					m_type(i,j) = BiomeType::Glacier;
				else
					m_type(i,j) = BiomeType::Mountain;
			}
		}
	}
}
