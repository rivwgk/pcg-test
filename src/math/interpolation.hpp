#pragma once

#include <type_traits>
#include <functional>
#include "utils.hpp"

namespace math {

template<typename K>
K
identity(const K x)
{
	return x;
}

template<typename K>
K
cubic_interpolant(const K x)
{
	return x*x*(-2*x+3);
}

template<typename K>
K
interpolate(K x0, K x1, double t, double size=1.0,
            const R2RFunction& f = identity<double>)
{  static_assert(std::is_arithmetic<K>());
	return (1-f(t/size))*x0+f(t/size)*x1;
}

template<typename K>
K
interpolate2d(K x00, K x01, K x10, K x11, double u, double v, double size=1.0,
              const R2RFunction& f = identity<double>)
{  static_assert(std::is_arithmetic<K>());
	return (1-f(u/size))*(1-f(v/size))*x00+(1-f(u/size))*   f(v/size) *x01
			+   f(u/size) *(1-f(v/size))*x10+   f(u/size) *   f(v/size) *x11;
}

template<typename K>
K
interpolate3d(K x000, K x001, K x010, K x011, K x100, K x101, K x110, K x111,
              double u, double v, double w, double size=1.0,
              const R2RFunction& f = identity<double>)
{
	return (1-f(u/size))*(1-f(v/size))*(1-f(w/size))*x000
			+(1-f(u/size))*(1-f(v/size))*   f(w/size) *x001
			+(1-f(u/size))*   f(v/size) *(1-f(w/size))*x010
			+(1-f(u/size))*   f(v/size) *   f(w/size) *x011
			+   f(u/size) *(1-f(v/size))*(1-f(w/size))*x100
			+   f(u/size) *(1-f(v/size))*   f(w/size) *x101
			+   f(u/size) *   f(v/size) *(1-f(w/size))*x110
			+   f(u/size) *   f(v/size) *   f(w/size) *x111;
}

}
