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

static constexpr uint8_t kLavaColors[] = {
    0x0, 0x4, 0x0, 0x5, 0x0, 0x4, 0x0, 0x4, 0x9,
    0x8, 0x4, 0x0, 0x4, 0x0, 0x5, 0x0, 0x4, 0x0,
};

constexpr std::span<const uint8_t> kLavaColorMap(kLavaColors);

static constexpr uint8_t kIslandColors[] = {
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0xd, 0xc, 0xf, 0xb, 0x3,
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

static constexpr uint8_t kDesertColors[] = {
    0x4, 0x4, 0x9, 0x9, 0x4, 0x4, 0x9, 0x9, 0x4, 0x4, 0x9, 0x9, 0xb, 0x1, 0x9,
    0x4, 0x9, 0x9, 0x4, 0x9, 0x9, 0x4, 0x9, 0x9, 0x4, 0x9, 0x9, 0x4, 0x9,
};

constexpr std::span<const uint8_t> kDesertColorMap(kDesertColors);

static constexpr uint8_t kGasGiantRedColors[] = {
    0x7, 0xf, 0xe, 0x2, 0x1, 0x2, 0x8, 0x8,
};
constexpr std::span<const uint8_t> kGasGiantRedColorMap(kGasGiantRedColors);

static constexpr uint8_t kGasGiantBlueColors[] = {
    0x7, 0x6, 0xd, 0x1, 0x2, 0x1, 0xc,
};
constexpr std::span<const uint8_t> kGasGiantBlueColorMap(kGasGiantBlueColors);

static constexpr uint8_t kGasGiantYellowColors[] = {
    0xf, 0xa, 0x9, 0x4, 0x9, 0xa,
};
constexpr std::span<const uint8_t>
    kGasGiantYellowColorMap(kGasGiantYellowColors);

static constexpr uint8_t kGasGiantRainbowColors[] = {
    0x1, 0xd, 0xc, 0xb, 0xa, 0x9, 0xe, 0x8, 0x2,
};

constexpr std::span<const uint8_t>
    kGasGiantRainbowColorMap(kGasGiantRainbowColors);

// clang-format off
PlanetTerrain kTerranPlanet = {
  .type              = terran,
  .type_string       = std::string_view {"terran"},
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
  .color_padding_start = 7,
  .color_padding_end   = 7,
};


PlanetTerrain kTundraPlanet = {
  .type              = tundra,
  .type_string       = std::string_view {"tundra"},
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
  .color_padding_start = 7,
  .color_padding_end = 7,
};


PlanetTerrain kGasGiantRedPlanet = {
  .type              = gas_giant_red,
  .type_string       = std::string_view {"gas giant red"},
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
  .color_padding_start = 0,
  .color_padding_end = 0,
};

PlanetTerrain kGasGiantBluePlanet = {
  .type              = gas_giant_blue,
  .type_string       = std::string_view {"gas giant blue"},
  .noise_octaves     = 1,
  .noise_zoom        = 0.4,
  .noise_persistance = 0.75,
  .map_icon_color    = 2,
  .full_shadow       = 1,
  .transparent_color = 14,
  .min_noise_stretch = 4,
  .max_noise_stretch = 20,
  .min_size          = 20,
  .color_map         = kGasGiantBlueColorMap,
  .color_padding_start = 0,
  .color_padding_end = 0,
};

PlanetTerrain kGasGiantYellowPlanet = {
  .type              = gas_giant_yellow,
  .type_string       = std::string_view {"gas giant yellow"},
  .noise_octaves     = 1,
  .noise_zoom        = 0.7,
  .noise_persistance = 0.75,
  .map_icon_color    = 10,
  .full_shadow       = 1,
  .transparent_color = 14,
  .min_noise_stretch = 4,
  .max_noise_stretch = 20,
  .min_size          = 20,
  .color_map         = kGasGiantYellowColorMap,
  .color_padding_start = 0,
  .color_padding_end = 0,
};

PlanetTerrain kGasGiantRainbowPlanet = {
  .type              = gas_giant_rainbow,
  .type_string       = std::string_view {"gas giant rainbow"},
  .noise_octaves     = 1,
  .noise_zoom        = 0.7,
  .noise_persistance = 0.75,
  .map_icon_color    = 15,
  .full_shadow       = 1,
  .transparent_color = 4,
  .min_noise_stretch = 4,
  .max_noise_stretch = 20,
  .min_size          = 20,
  .color_map         = kGasGiantRainbowColorMap,
  .color_padding_start = 0,
  .color_padding_end = 0,
};

PlanetTerrain kIslandPlanet = {
  .type              = island,
  .type_string       = std::string_view {"island"},
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
  .color_padding_start = 7,
  .color_padding_end = 4,
};


PlanetTerrain kBarrenPlanet = {
  .type              = barren,
  .type_string       = std::string_view {"barren"},
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
  .color_padding_start = 7,
  .color_padding_end = 7,
};

PlanetTerrain kDesertPlanet = {
  .type              = desert,
  .type_string       = std::string_view {"desert"},
  .noise_octaves     = 1,
  .noise_zoom        = 0.35,
  .noise_persistance = 0.3,
  .map_icon_color    = 9,
  .full_shadow       = 1,
  .transparent_color = 14,
  .min_noise_stretch = 1,
  .max_noise_stretch = 1,
  .min_size          = 10,
  .color_map         = kDesertColorMap,
  .color_padding_start = 5,
  .color_padding_end = 5,
};

PlanetTerrain kLavaPlanet = {
  .type              = lava,
  .type_string       = std::string_view {"lava"},
  .noise_octaves     = 5,
  .noise_zoom        = 0.55,
  .noise_persistance = 0.65,
  .map_icon_color    = 4,
  .full_shadow       = 1,
  .transparent_color = 14,
  .min_noise_stretch = 1,
  .max_noise_stretch = 1,
  .min_size          = 10,
  .color_map         = kLavaColorMap,
  .color_padding_start = 5,
  .color_padding_end = 5,
};

PlanetTerrain AllPlanets[] = {
    kTerranPlanet,
    kTundraPlanet,
    kBarrenPlanet,
    kIslandPlanet,
    kDesertPlanet,
    kLavaPlanet,
    kGasGiantRedPlanet,
    kGasGiantBluePlanet,
    kGasGiantYellowPlanet,
    kGasGiantRainbowPlanet,
};

std::span<PlanetTerrain> PlanetSpan(AllPlanets);


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
