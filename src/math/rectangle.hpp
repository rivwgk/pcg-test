/* \file rectangle.hpp
 * \brief a rectangle type
 */
#include <algorithm>
#include <optional>
#include <type_traits>

template<typename T>
struct Rectangle {
   static_assert(std::is_arithmetic<T>());
   Rectangle(T l,T r,T b,T t) : left{l},right{r},top{t},bottom{b} {}
   Rectangle(const Rectangle<T>& other) : left{other.left},right{other.right},
                                          top{other.top},bottom{other.bottom} {}

   bool
   contains(const Vector<T,2> value)
   const {
      return l < value[0] and value[0] < r
         and b < value[1] and value[1] < t;
   }
   Vector<T,2>
   contained_random(const Vector<T,2> random)
   const {
      return Vector<T,2>{left + random[0]%(right-left),
                         bottom + random[1]%(top-bottom)};
   }

   template<typename D> friend std::optional<Rectangle<D>>
   operator^(const Rectangle<D>& lhs,const Rectangle<D>& rhs);
private:
   const T left,right,bottom,top;
};

template<typename T> std::optional<Interval<T>>
operator^(const Rectangle<T>& lhs,const Rectangle<T>& rhs)
{
   if (std::min(lhs.right,rhs.right) < std::max(lhs.left,rhs.left) and
       std::min(lhs.top,rhs.top) < std::max(lhs.bottom,rhs.bottom))
      return std::nullopt;
   else
      return Rectangle<T>{std::max(lhs.left,rhs.left),
                          std::min(lhs.right,rhs.right),
                          std::max(lhs.bottom,rhs.bottom),
                          std::min(lhs.top,rhs.top)};
}