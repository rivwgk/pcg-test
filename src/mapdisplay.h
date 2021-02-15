/*
 */

#pragma once

#include <SFML/Graphics.hpp>
#include "grid.h"

class MapDisplay {
public:
	MapDisplay();
	MapDisplay(const MapDisplay&);
	MapDisplay& operator=(const MapDisplay&);

	void set_grid(Grid* grid);
	void set_size(unsigned w, unsigned h);
	void scale(sf::Vector2f pos,double factor);

	void draw(sf::RenderWindow& w);
	void update(sf::RenderWindow& w);
private:
	void load_dataset(int i);
	size_t current_tilesize() const;

	const char*FRAG_SHADER =
	"uniform sampler2D data;\n"
	"uniform sampler2D colours;\n"
	"uniform mat3 trafo;\n"
	"uniform bool coloured;\n"
	"void main() {\n"
	"  vec3 pos = trafo*vec3(gl_TexCoord[0].xy, 1.);\n"
	"  float v = texture2D(data, pos.xy).r;\n"
	"  vec3 color;\n"
	"  if (coloured) {\n"
	"    color = texture2D(data, pos.xy).rgb;\n"
	"  } else\n"
	"    color = vec3(v,v,v);\n"
	"  if (pos.x < 0. || 1. < pos.x || pos.y < 0. || 1. < pos.y)\n"
	"    color = vec3(0.,0.,0.);\n"
	"  gl_FragColor = vec4(color,1.);\n"
	"}";
	sf::Shader shader;
	sf::RectangleShape display;
	sf::Image img;
	sf::Texture tex;
	unsigned width = 0, height = 0;
	double scaling = 1.;
	float x_offset = 0, y_offset = 0;
	bool coloured = true;
	Grid *grid = nullptr;
	const float tilesize = 32;
};
