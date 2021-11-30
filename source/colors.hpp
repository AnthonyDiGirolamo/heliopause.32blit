#pragma once

#include "32blit.hpp"
#include "graphics/color.hpp"

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
    Pen(0xff, 0xcc, 0xaa), // 0xfe75,  // #ffccaa 15 PEACH

    // Colors * 0.5
    Pen(0, 0, 0),
    Pen(15, 22, 42),
    Pen(63, 19, 42),
    Pen(0, 68, 41),
    Pen(86, 41, 27),
    Pen(48, 44, 40),
    Pen(97, 98, 100),
    Pen(128, 121, 116),
    Pen(128, 0, 39),
    Pen(128, 82, 0),
    Pen(128, 118, 20),
    Pen(0, 114, 27),
    Pen(21, 87, 128),
    Pen(66, 59, 78),
    Pen(128, 60, 84),
    Pen(128, 102, 85),

    // Colors * 0.25
    Pen(0, 0, 0),
    Pen(7, 11, 21),
    Pen(32, 9, 21),
    Pen(0, 34, 20),
    Pen(43, 21, 14),
    Pen(24, 22, 20),
    Pen(49, 49, 50),
    Pen(64, 60, 58),
    Pen(64, 0, 19),
    Pen(64, 41, 0),
    Pen(64, 59, 10),
    Pen(0, 57, 14),
    Pen(10, 43, 64),
    Pen(33, 30, 39),
    Pen(64, 30, 42),
    Pen(64, 51, 43),
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

Pen ENDESGA32[] = {
    Pen(0x00, 0x00, 0x00), // 0,  // #000000
    Pen(0xbe, 0x4a, 0x2f), // 1,  // #be4a2f
    Pen(0xd7, 0x76, 0x43), // 2   // #d77643
    Pen(0xea, 0xd4, 0xaa), // 3,  // #ead4aa
    Pen(0xe4, 0xa6, 0x72), // 4,  // #e4a672
    Pen(0xb8, 0x6f, 0x50), // 5,  // #b86f50
    Pen(0x73, 0x3e, 0x39), // 6,  // #733e39
    Pen(0x3e, 0x27, 0x31), // 7,  // #3e2731
    Pen(0xa2, 0x26, 0x33), // 8,  // #a22633
    Pen(0xe4, 0x3b, 0x44), // 9,  // #e43b44
    Pen(0xf7, 0x76, 0x22), // 10, // #f77622
    Pen(0xfe, 0xae, 0x34), // 11, // #feae34
    Pen(0xfe, 0xe7, 0x61), // 12, // #fee761
    Pen(0x63, 0xc7, 0x4d), // 13, // #63c74d
    Pen(0x3e, 0x89, 0x48), // 14, // #3e8948
    Pen(0x26, 0x5c, 0x42), // 15, // #265c42
    Pen(0x19, 0x3c, 0x3e), // 16, // #193c3e
    Pen(0x12, 0x4e, 0x89), // 17, // #124e89
    Pen(0x00, 0x99, 0xdb), // 18, // #0099db
    Pen(0x2c, 0xe8, 0xf5), // 19, // #2ce8f5
    Pen(0xff, 0xff, 0xff), // 20, // #ffffff
    Pen(0xc0, 0xcb, 0xdc), // 21, // #c0cbdc
    Pen(0x8b, 0x9b, 0xb4), // 22, // #8b9bb4
    Pen(0x5a, 0x69, 0x88), // 23, // #5a6988
    Pen(0x3a, 0x44, 0x66), // 24, // #3a4466
    Pen(0x26, 0x2b, 0x44), // 25, // #262b44
    Pen(0x18, 0x14, 0x25), // 26, // #181425
    Pen(0xff, 0x00, 0x44), // 27, // #ff0044
    Pen(0x68, 0x38, 0x6c), // 28, // #68386c
    Pen(0xb5, 0x50, 0x88), // 29, // #b55088
    Pen(0xf6, 0x75, 0x7a), // 30, // #f6757a
    Pen(0xe8, 0xb7, 0x96), // 31, // #e8b796
    Pen(0xc2, 0x85, 0x69)  // 32, // #c28569
};
