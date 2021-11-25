#include "32blit.hpp"
#include "graphics/color.hpp"
#include "SimplexNoise.h"

using namespace blit;

Pen PICO8[] = {
    Pen(0x00, 0x00, 0x00), // 0x0000,  // #000000 0  BLACK
    Pen(0x1d, 0x2b, 0x53), // 0x194a,  // #1d2b53 1  DARK_BLUE
    Pen(0x7e, 0x25, 0x53), // 0x792a,  // #7e2553 2  DARK_PURPLE
    Pen(0x00, 0x87, 0x51), // 0x042a,  // #008751 3  DARK_GREEN
    Pen(0xab, 0x52, 0x36), // 0xaa86,  // #ab5236 4  BROWN
    Pen(0x5f, 0x57, 0x4f), // 0x5aa9,  // #5f574f 5  DARK_GRAY
    Pen(0xc2, 0xc3, 0xc7), // 0xc618,  // #c2c3c7 6  LIGHT_GRAY
    Pen(0xff, 0xf1, 0xe8), // 0xff9d,  // #fff1e8 7  WHITE
    Pen(0xff, 0x00, 0x4d), // 0xf809,  // #ff004d 8  RED
    Pen(0xff, 0xa3, 0x00), // 0xfd00,  // #ffa300 9  ORANGE
    Pen(0xff, 0xec, 0x27), // 0xff64,  // #ffec27 10 YELLOW
    Pen(0x00, 0xe4, 0x36), // 0x0726,  // #00e436 11 GREEN
    Pen(0x29, 0xad, 0xff), // 0x2d7f,  // #29adff 12 BLUE
    Pen(0x83, 0x76, 0x9c), // 0x83b3,  // #83769c 13 INDIGO
    Pen(0xff, 0x77, 0xa8), // 0xfbb5,  // #ff77a8 14 PINK
    Pen(0xff, 0xcc, 0xaa)  // 0xfe75,  // #ffccaa 15 PEACH
};

#define PICO8_BLACK PICO8[0]
#define PICO8_DARK_BLUE PICO8[1]
#define PICO8_DARK_PURPLE PICO8[2]
#define PICO8_DARK_GREEN PICO8[3]
#define PICO8_BROWN PICO8[4]
#define PICO8_DARK_GRAY PICO8[5]
#define PICO8_LIGHT_GRAY PICO8[6]
#define PICO8_WHITE PICO8[7]
#define PICO8_RED PICO8[8]
#define PICO8_ORANGE PICO8[9]
#define PICO8_YELLOW PICO8[10]
#define PICO8_GREEN PICO8[11]
#define PICO8_BLUE PICO8[12]
#define PICO8_INDIGO PICO8[13]
#define PICO8_PINK PICO8[14]
#define PICO8_PEACH PICO8[16]

const Pen ENDESGA32[] = {
    Pen(0xbe, 0x4a, 0x2f), // #be4a2f
    Pen(0xd7, 0x76, 0x43), // #d77643
    Pen(0xea, 0xd4, 0xaa), // #ead4aa
    Pen(0xe4, 0xa6, 0x72), // #e4a672
    Pen(0xb8, 0x6f, 0x50), // #b86f50
    Pen(0x73, 0x3e, 0x39), // #733e39
    Pen(0x3e, 0x27, 0x31), // #3e2731
    Pen(0xa2, 0x26, 0x33), // #a22633
    Pen(0xe4, 0x3b, 0x44), // #e43b44
    Pen(0xf7, 0x76, 0x22), // #f77622
    Pen(0xfe, 0xae, 0x34), // #feae34
    Pen(0xfe, 0xe7, 0x61), // #fee761
    Pen(0x63, 0xc7, 0x4d), // #63c74d
    Pen(0x3e, 0x89, 0x48), // #3e8948
    Pen(0x26, 0x5c, 0x42), // #265c42
    Pen(0x19, 0x3c, 0x3e), // #193c3e
    Pen(0x12, 0x4e, 0x89), // #124e89
    Pen(0x00, 0x99, 0xdb), // #0099db
    Pen(0x2c, 0xe8, 0xf5), // #2ce8f5
    Pen(0xff, 0xff, 0xff), // #ffffff
    Pen(0xc0, 0xcb, 0xdc), // #c0cbdc
    Pen(0x8b, 0x9b, 0xb4), // #8b9bb4
    Pen(0x5a, 0x69, 0x88), // #5a6988
    Pen(0x3a, 0x44, 0x66), // #3a4466
    Pen(0x26, 0x2b, 0x44), // #262b44
    Pen(0x18, 0x14, 0x25), // #181425
    Pen(0xff, 0x00, 0x44), // #ff0044
    Pen(0x68, 0x38, 0x6c), // #68386c
    Pen(0xb5, 0x50, 0x88), // #b55088
    Pen(0xf6, 0x75, 0x7a), // #f6757a
    Pen(0xe8, 0xb7, 0x96), // #e8b796
    Pen(0xc2, 0x85, 0x69)  // #c28569
};

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
