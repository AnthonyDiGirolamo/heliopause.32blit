#pragma once

#include "32blit.hpp"
#include "graphics/color.hpp"
#include "planet.hpp"
#include <sys/types.h>

#include <span>

static constexpr uint8_t kTerranColors[] = {
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0xd, 0xc, 0xf,
    0xb, 0xb, 0x3, 0x3, 0x3, 0x4, 0x5, 0x6, 0x7,
};

constexpr std::span<const uint8_t> kTerranColorMap(kTerranColors);

// clang-format off
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
// clang-format on

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
  float min_noise_stretch;
  float max_noise_stretch;
  int min_size;
  std::span<const uint8_t> color_map;
};

// clang-format off
Planet kTerranPlanet = {
  .type              = terran,
  .type_name         = "terran",
  .noise_octaves     = 5,
  .noise_zoom        = 0.3,
  .noise_persistance = 0.65,
  .map_icon_color         = 11,
  .full_shadow       = 1,
  .transparent_color = 14,
  .min_noise_stretch = 1.0,
  .max_noise_stretch = 1.0,
  .min_size          = 10,
  .color_map         = kTerranColorMap,
};
// clang-format on

static constexpr uint8_t kTundraColors[] = {
    0x7, 0x6, 0x5, 0x4, 0x5, 0x6, 0x7, 0x6, 0x5, 0x4, 0x3,
};

constexpr std::span<const uint8_t> kTundraColorMap(kTundraColors);

// clang-format off
Planet kTundraPlanet = {
  .type              = tundra,
  .type_name         = "tundra",
  .noise_octaves     = 5,
  .noise_zoom        = 0.5,
  .noise_persistance = 0.6,
  .map_icon_color    = 6,
  .full_shadow       = 1,
  .transparent_color = 14,
  .min_noise_stretch = 1.0,
  .max_noise_stretch = 1.0,
  .min_size          = 10,
  .color_map         = kTundraColorMap,
};
// clang-format on

static constexpr uint8_t kGasGiantRedColors[] = {
    0x7, 0xf, 0xe, 0x2, 0x1, 0x2, 0x8, 0x8,
};

constexpr std::span<const uint8_t> kGasGiantRedColorMap(kGasGiantRedColors);

// clang-format off
Planet kGasGiantRedPlanet = {
  .type              = gas_giant_red,
  .type_name         = "gas giant",
  .noise_octaves     = 1,
  .noise_zoom        = 0.4,
  .noise_persistance = 0.75,
  .map_icon_color    = 8,
  .full_shadow       = 1,
  .transparent_color = 12,
  .min_noise_stretch = 4,
  .max_noise_stretch = 20,
  .min_size          = 20,
  .color_map         = kGasGiantRedColorMap,
};
// clang-format on

Planet *AllPlanets[] = {
    &kTerranPlanet,
    &kTundraPlanet,
    &kGasGiantRedPlanet,
};

std::span<Planet *> PlanetSpan(AllPlanets);
