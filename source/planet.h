#pragma once

#include "32blit.hpp"
#include "graphics/color.hpp"

enum PlanetType {
  tundra,
  desert,
  barren,
  lava,
  terran,
  island,
  gas_giant1,
  gas_giant2,
  gas_giant3,
  rainbow_giant,
};

struct Planet {
  PlanetType type = terran;
  int noise_octaves = 5;
  float noise_zoom = 0.3;
  float noise_persistance = 0.65;
  Pen map_color = PICO8[11];
  uint8_t full_shadow = 1;
  Pen transparent_color = PICO8[14];
  float min_noise_stretch = 1.0;
  float max_noise_stretch = 1.0;
  int min_size = 10;
};

uint8_t terran_color_map[] = {
  0x1,
  0x1,
  0x1,
  0x1,
  0x1,
  0x1,
  0x1,
  0xd,
  0xc,
  0xf,
  0xb,
  0xb,
  0x3,
  0x3,
  0x3,
  0x4,
  0x5,
  0x6,
  0x7,
};

// type_name,     noise_octaves, noise_zoom, noise_persistance, map_color, full_shadow, transparent_color, min_noise_stretch_factor, max_noise_stretch_factor, min_size, color_map
// terran,        5,             .3,         .65,               11,        1,           14,                1,                        1,                        10,       1111111dcfbb3334567,
// tundra,        5,             .5,         .6,                6,         1,           14,                1,                        1,                        10,       76545676543,
// desert,        5,             .35,        .3,                9,         1,           14,                1,                        1,                        10,       449944994499b1949949949949949,
// barren,        5,             .55,        .35,               5,         1,           14,                1,                        1,                        10,       565056765056,
// lava,          5,             .55,        .65,               4,         1,           14,                1,                        1,                        10,       040504049840405040,
// island,        5,             .55,        .65,               12,        1,           14,                1,                        1,                        10,       11111111dcfb3,
// gas giant,     1,             .4,         .75,               2,         1,           14,                4,                        20,                       20,       76d121c,
// gas giant,     1,             .4,         .75,               8,         1,           12,                4,                        20,                       20,       7fe21288,
// gas giant,     1,             .7,         .75,               10,        1,           14,                4,                        20,                       20,       fa949a,
// rainbow giant, 1,             .7,         .75,               15,        1,           4,                 4,                        20,                       20,       1dcba9e82,
