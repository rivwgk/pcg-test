/*
 */

#include "mapdisplay.h"
#include "colourscales.h"

#include "math/vector.hpp"

MapDisplay::MapDisplay()
{
	shader.loadFromMemory(FRAG_SHADER,sf::Shader::Type::Fragment);
	shader.setUniform("coloured", coloured);
	display.setSize(sf::Vector2f{.0f,.0f});
}

MapDisplay::MapDisplay(const MapDisplay& other)
{
	width = other.width;
	height = other.height;
	scaling = other.scaling;
	coloured = other.coloured;
	x_offset = other.x_offset;
	y_offset = other.y_offset;
	grid = other.grid;
	shader.loadFromMemory(FRAG_SHADER,sf::Shader::Type::Fragment);
	shader.setUniform("coloured", coloured);
	display = other.display;
	img = other.img;
	tex = other.tex;
}

MapDisplay&
MapDisplay::operator=(const MapDisplay& other)
{
	if (&other == this)
		return *this;
	width = other.width;
	height = other.height;
	scaling = other.scaling;
	coloured = other.coloured;
	x_offset = other.x_offset;
	y_offset = other.y_offset;
	grid = other.grid;
	shader.loadFromMemory(FRAG_SHADER,sf::Shader::Type::Fragment);
	shader.setUniform("coloured", coloured);
	display = other.display;
	img = other.img;
	tex = other.tex;
	return *this;
}
/* ----------------------------------------------------------------- */
void
MapDisplay::set_grid(Grid* grid)
{
	this-> grid = grid;
	load_dataset(-1);
}
/* ----------------------------------------------------------------- */
void
MapDisplay::set_size(unsigned w, unsigned h)
{
	width = w;
	height = h;
	display.setSize(sf::Vector2f{static_cast<float>(width),
	                             static_cast<float>(height)});
}
/* ----------------------------------------------------------------- */
void
MapDisplay::scale(sf::Vector2f pos, double factor)
{  // 1.0: 32px/tile tilesize,  0.03125: one pixel per tile
	double xtile = x_offset + pos.x / current_tilesize();
	double ytile = y_offset + pos.y / current_tilesize();
	scaling = std::clamp<double>(factor*scaling, 0.03125, 1.0);
	printf("scaling factor of %f\n", scaling);
	x_offset = xtile - pos.x / current_tilesize();
	y_offset = ytile - pos.y / current_tilesize();
}
/* ----------------------------------------------------------------- */
void
MapDisplay::draw(sf::RenderWindow& w)
{
	display.setPosition(0.0f,0.0f);
	display.setSize(sf::Vector2f{w.getSize()});
	display.setTexture(&tex);
	const float xfac = width/(grid-> width()*tilesize)*5.0f;
	const float yfac = height/(grid-> height()*tilesize)*5.0f;
	sf::Glsl::Mat3 trafo({xfac/scaling, 0.0f,         x_offset/grid-> width(),
	                      0.0f,         yfac/scaling, y_offset/grid-> height(),
	                      0.0f,         0.0f,         1.0f});
	shader.setUniform("data", tex);
	shader.setUniform("trafo", trafo);
	w.draw(display, &shader);
}
/* ----------------------------------------------------------------- */
void
MapDisplay::update(sf::RenderWindow& w)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
		y_offset -= 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
		y_offset += 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
		x_offset -= 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
		x_offset += 1;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F1)) {
		printf("height\n"); coloured = false; load_dataset(0);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F2)) {
		printf("temperature\n"); coloured = false; load_dataset(1);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F3)) {
		printf("humidity\n"); coloured = false; load_dataset(2);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F4)) {
		coloured = true; load_dataset(-1);
	}
	shader.setUniform("coloured", coloured);
}
/* ----------------------------------------------------------------- */
void
MapDisplay::load_dataset(int i)
{
	img.create(grid-> width(), grid-> height());
	if (i == -1)
		for (int y=0; y<grid-> height(); ++y)
			for (int x=0; x<grid-> width(); ++x) {
				sf::Color c{Tile::BiomeColours[static_cast<char>((*grid)(x,y).type)]};
				img.setPixel(x, y, c);
			}
	else
		for (int y=0; y<grid-> height(); ++y)
			for (int x=0; x<grid-> width(); ++x) {
				double v = std::clamp((*grid)(x,y).values[i], 0.0, 1.0);
				img.setPixel(x, y, sf::Color{255*v,0,0});
			}
	tex.create(grid-> width(), grid-> height());
	tex.update(img);
}
/* ----------------------------------------------------------------- */
size_t
MapDisplay::current_tilesize()
const {
	return scaling*tilesize;
}
