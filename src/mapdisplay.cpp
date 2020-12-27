/*
 */

#include "mapdisplay.h"
#include "colourscales.h"

MapDisplay::MapDisplay()
{
   shader.loadFromMemory(SHADER,sf::Shader::Type::Fragment);
   shader.setUniform("coloured", coloured);
   display.setSize(sf::Vector2f{.0f,.0f});
}
MapDisplay::MapDisplay(const MapDisplay& other)
{
   width = other.width; height = other.height;
   scaling = other.scaling;
   coloured = other.coloured;
   x = other.x; y = other.y;
   grid = other.grid;
   shader.loadFromMemory(SHADER,sf::Shader::Type::Fragment);
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
   width = other.width; height = other.height;
   scaling = other.scaling;
   coloured = other.coloured;
   x = other.x; y = other.y;
   grid = other.grid;
   shader.loadFromMemory(SHADER,sf::Shader::Type::Fragment);
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
   load_dataset(0);
   shader.setUniform("width",grid-> width());
   shader.setUniform("height",grid-> height());
}
/* ----------------------------------------------------------------- */
void
MapDisplay::center_on(int x,int y)
{
}
/* ----------------------------------------------------------------- */
void
MapDisplay::set_size(int w,int h)
{
   width=w; height=h;
   display.setSize(sf::Vector2f{static_cast<float>(width),
                                static_cast<float>(height)});
}
/* ----------------------------------------------------------------- */
void
MapDisplay::scale(sf::Vector2f pos,double factor)
{  // 1.0: 32px/tile tilesize,  0.03125: one pixel per tile
   scaling = std::clamp<double>(factor*scaling, 0.03125, 1.0);
   printf("scaling factor of %f\n", scaling);
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
   sf::Glsl::Mat3 trafo({xfac/scaling, 0.0f,         x/grid-> width(),
                         0.0f,         yfac/scaling, y/grid-> height(),
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
      y -= 1;
   else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
      y += 1;
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
      x -= 1;
   else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
      x += 1;

   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F1))
      printf("height\n"), load_dataset(0);
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F2))
      printf("temperature\n"), load_dataset(1);
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F4))
      printf("humidity\n"), load_dataset(2);
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F6))
      printf("gradient.x\n"), load_dataset(5);
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F7))
      printf("gradient.y\n"), load_dataset(6);
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
      coloured = !coloured, shader.setUniform("coloured", coloured);
}
/* ----------------------------------------------------------------- */
void
MapDisplay::load_dataset(int i)
{
   img.create(grid-> width(), grid-> height());
   if (i == 5)
      for (int y=0; y<grid-> height(); ++y)
         for (int x=0; x<grid-> width(); ++x) {
            sf::Uint8 v = 255u*math::norm2<double,2>((*grid)(x,y).gradient);
            img.setPixel(x,y,sf::Color{v,v,v});
         }
   else
      for (int y=0; y<grid-> height(); ++y)
         for (int x=0; x<grid-> width(); ++x) {
            sf::Uint8 v = 255u*(*grid)(x,y).values[i];
            img.setPixel(x,y,sf::Color{v,v,v});
         }
   tex.create(grid-> width(), grid-> height());
   tex.update(img);
   tex.setRepeated(true);
}
/* ----------------------------------------------------------------- */
size_t
MapDisplay::current_tilesize()
const {
   return scaling*tilesize;
}
