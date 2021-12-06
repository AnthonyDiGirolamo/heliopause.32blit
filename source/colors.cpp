#include "colors.hpp"
#include "32blit.hpp"
#include "graphics/color.hpp"

using namespace blit;

Pen PICO8[] = {
    // Standard Pico 8 colors
    Pen(0x00, 0x00, 0x00), // 0x0000, 0,  // #000000 BLACK
    Pen(0x1d, 0x2b, 0x53), // 0x194a, 1,  // #1d2b53 DARK_BLUE
    Pen(0x7e, 0x25, 0x53), // 0x792a, 2,  // #7e2553 DARK_PURPLE
    Pen(0x00, 0x87, 0x51), // 0x042a, 3,  // #008751 DARK_GREEN
    Pen(0xab, 0x52, 0x36), // 0xaa86, 4,  // #ab5236 BROWN
    Pen(0x5f, 0x57, 0x4f), // 0x5aa9, 5,  // #5f574f DARK_GRAY
    Pen(0xc2, 0xc3, 0xc7), // 0xc618, 6,  // #c2c3c7 LIGHT_GRAY
    Pen(0xff, 0xf1, 0xe8), // 0xff9d, 7,  // #fff1e8 WHITE
    Pen(0xff, 0x00, 0x4d), // 0xf809, 8,  // #ff004d RED
    Pen(0xff, 0xa3, 0x00), // 0xfd00, 9,  // #ffa300 ORANGE
    Pen(0xff, 0xec, 0x27), // 0xff64, 10, // #ffec27 YELLOW
    Pen(0x00, 0xe4, 0x36), // 0x0726, 11, // #00e436 GREEN
    Pen(0x29, 0xad, 0xff), // 0x2d7f, 12, // #29adff BLUE
    Pen(0x83, 0x76, 0x9c), // 0x83b3, 13, // #83769c INDIGO
    Pen(0xff, 0x77, 0xa8), // 0xfbb5, 14, // #ff77a8 PINK
    Pen(0xff, 0xcc, 0xaa), // 0xfe75, 15, // #ffccaa PEACH

    // Pico8 Colors * 0.5 brightness
    Pen(0,   0,   0),   // 16
    Pen(15,  22,  42),  // 17
    Pen(63,  19,  42),  // 18
    Pen(0,   68,  41),  // 19
    Pen(86,  41,  27),  // 20
    Pen(48,  44,  40),  // 21
    Pen(97,  98,  100), // 22
    Pen(128, 121, 116), // 23
    Pen(128, 0,   39),  // 24
    Pen(128, 82,  0),   // 25
    Pen(128, 118, 20),  // 26
    Pen(0,   114, 27),  // 27
    Pen(21,  87,  128), // 28
    Pen(66,  59,  78),  // 29
    Pen(128, 60,  84),  // 30
    Pen(128, 102, 85),  // 31

    // Pico8 Colors * 0.25 brightness
    Pen(0,  0,  0),  // 32
    Pen(7,  11, 21), // 33
    Pen(32, 9,  21), // 34
    Pen(0,  34, 20), // 35
    Pen(43, 21, 14), // 36
    Pen(24, 22, 20), // 37
    Pen(49, 49, 50), // 38
    Pen(64, 60, 58), // 39
    Pen(64, 0,  19), // 40
    Pen(64, 41, 0),  // 41
    Pen(64, 59, 10), // 42
    Pen(0,  57, 14), // 43
    Pen(10, 43, 64), // 44
    Pen(33, 30, 39), // 45
    Pen(64, 30, 42), // 46
    Pen(64, 51, 43), // 47

    // TODO: Figure out why transparent color index isn't reliable
    Pen(255, 0, 255, 0),  // 48
    Pen(255, 0, 255, 0),  // 49

    // Endesga32 Colors
    Pen(0x00, 0x00, 0x00), // 50, // #000000
    Pen(0xbe, 0x4a, 0x2f), // 51, // #be4a2f
    Pen(0xd7, 0x76, 0x43), // 52  // #d77643
    Pen(0xea, 0xd4, 0xaa), // 53, // #ead4aa
    Pen(0xe4, 0xa6, 0x72), // 54, // #e4a672
    Pen(0xb8, 0x6f, 0x50), // 55, // #b86f50
    Pen(0x73, 0x3e, 0x39), // 56, // #733e39
    Pen(0x3e, 0x27, 0x31), // 57, // #3e2731
    Pen(0xa2, 0x26, 0x33), // 58, // #a22633
    Pen(0xe4, 0x3b, 0x44), // 59, // #e43b44
    Pen(0xf7, 0x76, 0x22), // 60, // #f77622
    Pen(0xfe, 0xae, 0x34), // 61, // #feae34
    Pen(0xfe, 0xe7, 0x61), // 62, // #fee761
    Pen(0x63, 0xc7, 0x4d), // 63, // #63c74d
    Pen(0x3e, 0x89, 0x48), // 64, // #3e8948
    Pen(0x26, 0x5c, 0x42), // 65, // #265c42
    Pen(0x19, 0x3c, 0x3e), // 66, // #193c3e
    Pen(0x12, 0x4e, 0x89), // 67, // #124e89
    Pen(0x00, 0x99, 0xdb), // 68, // #0099db
    Pen(0x2c, 0xe8, 0xf5), // 69, // #2ce8f5
    Pen(0xff, 0xff, 0xff), // 70, // #ffffff
    Pen(0xc0, 0xcb, 0xdc), // 71, // #c0cbdc
    Pen(0x8b, 0x9b, 0xb4), // 72, // #8b9bb4
    Pen(0x5a, 0x69, 0x88), // 73, // #5a6988
    Pen(0x3a, 0x44, 0x66), // 74, // #3a4466
    Pen(0x26, 0x2b, 0x44), // 75, // #262b44
    Pen(0x18, 0x14, 0x25), // 76, // #181425
    Pen(0xff, 0x00, 0x44), // 77, // #ff0044
    Pen(0x68, 0x38, 0x6c), // 78, // #68386c
    Pen(0xb5, 0x50, 0x88), // 79, // #b55088
    Pen(0xf6, 0x75, 0x7a), // 80, // #f6757a
    Pen(0xe8, 0xb7, 0x96), // 81, // #e8b796
    Pen(0xc2, 0x85, 0x69), // 82, // #c28569

    // Endesga32 Colors * 0.5 brightness
    Pen(0, 0, 0),
    Pen(95, 37, 24),
    Pen(108, 59, 34),
    Pen(117, 106, 85),
    Pen(114, 83, 57),
    Pen(92, 56, 40),
    Pen(58, 31, 29),
    Pen(31, 20, 25),
    Pen(81, 19, 26),
    Pen(114, 30, 34),
    Pen(124, 59, 17),
    Pen(127, 87, 26),
    Pen(127, 116, 49),
    Pen(50, 100, 39),
    Pen(31, 69, 36),
    Pen(19, 46, 33),
    Pen(13, 30, 31),
    Pen(9, 39, 69),
    Pen(0, 77, 110),
    Pen(22, 116, 123),
    Pen(128, 128, 128),
    Pen(96, 102, 110),
    Pen(70, 78, 90),
    Pen(45, 53, 68),
    Pen(29, 34, 51),
    Pen(19, 22, 34),
    Pen(12, 10, 19),
    Pen(128, 0, 34),
    Pen(52, 28, 54),
    Pen(91, 40, 68),
    Pen(123, 59, 61),
    Pen(116, 92, 75),
    Pen(97, 67, 53),
};

/*
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
*/
