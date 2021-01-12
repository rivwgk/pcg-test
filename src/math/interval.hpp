#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <type_traits>

template<typename T>
struct Interval {
	static_assert(std::is_arithmetic<T>());
	Interval(T min, T max) : min{min},max{max} {}
	Interval(const Interval<T>& other) : min{other.min},max{other.max} {}

	bool
	contains(const T value)
	const {
		return min <= value and value <= max;
	}

	T
	contained_random(const T random)
	const {
		return random % (max-min) + min;
	}

	T
	lower()
	const {
		return min;
	}

	T
	upper()
	const {
		return max;
	}

	Interval<T>
	operator+(const T& other)
	const {
		return Interval<T>{min+other, max+other};
	}

	Interval<T>
	operator-(const T& other)
	const {
		return Interval<T>{min-other, max-other};
	}

	std::optional<Interval<T>>
	intersect(const Interval<T>& lhs, const Interval<T>& rhs)
	{
		if (std::min<T>(lhs.max,rhs.max) < std::max<T>(lhs.min,rhs.min))
			return std::nullopt;
		else
			return std::optional<Interval<T>>{std::max<T>(lhs.min, rhs.min),
			                                  std::min<T>(lhs.max, rhs.max)};
	}

	T
	size()
	const {
		static_assert(std::is_integral<T>());
		return max-min+1;
	}
private:
	const T min, max;
};
