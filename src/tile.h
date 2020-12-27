#include <SFML/Graphics.hpp>

#pragma once

enum struct BiomeType {
   Tundra,
   Taiga,
   Nival_Coast,
   Temperate_Forest,
   Temperate_Coast,
   Temperate_Desert,
   Temperate_Shrubland,
   Temperate_Swamp,
   Tropical_Forest,
   Tropical_Coast,
   Tropical_Desert,
   Tropical_Shrubland,
   Tropical_Swamp,
   Mountains,
   Deep_Ocean,
   Shallow_Ocean,
   River,
   _Last,
};

constexpr sf::Color BiomeColours[static_cast<char>(BiomeType::_Last)] = {

};

struct Tile {
   Tile();
   Tile(const Tile& other);
   Tile& operator=(const Tile& other);

   union {
      struct {
         double height;
         double temperature;
         double humidity;
      };
      double values[3];
   };
   double grad[2];
};
