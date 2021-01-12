#pragma once

#include <type_traits>
#include <functional>

namespace math {

template<typename T>
T
identity(const T x)
{
	return x;
}

template<typename T>
T
interpolate(T x0, T x1, double t, double size=1.0,
            const std::function<double(double)>&f=identity<double>)
{  static_assert(std::is_arithmetic<T>());
	return (1-f(t/size))*x0+f(t/size)*x1;
}

template<typename T>
T
interpolate2d(T x00, T x01, T x10, T x11, double u, double v, double size=1.0,
              const std::function<double(double)>&f=identity<double>)
{  static_assert(std::is_arithmetic<T>());
	return (1-f(u/size))*(1-f(v/size))*x00+(1-f(u/size))*   f(v/size) *x01
			+   f(u/size) *(1-f(v/size))*x10+   f(u/size) *   f(v/size) *x11;
}

template<typename T>
T
interpolate3d(T x000, T x001, T x010, T x011, T x100, T x101, T x110, T x111,
              double u, double v, double w, double size=1.0,
              const std::function<double(double)>&f=identity<double>)
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
