#pragma once

#include "utilities.hpp"

namespace math {

template<typename K>
struct Matrix {

	Matrix() = delete;
	Matrix(size_t rows, size_t cols)
	 : m_rows{rows}, m_cols{cols}
	{
		if (rows == 0 or cols == 0)
			throw std::runtime_error{"matrix dimensions must be positive"};
		m_data = new K[m_rows*m_cols];
	}

	Matrix(const Matrix<K>& other)
	 : m_rows{other.m_rows}, m_cols{other.m_cols}
	{
		m_data = new K[m_rows*m_cols];
		for (size_t i=0; i < m_rows*m_cols; ++i)
			m_data[i] = other[i];
	}

	Matrix<K>&
	operator=(const Matrix<K>& other)
	{
		if (&other == this)
			return *this;

		m_rows = other.m_rows;
		m_cols = other.m_cols;
		m_data = new K[m_rows*m_cols];
		for (size_t i=0; i < m_rows*m_cols; ++i)
			m_data[i] = other[i];
		return *this;
	}

	Matrix<K>&
	operator=(const K& lambda)
	{
		for (size_t i=0; i < m_rows*m_cols; ++i)
			m_data[i] = lambda;
		return *this;
	}

	~Matrix()
	{
		delete[] m_data;
	}

	K&
	operator()(size_t i, size_t j)
	{
		return m_data[field_index(i, j)];
	}
	const K&
	operator()(size_t i, size_t j)
	const {
		return m_data[field_index(i, j)];
	}

	K&
	operator[](size_t i)
	{
		return m_data[i];
	}
	const K&
	operator[](size_t i)
	const {
		return m_data[i];
	}

	Matrix<K>
	operator+(const Matrix<K>& other)
	const {
		if (!check_dimensions(other))
			throw std::runtime_error{"mismatching matrix dimensions (+)"};

		Matrix<K> m{m_rows, m_cols};
		for (size_t i=0; i < m_rows*m_cols; ++i)
			m[i] = m_data[i] + other[i];
		return m;
	}

	Matrix<K>&
	operator+=(const Matrix<K>& other)
	{
		if (!check_dimensions(other))
			throw std::runtime_error{"mismatching matrix dimensions (+=)"};

		for (size_t i=0; i < m_rows*m_cols; ++i)
			m_data[i] += other[i];
		return *this;
	}

	Matrix<K>&
	operator+=(const K& lambda)
	{
		for (size_t i=0; i < m_rows*m_cols; ++i)
			m_data[i] += lambda;
		return *this;
	}

	Matrix<K>
	operator-()
	const {
		Matrix<K> m{m_rows, m_cols};
		for (size_t i=0; i < m_rows*m_cols; ++i)
			m[i] = -(*this)[i];
		return m;
	}

	Matrix<K>
	operator-(const Matrix<K>& other)
	const {
		if (!check_dimensions(other))
			throw std::runtime_error{"mismatching matrix dimensions (-)"};

		Matrix<K> m;
		for (size_t i=0; i < m_rows*m_cols; ++i)
			m[i] = m_data[i] - other[i];
		return m;
	}

	Matrix<K>&
	operator-=(const Matrix<K>& other)
	{
		if (!check_dimensions(other))
			throw std::runtime_error{"mismatching matrix dimensions (-=)"};

		for (size_t i=0; i < m_rows*m_cols; ++i)
			m_data[i] -= other[i];
		return *this;
	}

	Matrix<K>&
	operator-=(const K& lambda)
	{
		for (size_t i=0; i < m_rows*m_cols; ++i)
			m_data[i] -= lambda;
		return *this;
	}

	Matrix<K>
	operator*(const Matrix<K>& other)
	const {
		if (m_cols != other.m_rows)
			throw std::runtime_error{"mismatching matrix dimensions (*)"};

		Matrix<K> m{m_rows, other.m_cols};
		for (size_t i=0; i < m.m_rows; ++i)
			for (size_t j=0; j < m.m_cols; ++j) {
				m(i,j) = K{};
				for (size_t k=0; k < m_cols; ++k)
					m(i,j) += (*this)(i,k) * other(k,j);
			}
		return m;
	}

   Matrix<K>
   operator*(const K& lambda)
   const {
      Matrix<K> m{m_rows, m_cols};
      for (size_t i=0; i < m_rows*m_cols; ++i)
         m[i] = m_data[i] * lambda;
      return m;
   }

   Matrix<K>&
   operator*=(const K& lambda)
   {
		for (size_t i=0; i < m_rows*m_cols; ++i)
			m_data[i] *= lambda;
		return *this;
   }

   Matrix<K>
   operator/(const K& lambda)
   const {
      Matrix<K> m{m_rows, m_cols};
      for (size_t i=0; i < m_rows*m_cols; ++i)
         m[i] = m_data[i] / lambda;
      return m;
   }

   Matrix<K>&
   operator/=(const K& lambda)
   {
		for (size_t i=0; i < m_rows*m_cols; ++i)
			m_data[i] /= lambda;
		return *this;
   }

	size_t
	rows()
	const {
		return m_rows;
	}

	size_t
	cols()
	const {
		return m_cols;
	}

	template<typename T>
	friend T min(const Matrix<T>&);
	template<typename T>
	friend T max(const Matrix<T>&);

private:
	bool
	check_dimensions(const Matrix<K>& other)
	const {
		return m_rows == other.m_rows and m_cols == other.m_cols;
	}

	size_t
	field_index(size_t i, size_t j)
	const {
		return i*m_cols + j;
	}

	size_t m_rows, m_cols;
	K*m_data;
};

template<typename K>
K
min(const Matrix<K>& m)
{
	K p = m[0];
	for (size_t i=1; i < m.m_rows*m.m_cols; ++i)
		p = min(p, m[i]);
	return p;
}

template<typename K>
K
max(const Matrix<K>& m)
{
	K p = m[0];
	for (size_t i=1; i < m.m_rows*m.m_cols; ++i)
		p = max(p, m[i]);
	return p;
}

}
