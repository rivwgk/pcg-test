#pragma once

#include <algorithm>

namespace math {

template<typename K>
const K&
min(const K& a, const K& b)
{
   return std::min(a, b);
}

template<typename K>
const K&
max(const K& a, const K& b)
{
   return std::max(a, b);
}

}
