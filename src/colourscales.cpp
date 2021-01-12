#include <cassert>
#include <cmath>
#include "colourscales.h"

sf::Color
greyscale(double value, double min, double max)
{
	const sf::Uint8 x = (value - min)/(max - min) * 255;
	return sf::Color{x, x, x};
}

sf::Color
rainbow(double value, double min, double max)
{
	const double h = (value - min)/(max - min)*360.0;
	return HSVtoRGB(HSVColor{h, 0.6, 1.0});
}

sf::Color
temperature(double value, double min, double max)
{
	const double x = (value - min)/(max - min);
	sf::Color c{255, 255, 255};
	if (3*x < 1.0) c.r *= 3*x;
	else c.r *= 1.0;
	if (3*x <= 1.0) c.g *= 0.0;
	else if (3*x > 1.0 and 3*x < 2.0) c.g *= (3*x)-1;
	else c.g *= 1;
	if (3*x < 2.0) c.b *= 0.0;
	else c.b *= 3*x-2;
	return c;
}

sf::Color
HSVtoRGB(const HSVColor& hsv)
{
	float r,g,b;
	const float h=hsv.h/360.0;
	const float s=hsv.s/100.0;
	const float v=hsv.v/100.0;
	if (s==0)
		return sf::Color{sf::Uint8(v*256),sf::Uint8(v*256),sf::Uint8(v*256)};
	else {
		float f,p,q,t;
		int i=int(std::floor(h*6));
		f=h*6-i;
		p=v*(1-s);
		q=v*(1-(s*f));
		t=v*(1-(s*(1-f)));
		switch (i) {
		case 0: r=v; g=t; b=p; break;
		case 1: r=q; g=v; b=p; break;
		case 2: r=p; g=v; b=t; break;
		case 3: r=p; g=q; b=v; break;
		case 4: r=t; g=p; b=v; break;
		case 5: r=v; g=p; b=q; break;
		}
		return sf::Color{sf::Uint8(255*r),sf::Uint8(255*g),sf::Uint8(255*b)};
	}
}
