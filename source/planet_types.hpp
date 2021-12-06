#pragma once

#include "32blit.hpp"
#include "graphics/color.hpp"
#include "planet.hpp"
#include <sys/types.h>

#include <span>

/* Color Palette

  0,  // #000000 BLACK
  1,  // #1d2b53 DARK_BLUE
  2,  // #7e2553 DARK_PURPLE
  3,  // #008751 DARK_GREEN
  4,  // #ab5236 BROWN
  5,  // #5f574f DARK_GRAY
  6,  // #c2c3c7 LIGHT_GRAY
  7,  // #fff1e8 WHITE
  8,  // #ff004d RED
  9,  // #ffa300 ORANGE
  10, // #ffec27 YELLOW
  11, // #00e436 GREEN
  12, // #29adff BLUE
  13, // #83769c INDIGO
  14, // #ff77a8 PINK
  15, // #ffccaa PEACH

  50, // #000000
  51, // #be4a2f
  52, // #d77643
  53, // #ead4aa
  54, // #e4a672
  55, // #b86f50
  56, // #733e39
  57, // #3e2731
  58, // #a22633
  59, // #e43b44
  60, // #f77622
  61, // #feae34
  62, // #fee761
  63, // #63c74d
  64, // #3e8948
  65, // #265c42
  66, // #193c3e
  67, // #124e89
  68, // #0099db
  69, // #2ce8f5
  70, // #ffffff
  71, // #c0cbdc
  72, // #8b9bb4
  73, // #5a6988
  74, // #3a4466
  75, // #262b44
  76, // #181425
  77, // #ff0044
  78, // #68386c
  79, // #b55088
  80, // #f6757a
  81, // #e8b796
  82, // #c28569

*/

static constexpr uint8_t kTerranColors[] = {

  // pico8
  1,  // #1d2b53 DARK_BLUE
  1,  // #1d2b53 DARK_BLUE
  1,  // #1d2b53 DARK_BLUE
  1,  // #1d2b53 DARK_BLUE
  1,  // #1d2b53 DARK_BLUE
  1,  // #1d2b53 DARK_BLUE
  13, // #83769c INDIGO
  12, // #29adff BLUE
  15, // #ffccaa PEACH
  11, // #00e436 GREEN
  11, // #00e436 GREEN
  3,  // #008751 DARK_GREEN
  3,  // #008751 DARK_GREEN
  3,  // #008751 DARK_GREEN
  4,  // #ab5236 BROWN
  5,  // #5f574f DARK_GRAY
  6,  // #c2c3c7 LIGHT_GRAY
  7,  // #fff1e8 WHITE

  // // endesga32
  // 75, // #262b44
  // 67, // #124e89
  // 68, // #0099db
  // 69, // #2ce8f5
  // 53, // #ead4aa
  // 63, // #63c74d
  // 64, // #3e8948
  // 65, // #265c42
  // 56, // #733e39
  // 55, // #b86f50
  // 71, // #c0cbdc
  // 70, // #ffffff

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
    // pico8
    // 4,
    // 4,
    // 9,
    // 9,
    // 4,
    // 4,
    // 9,
    // 9,
    // 4,
    // 4,
    // 9,
    // 9,
    // 11,
    // 1,
    // 9,
    // 4,
    // 9,
    // 9,
    // 4,
    // 9,
    // 9,
    // 4,
    // 9,
    // 9,
    // 4,
    // 9,
    // 9,
    // 4,
    // 9,

    // endesga32
    53, // #ead4aa
    81, // #e8b796
    54, // #e4a672
    81, // #e8b796
    53, // #ead4aa
    53, // #ead4aa
    81, // #e8b796
    54, // #e4a672
    52, // #d77643
    51, // #be4a2f
    82, // #c28569
    55, // #b86f50
    56, // #733e39
    57, // #3e2731
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
  .type_string       = std::string_view {"Terran"},
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
  .palette_dark_offset = 16,
};


PlanetTerrain kTundraPlanet = {
  .type              = tundra,
  .type_string       = std::string_view {"Tundra"},
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
  .palette_dark_offset = 16,
};


PlanetTerrain kGasGiantRedPlanet = {
  .type              = gas_giant_red,
  .type_string       = std::string_view {"Red Giant"},
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
  .palette_dark_offset = 16,
};

PlanetTerrain kGasGiantBluePlanet = {
  .type              = gas_giant_blue,
  .type_string       = std::string_view {"Blue Giant"},
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
  .palette_dark_offset = 16,
};

PlanetTerrain kGasGiantYellowPlanet = {
  .type              = gas_giant_yellow,
  .type_string       = std::string_view {"Yellow Giant"},
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
  .palette_dark_offset = 16,
};

PlanetTerrain kGasGiantRainbowPlanet = {
  .type              = gas_giant_rainbow,
  .type_string       = std::string_view {"Rainbow Giant"},
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
  .palette_dark_offset = 16,
};

PlanetTerrain kIslandPlanet = {
  .type              = island,
  .type_string       = std::string_view {"Island"},
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
  .palette_dark_offset = 16,
};


PlanetTerrain kBarrenPlanet = {
  .type              = barren,
  .type_string       = std::string_view {"Barren"},
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
  .palette_dark_offset = 16,
};

PlanetTerrain kDesertPlanet = {
  .type              = desert,
  .type_string       = std::string_view {"Desert"},
  .noise_octaves     = 5,
  .noise_zoom        = 1.0,
  .noise_persistance = 0.65,
  .map_icon_color    = 9,
  .full_shadow       = 1,
  .transparent_color = 14,
  .min_noise_stretch = 1,
  .max_noise_stretch = 1,
  .min_size          = 10,
  .color_map         = kDesertColorMap,
  .color_padding_start = 10,
  .color_padding_end = 5,
  .palette_dark_offset = 33,
};

PlanetTerrain kLavaPlanet = {
  .type              = lava,
  .type_string       = std::string_view {"Lava"},
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
  .palette_dark_offset = 16,
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
