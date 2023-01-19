#pragma once

#include "graphics/color.hpp"

using namespace blit;

extern Pen PICO8[];
extern Pen ENDESGA32[];
extern Pen ENDESGA64[];

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

enum Endesga64Colors {
  Blood,         // 00 #ff0040
  Black0,        // 01 #131313
  Black1,        // 02 #1b1b1b
  Gray0,         // 03 #272727
  Gray1,         // 04 #3d3d3d
  Gray2,         // 05 #5d5d5d
  Gray3,         // 06 #858585
  Gray4,         // 07 #b4b4b4
  White,         // 08 #ffffff
  Steel6,        // 09 #c7cfdd
  Steel5,        // 10 #92a1b9
  Steel4,        // 11 #657392
  Steel3,        // 12 #424c6e
  Steel2,        // 13 #2a2f4e
  Steel1,        // 14 #1a1932
  Steel0,        // 15 #0e071b
  Coffee0,       // 16 #1c121c
  Coffee1,       // 17 #391f21
  Coffee2,       // 18 #5d2c28
  Coffee3,       // 19 #8a4836
  Coffee4,       // 20 #bf6f4a
  Coffee5,       // 21 #e69c69
  Coffee6,       // 22 #f6ca9f
  Coffee7,       // 23 #f9e6cf
  Orange3,       // 24 #edab50
  Orange2,       // 25 #e07438
  Orange1,       // 26 #c64524
  Orange0,       // 27 #8e251d
  BrightOrange0, // 28 #ff5000
  BrightOrange1, // 29 #ed7614
  BrightOrange2, // 30 #ffa214
  Yellow0,       // 31 #ffc825
  Yellow1,       // 32 #ffeb57
  Green5,        // 33 #d3fc7e
  Green4,        // 34 #99e65f
  Green3,        // 35 #5ac54f
  Green2,        // 36 #33984b
  Green1,        // 37 #1e6f50
  Green0,        // 38 #134c4c
  Ocean0,        // 39 #0c2e44
  Ocean1,        // 40 #00396d
  Ocean2,        // 41 #0069aa
  Ocean3,        // 42 #0098dc
  Ocean4,        // 43 #00cdf9
  Ocean5,        // 44 #0cf1ff
  Ocean6,        // 45 #94fdff
  CandyGrape3,   // 46 #fdd2ed
  CandyGrape2,   // 47 #f389f5
  CandyGrape1,   // 48 #db3ffd
  CandyGrape0,   // 49 #7a09fa
  RoyalBlue2,    // 50 #3003d9
  RoyalBlue1,    // 51 #0c0293
  RoyalBlue0,    // 52 #03193f
  Purple0,       // 53 #3b1443
  Purple1,       // 54 #622461
  Purple2,       // 55 #93388f
  Purple3,       // 56 #ca52c9
  Salmon0,       // 57 #c85086
  Salmon1,       // 58 #f68187
  Red4,          // 59 #f5555d
  Red3,          // 60 #ea323c
  Red2,          // 61 #c42430
  Red1,          // 62 #891e2b
  Red0,          // 63 #571c27
};
