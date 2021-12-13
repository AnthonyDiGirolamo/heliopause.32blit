#pragma once

#include "graphics/font.hpp"

#if defined(TARGET_32BLIT_HW)
#define PLATFORM_32BLIT
#elif defined(PICO_BOARD)
#define PLATFORM_PICOSYSTEM
#else
#define PLATFORM_SDL
#endif

#if defined(PLATFORM_PICOSYSTEM)

#ifdef SCREEN_MODE_HIRES
#define PLANET_FRAMEBUFFER_WIDTH 240
#define PLANET_FRAMEBUFFER_HEIGHT 240
#else
#define PLANET_FRAMEBUFFER_WIDTH 120
#define PLANET_FRAMEBUFFER_HEIGHT 120
#endif

#else // PLATFORM_32BLIT or PLATFORM_SDL

// Use Hires screen
#define SCREEN_MODE_HIRES 1

#ifdef SCREEN_MODE_HIRES
#define PLANET_FRAMEBUFFER_WIDTH 320
#define PLANET_FRAMEBUFFER_HEIGHT 240
#else
#define PLANET_FRAMEBUFFER_WIDTH 160
#define PLANET_FRAMEBUFFER_HEIGHT 120
#endif

#endif

extern const blit::Font custom_font;

namespace heliopause {

enum Platform {
  picosystem,
  stm32blit,
  sdl,
};

extern Platform current_platform;

} // namespace heliopause
