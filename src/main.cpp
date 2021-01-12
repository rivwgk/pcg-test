#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

#include "grid.h"
#include "mapdisplay.h"
#include "states.h"

int
main(int argc, char **argv)
{
	sf::RenderWindow window{sf::VideoMode{640u, 480u}, "test"};
	window.setFramerateLimit(20u);
	StateManager sman{};
	const int ld_cellsize = 4;
	const int xcells = 15;
	const int ycells = 10;
	Grid gd = diamond_square(ld_cellsize,xcells,ycells,0.8);
	Grid gl = value_noise(1<<ld_cellsize,xcells,ycells,1);
	Grid gc = value_noise(1<<ld_cellsize,xcells,ycells,1,
	                      [](double t)->double{return t*t*(-2*t+3);});
	Grid gg = gradient_noise(1<<ld_cellsize,xcells,ycells,3);
	sman.push(new Grid2DState{&gd});

	while (window.isOpen()) {
		sman.update(window);

		window.clear();
		sman.draw(window);
		window.display();
	}
	delete sman.pop();
}
