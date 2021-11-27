#pragma once

#include "32blit.hpp"
#include "graphics/color.hpp"
#include "planet.hpp"
#include <sys/types.h>

#include <span>

enum PlanetType {
  tundra,
  desert,
  barren,
  lava,
  terran,
  island,
  gas_giant_red,
  gas_giant_purple,
  gas_giant_yellow,
  gas_giant_rainbow,
};

struct Planet {
  PlanetType type;
  char *type_name;
  int noise_octaves;
  float noise_zoom;
  float noise_persistance;
  uint8_t map_icon_color;
  uint8_t full_shadow;
  uint8_t transparent_color;
  int min_noise_stretch;
  int max_noise_stretch;
  int min_size;
  std::span<const uint8_t> color_map;
};
