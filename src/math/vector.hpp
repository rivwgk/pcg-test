#pragma once

#include <algorithm>
#include <cmath>
#include <type_traits>

namespace math {

template<typename K,size_t N>
struct Vector {
	static_assert(std::is_arithmetic<K>());

	Vector() {}
	Vector(const K v)
	{
		for (size_t i=0; i<N; ++i)
			data[i]=v;
	}

	Vector(const Vector<K,N>& other)
	{
		for (size_t i=0; i<N; ++i)
			data[i]=other[i];
	}

	Vector<K,N>&
	operator=(const Vector<K,N>& other)
	{
		if (&other == this)
			return *this;
		for (size_t i=0; i<N; ++i)
			data[i]=other[i];
		return *this;
	}
	/* ============================================================== */
	K&
	operator[](const size_t i)
	{
		return data[i];
	}
	const K&
	operator[](const size_t i)
	const {
		return data[i];
	}
	/* ============================================================== */
	Vector<K,N>
	operator+(const Vector<K,N>& other)
	const {
		Vector<K,N> d;
		for (size_t i=0; i<N; ++i)
			d[i] = data[i]+other[i];
		return d;
	}

	Vector<K,N>
	operator+=(const Vector<K,N>& other)
	{
		for (size_t i=0; i<N; ++i)
			data[i] += other[i];
		return *this;
	}
	/* ============================================================== */
	Vector<K,N>
	operator-()
	const {
		Vector<K,N> d;
		for (size_t i=0; i<N; ++i)
			d[i] = -data[i];
		return d;
	}

	Vector<K,N>
	operator-(const Vector<K,N>& other)
	const {
		Vector<K,N> d;
		for (size_t i=0; i<N; ++i)
			d[i] = data[i]-other[i];
		return d;
	}

	Vector<K,N>
	operator-=(const Vector<K,N>& other)
	{
		for (size_t i=0; i<N; ++i)
			data[i] -= other[i];
		return *this;
	}
	/* ============================================================== */
	Vector<K,N>
	operator*(const Vector<K,N>& other)
	const {
		Vector<K,N> d;
		for (size_t i=0; i<N; ++i)
			d[i] = data[i]*other[i];
		return d;
	}
	/* -------------------------------------------------------------- */
	Vector<K,N>
	operator*=(const Vector<K,N>& other)
	{
		for (size_t i=0; i<N; ++i)
			data[i] *= other[i];
		return *this;
	}
	/* ============================================================== */
	Vector<K,N>
	operator/(const Vector<K,N>& other)
	const {
		Vector<K,N> d;
		for (size_t i=0; i<N; ++i)
			d[i] = data[i]/other[i];
		return d;
	}
	/* -------------------------------------------------------------- */
	Vector<K,N>
	operator/=(const Vector<K,N>& other)
	{
		for (size_t i=0; i<N; ++i)
			data[i] /= other[i];
		return *this;
	}
private:
	K data[N];
};
/* ================================================================= */
template<typename K, size_t N> K
dot(const Vector<K,N>& lhs, const Vector<K,N>& rhs)
{
	K d{};
	for (size_t i=0; i<N; ++i)
		d += lhs[i]*rhs[i];
	return d;
}
template<typename K, size_t N> K
dot(const K lhs[N], const K rhs[N])
{
	K d{};
	for (size_t i=0; i<N; ++i)
		d += lhs[i]*rhs[i];
	return d;
}
/* ----------------------------------------------------------------- */
template<typename K, size_t N> K
norm2(const Vector<K,N>& v)
{
	K n{};
	for (size_t i=0; i<N; ++i)
		n += v[i]*v[i];
	return std::sqrt(n);
}
template<typename K, size_t N> K
norm2(const K v[N])
{
	K n{};
	for (size_t i=0; i<N; ++i)
		n += v[i]*v[i];
	return std::sqrt(n);
}
/* ----------------------------------------------------------------- */
template<typename K, size_t N> void
normalize(K v[N])
{
	K n=norm2<K,N>(v);
	for (size_t i=0; i<N; ++i)
		v[i] /= n;
}

template<typename K, size_t N> void
normalize(Vector<K,N>& v)
{
	K n=norm2(v);
	for (size_t i=0; i<N; ++i)
		v[i] /= n;
}

} // namespace math
