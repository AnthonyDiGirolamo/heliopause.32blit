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
    Pen(0, 0, 0),       // 16
    Pen(15, 22, 42),    // 17
    Pen(63, 19, 42),    // 18
    Pen(0, 68, 41),     // 19
    Pen(86, 41, 27),    // 20
    Pen(48, 44, 40),    // 21
    Pen(97, 98, 100),   // 22
    Pen(128, 121, 116), // 23
    Pen(128, 0, 39),    // 24
    Pen(128, 82, 0),    // 25
    Pen(128, 118, 20),  // 26
    Pen(0, 114, 27),    // 27
    Pen(21, 87, 128),   // 28
    Pen(66, 59, 78),    // 29
    Pen(128, 60, 84),   // 30
    Pen(128, 102, 85),  // 31

    // Pico8 Colors * 0.25 brightness
    Pen(0, 0, 0),    // 32
    Pen(7, 11, 21),  // 33
    Pen(32, 9, 21),  // 34
    Pen(0, 34, 20),  // 35
    Pen(43, 21, 14), // 36
    Pen(24, 22, 20), // 37
    Pen(49, 49, 50), // 38
    Pen(64, 60, 58), // 39
    Pen(64, 0, 19),  // 40
    Pen(64, 41, 0),  // 41
    Pen(64, 59, 10), // 42
    Pen(0, 57, 14),  // 43
    Pen(10, 43, 64), // 44
    Pen(33, 30, 39), // 45
    Pen(64, 30, 42), // 46
    Pen(64, 51, 43), // 47

    // TODO: Figure out why transparent color index isn't reliable
    Pen(255, 0, 255, 0), // 48
    Pen(255, 0, 255, 0), // 49

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

    // Endesga64
    Pen(0xff, 0x00, 0x40), // #ff0040
    Pen(0x13, 0x13, 0x13), // #131313
    Pen(0x1b, 0x1b, 0x1b), // #1b1b1b
    Pen(0x27, 0x27, 0x27), // #272727
    Pen(0x3d, 0x3d, 0x3d), // #3d3d3d
    Pen(0x5d, 0x5d, 0x5d), // #5d5d5d
    Pen(0x85, 0x85, 0x85), // #858585
    Pen(0xb4, 0xb4, 0xb4), // #b4b4b4
    Pen(0xff, 0xff, 0xff), // #ffffff
    Pen(0xc7, 0xcf, 0xdd), // #c7cfdd
    Pen(0x92, 0xa1, 0xb9), // #92a1b9
    Pen(0x65, 0x73, 0x92), // #657392
    Pen(0x42, 0x4c, 0x6e), // #424c6e
    Pen(0x2a, 0x2f, 0x4e), // #2a2f4e
    Pen(0x1a, 0x19, 0x32), // #1a1932
    Pen(0x0e, 0x07, 0x1b), // #0e071b
    Pen(0x1c, 0x12, 0x1c), // #1c121c
    Pen(0x39, 0x1f, 0x21), // #391f21
    Pen(0x5d, 0x2c, 0x28), // #5d2c28
    Pen(0x8a, 0x48, 0x36), // #8a4836
    Pen(0xbf, 0x6f, 0x4a), // #bf6f4a
    Pen(0xe6, 0x9c, 0x69), // #e69c69
    Pen(0xf6, 0xca, 0x9f), // #f6ca9f
    Pen(0xf9, 0xe6, 0xcf), // #f9e6cf
    Pen(0xed, 0xab, 0x50), // #edab50
    Pen(0xe0, 0x74, 0x38), // #e07438
    Pen(0xc6, 0x45, 0x24), // #c64524
    Pen(0x8e, 0x25, 0x1d), // #8e251d
    Pen(0xff, 0x50, 0x00), // #ff5000
    Pen(0xed, 0x76, 0x14), // #ed7614
    Pen(0xff, 0xa2, 0x14), // #ffa214
    Pen(0xff, 0xc8, 0x25), // #ffc825
    Pen(0xff, 0xeb, 0x57), // #ffeb57
    Pen(0xd3, 0xfc, 0x7e), // #d3fc7e
    Pen(0x99, 0xe6, 0x5f), // #99e65f
    Pen(0x5a, 0xc5, 0x4f), // #5ac54f
    Pen(0x33, 0x98, 0x4b), // #33984b
    Pen(0x1e, 0x6f, 0x50), // #1e6f50
    Pen(0x13, 0x4c, 0x4c), // #134c4c
    Pen(0x0c, 0x2e, 0x44), // #0c2e44
    Pen(0x00, 0x39, 0x6d), // #00396d
    Pen(0x00, 0x69, 0xaa), // #0069aa
    Pen(0x00, 0x98, 0xdc), // #0098dc
    Pen(0x00, 0xcd, 0xf9), // #00cdf9
    Pen(0x0c, 0xf1, 0xff), // #0cf1ff
    Pen(0x94, 0xfd, 0xff), // #94fdff
    Pen(0xfd, 0xd2, 0xed), // #fdd2ed
    Pen(0xf3, 0x89, 0xf5), // #f389f5
    Pen(0xdb, 0x3f, 0xfd), // #db3ffd
    Pen(0x7a, 0x09, 0xfa), // #7a09fa
    Pen(0x30, 0x03, 0xd9), // #3003d9
    Pen(0x0c, 0x02, 0x93), // #0c0293
    Pen(0x03, 0x19, 0x3f), // #03193f
    Pen(0x3b, 0x14, 0x43), // #3b1443
    Pen(0x62, 0x24, 0x61), // #622461
    Pen(0x93, 0x38, 0x8f), // #93388f
    Pen(0xca, 0x52, 0xc9), // #ca52c9
    Pen(0xc8, 0x50, 0x86), // #c85086
    Pen(0xf6, 0x81, 0x87), // #f68187
    Pen(0xf5, 0x55, 0x5d), // #f5555d
    Pen(0xea, 0x32, 0x3c), // #ea323c
    Pen(0xc4, 0x24, 0x30), // #c42430
    Pen(0x89, 0x1e, 0x2b), // #891e2b
    Pen(0x57, 0x1c, 0x27), // #571c27

    // Endesga64 * 0.5
    Pen(128, 0, 32),
    Pen(10, 10, 10),
    Pen(14, 14, 14),
    Pen(20, 20, 20),
    Pen(31, 31, 31),
    Pen(47, 47, 47),
    Pen(67, 67, 67),
    Pen(90, 90, 90),
    Pen(128, 128, 128),
    Pen(100, 104, 111),
    Pen(73, 81, 93),
    Pen(51, 58, 73),
    Pen(33, 38, 55),
    Pen(21, 24, 39),
    Pen(13, 13, 25),
    Pen(7, 4, 14),
    Pen(14, 9, 14),
    Pen(29, 16, 17),
    Pen(47, 22, 20),
    Pen(69, 36, 27),
    Pen(96, 56, 37),
    Pen(115, 78, 53),
    Pen(123, 101, 80),
    Pen(125, 115, 104),
    Pen(119, 86, 40),
    Pen(112, 58, 28),
    Pen(99, 35, 18),
    Pen(71, 19, 15),
    Pen(128, 40, 0),
    Pen(119, 59, 10),
    Pen(128, 81, 10),
    Pen(128, 100, 19),
    Pen(128, 118, 44),
    Pen(106, 126, 63),
    Pen(77, 115, 48),
    Pen(45, 99, 40),
    Pen(26, 76, 38),
    Pen(15, 56, 40),
    Pen(10, 38, 38),
    Pen(6, 23, 34),
    Pen(0, 29, 55),
    Pen(0, 53, 85),
    Pen(0, 76, 110),
    Pen(0, 103, 125),
    Pen(6, 121, 128),
    Pen(74, 127, 128),
    Pen(127, 105, 119),
    Pen(122, 69, 123),
    Pen(110, 32, 127),
    Pen(61, 5, 125),
    Pen(24, 2, 109),
    Pen(6, 1, 74),
    Pen(2, 13, 32),
    Pen(30, 10, 34),
    Pen(49, 18, 49),
    Pen(74, 28, 72),
    Pen(101, 41, 101),
    Pen(100, 40, 67),
    Pen(123, 65, 68),
    Pen(123, 43, 47),
    Pen(117, 25, 30),
    Pen(98, 18, 24),
    Pen(69, 15, 22),
    Pen(44, 14, 20),

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
