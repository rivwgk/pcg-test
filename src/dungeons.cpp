/*
 */
#include "math/interval.hpp"
#include "grid.h"

#include <algorithm>
#include <cassert>
#include <random>

struct Dungeon {
};

Dungeon
evolutionary_dungeon(const int λ,const int μ,const int gens)
{
   assert(λ > 0 and μ > 0 and gens > 0);
   const int n = λ+μ;

   for (int i=0; i<n; ++i)
      ; // TODO initialize set
   for (int i=0; i<gens; ++i) {
      // TODO evaluate fitness
      //std::sort(...);
      // TODO keep λ top-most, delete μ remaining
      // TODO let λ mate for μ offspring
      // TODO mutate all n
   }
   return Dungeon{};
}

static Dungeon
BSP_dungeon(const Interval<int> x,const Interval<int> y,const int depth,
            const Interval<int> w,const Interval<int> h,const bool split_hor,
            std::mt19937 g,Dungeon& dungeon)
{
   if (x.size() <= w.lower() and y.size() <= h.lower() or depth == 0) {
      int width = w.contained_random(g());
      int height = h.contained_random(g());
      int xpos = (x-width).contained_random(g());
      int ypos = (y-height).contained_random(g());
   // dungeon.add_room(Rectangle<int>(xpos, ypos, width, height));
   } else {
      if (split_hor) {
         int split_pos = 1+g()%(y.upper()-y.lower());
         BSP_dungeon(x, Interval<int>{y.lower(), split_pos-1}, depth-1, w, h,
                     false, g, dungeon);
         BSP_dungeon(x, Interval<int>{split_pos+1, y.upper()}, depth-1, w, h,
                     false, g, dungeon);
      } else {
         int split_pos = x.contained_random(g());
         BSP_dungeon(Interval<int>{x.lower(), split_pos-1}, y, depth-1, w, h,
                     true, g, dungeon);
         BSP_dungeon(Interval<int>{split_pos+1, x.upper()}, y, depth-1, w, h,
                     true, g, dungeon);
      }
   }
   return Dungeon{};
}

Dungeon
BSP_dungeon(int x_dim, int y_dim, int depth, Interval<int> w, Interval<int> h)
{
   Dungeon d{};
   BSP_dungeon(Interval<int>(0,x_dim), Interval<int>(0,y_dim), depth, w, h,
               true, std::mt19937{std::random_device{}()}, d);
   return d;
}

Dungeon
miner_dungeon(int xdim,int ydim,long seed=0)
{
   assert(xdim > 0 and ydim > 0);
   return Dungeon{};
}

/*
Grid
rasterize(const Dungeon& dungeon)
{
}
*/
