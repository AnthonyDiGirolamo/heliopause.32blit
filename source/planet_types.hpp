#pragma once

#include "32blit.hpp"
#include "graphics/color.hpp"
#include "planet.hpp"
#include <sys/types.h>

#include <span>

static constexpr uint8_t kTerranColors[] = {
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x1, 0x1, 0x1, 0xd, 0xc, 0xf, 0xb, 0xb, 0x3, 0x3, 0x3,
    0x4, 0x5, 0x6, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
};

constexpr std::span<const uint8_t> kTerranColorMap(kTerranColors);

static constexpr uint8_t kIslandColors[] = {
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x1, 0xd, 0xc, 0xf, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3,
};

constexpr std::span<const uint8_t> kIslandColorMap(kIslandColors);

static constexpr uint8_t kTundraColors[] = {
    0x7, 0x6, 0x5, 0x4, 0x5, 0x6, 0x7, 0x6, 0x5, 0x4, 0x3,
};

constexpr std::span<const uint8_t> kTundraColorMap(kTundraColors);

static constexpr uint8_t kBarrenColors[] = {
    0x5, 0x6, 0x5, 0x0, 0x5, 0x6, 0x7, 0x6, 0x5, 0x0, 0x5, 0x6,
};

constexpr std::span<const uint8_t> kBarrenColorMap(kBarrenColors);

static constexpr uint8_t kGasGiantRedColors[] = {
    0x7, 0xf, 0xe, 0x2, 0x1, 0x2, 0x8, 0x8,
};
static constexpr uint8_t kGasGiantBlueColors[] = {
    0x7, 0x6, 0xd, 0x1, 0x2, 0x1, 0xc,
};
static constexpr uint8_t kGasGiantYellowColors[] = {
    0xf, 0xa, 0x9, 0x4, 0x9, 0xa,
};

constexpr std::span<const uint8_t> kGasGiantRedColorMap(kGasGiantRedColors);

// clang-format off
Planet kTerranPlanet = {
  .type              = terran,
  .type_name         = "terran",
  .noise_octaves     = 5,
  .noise_zoom        = 0.5,
  .noise_persistance = 0.65,
  .map_icon_color    = 11,
  .full_shadow       = 1,
  .transparent_color = 14,
  .min_noise_stretch = 1,
  .max_noise_stretch = 1,
  .min_size          = 10,
  .color_map         = kTerranColorMap,
};


Planet kTundraPlanet = {
  .type              = tundra,
  .type_name         = "tundra",
  .noise_octaves     = 5,
  .noise_zoom        = 0.5,
  .noise_persistance = 0.6,
  .map_icon_color    = 6,
  .full_shadow       = 1,
  .transparent_color = 14,
  .min_noise_stretch = 1,
  .max_noise_stretch = 1,
  .min_size          = 10,
  .color_map         = kTundraColorMap,
};


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


Planet kIslandPlanet = {
  .type              = island,
  .type_name         = "island",
  .noise_octaves     = 5,
  .noise_zoom        = 0.55,
  .noise_persistance = 0.65,
  .map_icon_color    = 4,
  .full_shadow       = 1,
  .transparent_color = 14,
  .min_noise_stretch = 1,
  .max_noise_stretch = 1,
  .min_size          = 10,
  .color_map         = kIslandColorMap,
};


Planet kBarrenPlanet = {
  .type              = barren,
  .type_name         = "barren",
  .noise_octaves     = 5,
  .noise_zoom        = 0.55,
  .noise_persistance = 0.35,
  .map_icon_color    = 5,
  .full_shadow       = 1,
  .transparent_color = 14,
  .min_noise_stretch = 1,
  .max_noise_stretch = 1,
  .min_size          = 10,
  .color_map         = kBarrenColorMap,
};


Planet *AllPlanets[] = {
    &kTerranPlanet,      &kTundraPlanet, &kBarrenPlanet,
    &kGasGiantRedPlanet, &kIslandPlanet,
};

std::span<Planet *> PlanetSpan(AllPlanets);


// type_name,     noise_octaves, noise_zoom, noise_persistance, map_color, full_shadow, transparent_color, min_noise_stretch_factor, max_noise_stretch_factor, min_size, color_map
// terran,        5,             .3,         .65,               11,        1,           14,                1,                        1,                        10,       1111111dcfbb3334567,
// tundra,        5,             .5,         .6,                6,         1,           14,                1,                        1,                        10,       76545676543,
// island,        5,             .55,        .65,               12,        1,           14,                1,                        1,                        10,       11111111dcfb3,
// desert,        5,             .35,        .3,                9,         1,           14,                1,                        1,                        10,       449944994499b1949949949949949,
// barren,        5,             .55,        .35,               5,         1,           14,                1,                        1,                        10,       565056765056,
// lava,          5,             .55,        .65,               4,         1,           14,                1,                        1,                        10,       040504049840405040,
// gas giant,     1,             .4,         .75,               2,         1,           14,                4,                        20,                       20,       76d121c,
// gas giant,     1,             .4,         .75,               8,         1,           12,                4,                        20,                       20,       7fe21288,
// gas giant,     1,             .7,         .75,               10,        1,           14,                4,                        20,                       20,       fa949a,
// rainbow giant, 1,             .7,         .75,               15,        1,           4,                 4,                        20,                       20,       1dcba9e82,
// clang-format on
