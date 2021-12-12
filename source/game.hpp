#pragma once

#include "graphics/font.hpp"

#define SCREEN_MODE_HIRES 1

#ifdef SCREEN_MODE_HIRES

#if defined(PICO_BOARD)
#define PLANET_WIDTH 240
#else
#define PLANET_WIDTH 320
#endif

#define PLANET_HEIGHT 240

#else // LOW RES

#if defined(PICO_BOARD)
#define PLANET_WIDTH 120
#else
#define PLANET_WIDTH 160
#endif

#define PLANET_HEIGHT 120
#endif

extern const blit::Font custom_font;

namespace heliopause {

enum Platform {
  picosystem,
  stm32blit,
  sdl,
};

extern Platform current_platform;

}
