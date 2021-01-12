#pragma once

#include <algorithm>
#include <cmath>
#include <type_traits>

namespace math {

template<typename T,size_t N>
struct Vector {
	static_assert(std::is_arithmetic<T>());
	/* -------------------------------------------------------------- */
	Vector() {}
	/* -------------------------------------------------------------- */
	Vector(const T v)
	{
		for (size_t i=0; i<N; ++i)
			data[i]=v;
	}
	/* -------------------------------------------------------------- */
	Vector(const T v[N])
	{
		for (size_t i=0; i<N; ++i)
			data[i]=v[i];
	}
	/* -------------------------------------------------------------- */
	Vector(const Vector<T,N>& other)
	{
		for (size_t i=0; i<N; ++i)
			data[i]=other[i];
	}
	/* -------------------------------------------------------------- */
	Vector<T,N>&
	operator=(const Vector<T,N>& other)
	{
		if (&other == this)
			return *this;
		for (size_t i=0; i<N; ++i)
			data[i]=other[i];
		return *this;
	}
	/* ============================================================== */
	T&
	operator[](const size_t i)
	{
		return data[i];
	}
	/* -------------------------------------------------------------- */
	const T&
	operator[](const size_t i)
	const {
		return data[i];
	}
	/* ============================================================== */
	Vector<T,N>
	operator+(const Vector<T,N>& other)
	const {
		Vector<T,N> d;
		for (size_t i=0; i<N; ++i)
			d[i] = data[i]+other[i];
		return d;
	}
	/* -------------------------------------------------------------- */
	Vector<T,N>
	operator+=(const Vector<T,N>& other)
	{
		for (size_t i=0; i<N; ++i)
			data[i] += other[i];
		return *this;
	}
	/* ============================================================== */
	Vector<T,N>
	operator-()
	const {
		Vector<T,N> d;
		for (size_t i=0; i<N; ++i)
			d[i] = -data[i];
		return d;
	}
	/* -------------------------------------------------------------- */
	Vector<T,N>
	operator-(const Vector<T,N>& other)
	const {
		Vector<T,N> d;
		for (size_t i=0; i<N; ++i)
			d[i] = data[i]-other[i];
		return d;
	}
	/* -------------------------------------------------------------- */
	Vector<T,N>
	operator-=(const Vector<T,N>& other)
	{
		for (size_t i=0; i<N; ++i)
			data[i] -= other[i];
		return *this;
	}
	/* ============================================================== */
	Vector<T,N>
	operator*(const Vector<T,N>& other)
	const {
		Vector<T,N> d;
		for (size_t i=0; i<N; ++i)
			d[i] = data[i]*other[i];
		return d;
	}
	/* -------------------------------------------------------------- */
	Vector<T,N>
	operator*=(const Vector<T,N>& other)
	{
		for (size_t i=0; i<N; ++i)
			data[i] *= other[i];
		return *this;
	}
	/* ============================================================== */
	Vector<T,N>
	operator/(const Vector<T,N>& other)
	const {
		Vector<T,N> d;
		for (size_t i=0; i<N; ++i)
			d[i] = data[i]/other[i];
		return d;
	}
	/* -------------------------------------------------------------- */
	Vector<T,N>
	operator/=(const Vector<T,N>& other)
	{
		for (size_t i=0; i<N; ++i)
			data[i] /= other[i];
		return *this;
	}
private:
	T data[N];
};
/* ================================================================= */
template<typename T, size_t N> T
dot(const Vector<T,N>& lhs, const Vector<T,N>& rhs)
{
	T d{};
	for (size_t i=0; i<N; ++i)
		d += lhs[i]*rhs[i];
	return d;
}
template<typename T, size_t N> T
dot(const T lhs[N], const T rhs[N])
{
	T d{};
	for (size_t i=0; i<N; ++i)
		d += lhs[i]*rhs[i];
	return d;
}
/* ----------------------------------------------------------------- */
template<typename T, size_t N> T
norm2(const Vector<T,N>& v)
{
	T n{};
	for (size_t i=0; i<N; ++i)
		n += v[i]*v[i];
	return std::sqrt(n);
}
template<typename T, size_t N> T
norm2(const T v[N])
{
	T n{};
	for (size_t i=0; i<N; ++i)
		n += v[i]*v[i];
	return std::sqrt(n);
}
/* ----------------------------------------------------------------- */
template<typename T, size_t N> void
normalize(T v[N])
{
	T n=norm2<T,N>(v);
	for (size_t i=0; i<N; ++i)
		v[i] /= n;
}

template<typename T, size_t N> void
normalize(Vector<T,N>& v)
{
   T n=norm2(v);
   for (size_t i=0; i<N; ++i)
      v[i] /= n;
}
} // namespace math
/* ================================================================= */
namespace std {

template<typename T,int N> T
max(const math::Vector<T,N>& v)
{
	T p = v[0];
	for (int i=1; i<N; ++i)
		p = max(p,v[i]);
	return p;
}
/* -------------------------------------------------------------- */
template<typename T,int N> T
min(const math::Vector<T,N>& v)
{
	T p = v[0];
	for (int i=1; i<N; ++i)
		p = min(p,v[i]);
	return p;
}

} // namespace std
