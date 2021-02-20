#include "matrixdisplay.h"

MatrixDisplay::MatrixDisplay()
{
	shader.loadFromMemory(FRAG_SHADER, sf::Shader::Type::Fragment);
	display.setSize(sf::Vector2f{0.0f, 0.0f});
	display.setPosition(0.0f, 0.0f);
}

MatrixDisplay::MatrixDisplay(const MatrixDisplay& other)
 : display{other.display}, img{other.img}, tex{other.tex},
   width{other.width}, height{other.height}, scaling{other.scaling},
   x_offset{other.x_offset}, y_offset{other.y_offset}
{
	shader.loadFromMemory(FRAG_SHADER, sf::Shader::Type::Fragment);
}

MatrixDisplay&
MatrixDisplay::operator=(const MatrixDisplay& other)
{
	if (&other == this)
		return *this;

	width = other.width;
	height = other.height;
	scaling = other.scaling;
	x_offset = other.x_offset;
	y_offset = other.y_offset;
	shader.loadFromMemory(FRAG_SHADER, sf::Shader::Type::Fragment);
	display = other.display;
	img = other.img;
	tex = other.tex;
	return *this;
}

void
MatrixDisplay::set_size(size_t w, size_t h)
{
	width = w; height = h;
	display.setSize(sf::Vector2f{width, height});
}

void
MatrixDisplay::scale(sf::Vector2f pos, double factor)
{
	double xtile = x_offset + pos.x / current_tilesize();
	double ytile = y_offset + pos.y / current_tilesize();
	scaling = std::clamp<double>(factor*scaling, min_scale, max_scale);
  	x_offset = xtile - pos.x / current_tilesize();
  	y_offset = ytile - pos.y / current_tilesize();
}

void
MatrixDisplay::draw(sf::RenderWindow& w)
{
  	display.setTexture(&tex);
	sf::Vector2u shape = img.getSize();
	const float xfac = (width / current_tilesize()) / shape.x;
	const float yfac = (height / current_tilesize()) / shape.y;
	sf::Glsl::Mat3 trafo({xfac, 0.0f, x_offset / shape.x,
	                      0.0f, yfac, y_offset / shape.y,
	                      0.0f, 0.0f, 1.0f});
	shader.setUniform("trafo", trafo);
	w.draw(display, &shader);
}

void
MatrixDisplay::update()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
		y_offset -= 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
		y_offset += 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
		x_offset -= 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
		x_offset += 1;
}

double
MatrixDisplay::current_tilesize()
const {
	return scaling*tilesize;
}
