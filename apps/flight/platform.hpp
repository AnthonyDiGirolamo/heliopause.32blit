#pragma once

#include "graphics/font.hpp"
#include "graphics/surface.hpp"

// Use Hires screen
#define SCREEN_MODE_HIRES 1

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

#ifdef SCREEN_MODE_HIRES
#define PLANET_FRAMEBUFFER_WIDTH 320
#define PLANET_FRAMEBUFFER_HEIGHT 240
#else
#define PLANET_FRAMEBUFFER_WIDTH 160
#define PLANET_FRAMEBUFFER_HEIGHT 120
#endif

#endif

namespace heliopause {

enum PlatformType {
  picosystem,
  stm32blit,
  sdl,
};

extern PlatformType kCurrentPlatform;

extern const blit::Font kCustomFont;

} // namespace heliopause
