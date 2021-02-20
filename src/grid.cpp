/*
 */
#include "grid.h"
#include "tile.h"
#include "math/matrix.hpp"
#include "math/vector.hpp"
#include "math/interpolation.hpp"
#include "math/utilities.hpp"

#include <algorithm>
#include <random>
#include <cassert>
#include <cmath>
#include <cstdlib>

Grid::Grid(size_t height, size_t width, math::Matrix<double> heightmap,
           math::Matrix<double> temperature, math::Matrix<double> humidity)
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

size_t
Grid::height()
const {
	return m_height;
}

size_t
Grid::width()
const {
	return m_width;
}

const math::Matrix<double>&
Grid::heightmap()
const {
	return m_heightmap;
}

const math::Matrix<double>&
Grid::temperature()
const {
	return m_temperature;
}

const math::Matrix<double>&
Grid::humidity()
const {
	return m_humidity;
}

const math::Matrix<BiomeType>&
Grid::types()
const {
	return m_type;
}

/* =================================================================
 * classic diamond square algorithm
 * TODO: documentation
 */
math::Matrix<double>
diamond_square(size_t exp, size_t xcells, size_t ycells, double wiggle)
{
	const size_t size = 1<<exp;
	const size_t width = size * xcells;
	const size_t height = size * ycells;
	const double damping = 0.70;
	std::mt19937 gen{std::random_device{}()};
	math::Matrix<double> m{height, width};
	auto rand = [&](){ return std::generate_canonical<double,28>(gen); };
	auto diamond = [&](long x, long y, size_t s, double w)-> void
	{
		const size_t xc = (x*s) % m.cols();
		const size_t yc = (y*s) % m.rows();
		const size_t xnc = ((x+1)*s) % m.cols();
		const size_t ync = ((y+1)*s) % m.rows();
		m(y*s+s/2,x*s+s/2) = std::clamp<double>((m(yc,xc) + m(ync,xc) + m(yc,xnc) + m(ync,xnc))/4
		                                       + 2*rand()*w-w, 0.0, 1.0);
	};
	auto square = [&](long x, long y, size_t s, double w)-> void
	{
		const size_t xpc = ((x-1)*s+m.cols()) % m.cols();
		const size_t ypc = ((y-1)*s+m.rows()) % m.rows();
		const size_t xnc = ((x+1)*s) % m.cols();
		const size_t ync = ((y+1)*s) % m.rows();
		m(y*s,x*s) = std::clamp<double>((m(y*s,xpc) + m(y*s,xnc) + m(ypc,x*s) + m(ync,x*s))/4
		                               + 2*rand()*w-w, 0.0, 1.0);
	};

	// set each of some nodes' value on an initial coarse grid
	for (size_t y=0; y<=ycells; ++y)
		for (size_t x=0; x<=xcells; ++x)
			m(y*size,x*size) = rand();

	for (size_t cellsize=size; cellsize>1; ) {
		for (long y=0; y<ycells; ++y)
			for (long x=0; x<xcells; ++x)
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
math::Matrix<double>
value_noise(size_t cellsize, size_t xcells, size_t ycells, size_t n_oct/*=1*/,
            const std::function<double(double)>& f/*=math::identity<double>*/)
{
	assert(n_oct < std::log2(cellsize));
	std::mt19937 gen{std::random_device{}()};
	auto rand = [&](){ return std::generate_canonical<double,28>(gen); };
	math::Matrix<double> noise{ycells*cellsize, xcells*cellsize};
	math::Matrix<double> m{ycells*cellsize, xcells*cellsize};
	noise = 0.0;

	for (size_t k=0; k < n_oct; ++k) {
		const size_t csize = (cellsize>>k);
		for (size_t i=0; i < (ycells*(1<<k)); ++i)
			for (size_t j=0; j < (xcells*(1<<k)); ++j)
				m(i*csize,j*csize) = rand();

		for (size_t i=0; i < ycells*cellsize; ++i)
			for (size_t j=0; j < xcells*cellsize; ++j) {
				size_t ic = i / csize;
				size_t jc = j / csize;
				size_t u = i % csize;
				size_t v = j % csize;
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
math::Matrix<double>
gradient_noise(size_t cellsize,size_t xcells,size_t ycells,size_t n_oct/*=1*/)
{
	assert(n_oct < std::log2(cellsize));
	std::mt19937 gen{std::random_device{}()};
	auto rand = [&](){ return 2*std::generate_canonical<double,28>(gen)-1; };

	math::Matrix<double> noise{ycells*cellsize,xcells*cellsize};
	math::Matrix<double> m{ycells*cellsize,xcells*cellsize};
	math::Matrix<math::Vector<double,2>> grad{ycells*cellsize,xcells*cellsize};
	noise = 0.0;

	math::Vector<double,2> uv00, uv01, uv10, uv11;
	for (size_t k=0; k < n_oct; ++k) {
		const size_t csize = cellsize >> k;

		for (size_t i=0; i < (ycells*(1<<k)); ++i)
			for (size_t j=0; j < (xcells*(1<<k)); ++j) {
				grad(i*csize,j*csize)[0] = rand();
				grad(i*csize,j*csize)[1] = rand();
				math::normalize(grad(i*csize,j*csize));
			}

		for (size_t i=0; i < ycells*cellsize; ++i)
			for (size_t j=0; j < xcells*cellsize; ++j) {
				size_t ic = i / csize;
				size_t jc = j / csize;
				double u = i % csize;
				double v = j % csize;
				uv00[0]=u/csize    ; uv00[1]=v/csize;
				uv01[0]=u/csize    ; uv01[1]=v/csize-1.0;
				uv10[0]=u/csize-1.0; uv10[1]=v/csize;
				uv11[0]=u/csize-1.0; uv11[1]=v/csize-1.0;
				m(i,j) = math::interpolate2d(math::dot(grad(ic*csize,jc*csize),uv00),
				                             math::dot(grad(ic*csize,(jc+1)*csize % m.cols()),uv01),
				                             math::dot(grad((ic+1)*csize % m.rows(),jc*csize),uv10),
				                             math::dot(grad((ic+1)*csize % m.rows(),(jc+1)*csize % m.cols()),uv11),
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
doran_parberry(size_t width,size_t height)
{
	math::Matrix<double> heightfield{height,width};
	math::Matrix<double> temperature{height,width};
	math::Matrix<double> humidity{height,width};
	return Grid{width,height,heightfield,temperature,humidity};
}

void
Grid::classify()
{
	for (size_t i=0; i < height(); ++i) {
		for (size_t j=0; j < width(); ++j) {
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
