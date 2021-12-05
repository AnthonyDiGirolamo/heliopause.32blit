#pragma once

#include "graphics/font.hpp"

// #define SCREEN_MODE_HIRES 1
// #define SCREEN_MODE_LORES 1

#ifdef SCREEN_MODE_HIRES
#define PLANET_WIDTH 240
#define PLANET_HEIGHT 240
#else
#define PLANET_WIDTH 120
#define PLANET_HEIGHT 120
#endif

extern const blit::Font custom_font;
