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
	"uniform sampler1D colours;\n"
	"uniform mat3 trafo;\n"
	"uniform bool coloured;\n"
	"void main() {\n"
	"  vec3 pos = trafo*vec3(gl_TexCoord[0].xy, 1.);\n"
	"  float v = texture2D(data, pos.xy).r;\n"
	"  vec3 color;\n"
	"  if (coloured) {\n"
	"  if (v <= .55)\n"
	"    color = mix(vec3(0., 0., 0.), vec3(0., 0., .55), v);\n"
	"  else if (v <= .6)\n"
	"    color = mix(vec3(0., 0., .55), vec3(0., .5, .5), (v-0.55)/0.05);\n"
	"  else if (v <= .7)\n"
	"    color = mix(vec3(0., .5, .5), vec3( .7, .5, 0.), (v-0.56)/0.14);\n"
	"  else if (v <= .8)\n"
	"    color = mix(vec3(.7, .5, .0), vec3(.4, .55, 0.), (v-0.7)/0.1);\n"
	"  else if (v <= .92)\n"
	"    color = mix(vec3(.4, .55, 0.), vec3(.2, .4, .0), (v-0.8)/0.12);\n"
	"  else if (v <= .96)\n"
	"    color = mix(vec3(.2, .4, .0), vec3(.5, .5, .5), (v-0.92)/0.04);\n"
	"  else\n"
	"    color = mix(vec3(.5, .5, .5), vec3(.8, .8, .8), (v-0.96)/0.01);\n"
	"  } else color = mix(vec3(.0, .0, .0), vec3(1., 1., 1.),v);\n"
#if 1
	"  if (pos.x < 0. || 1. < pos.x || pos.y < 0. || 1. < pos.y)\n"
	"    color = vec3(0.,0.,0.);\n"
#endif
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
