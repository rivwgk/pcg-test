#include <chrono>
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

#include "grid.h"
#include "states.h"

int
main(int argc, char **argv)
{
	sf::RenderWindow window{sf::VideoMode{640u, 480u}, "test"};
	window.setFramerateLimit(20u);
	StateManager sman{};
	constexpr size_t ld_cellsize = 5;
	constexpr size_t cellsize = 1 << ld_cellsize;
	constexpr size_t xcells = 30;
	constexpr size_t ycells = 20;
	auto gd = diamond_square(ld_cellsize,xcells,ycells,0.7);
	auto gl = value_noise(1<<ld_cellsize,xcells,ycells, 2);
	auto gc = value_noise(1<<ld_cellsize,xcells,ycells, 2,
	                      math::cubic_interpolant<double>);
	auto gg = gradient_noise(1<<ld_cellsize,xcells,ycells,3);
	gg += 1.0;
	Grid g{ycells*cellsize, xcells*cellsize, gd, gc, gg};
	sman.push(new Grid2DState{&g});

	while (window.isOpen()) {
		sman.update(window);

		window.clear();
		sman.draw(window);
		window.display();
	}
	delete sman.pop();
}
