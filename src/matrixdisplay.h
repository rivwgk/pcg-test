/*
 */

#pragma once

#include <SFML/Graphics.hpp>
#include "grid.h"

class MatrixDisplay {
public:
	MatrixDisplay();
	MatrixDisplay(const MatrixDisplay&);
	MatrixDisplay& operator=(const MatrixDisplay&);

	void set_size(size_t w, size_t h);
	void scale(sf::Vector2f pos, double factor);
	void draw(sf::RenderWindow&);
	void update();

	template<typename K>
	void
	load_matrix(const Field2D<K>& mat,
	            const std::function<sf::Color(const K&)> conv)
	{
		img.create(mat.cols(), mat.rows());
		for (long y = 0; y < mat.rows(); ++y)
			for (long x = 0; x < mat.cols(); ++x)
				img.setPixel(x, y, conv(mat(y,x)));
		tex.create(mat.cols(), mat.rows());
		tex.update(img);
		shader.setUniform("data", tex);
	}

private:
	double current_tilesize() const;

	const char*FRAG_SHADER =
	"uniform sampler2D data;\n"
	"uniform mat3 trafo;\n"
	"void main() {\n"
	"  vec3 pos = trafo*vec3(gl_TexCoord[0].xy, 1.);\n"
	"  vec3 color;\n"
	"  color = texture2D(data, pos.xy).rgb;\n"
	"  if (pos.x < 0. || 1. < pos.x || pos.y < 0. || 1. < pos.y)\n"
	"    color = vec3(0.,0.,0.);\n"
	"  gl_FragColor = vec4(color,1.);\n"
	"}";
	sf::Shader shader;
	sf::RectangleShape display;
	sf::Image img;
	sf::Texture tex;
	size_t width = 0, height = 0;
	double scaling = 1.;
	double x_offset = 0, y_offset = 0;

	constexpr static double tilesize = 32;
	constexpr static double min_scale = 1.0 / (1 << 8);
	constexpr static double max_scale = 1.0;
};
