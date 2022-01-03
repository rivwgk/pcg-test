#pragma once

#include <functional>
#include <complex>
#include <Eigen/Dense>

using R2RFunction = std::function<double(double)>;

using Realfield2D = Eigen::ArrayXX<double>;
using Vectorfield2D = Eigen::ArrayXX<Eigen::Vector2d>;
using Complexfield2D = Eigen::ArrayXX<std::complex<double>>;

template<typename T>
using Field2D = Eigen::ArrayXX<T>;
