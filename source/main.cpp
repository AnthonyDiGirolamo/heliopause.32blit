#include "main.hpp"
// 32blit
#include "32blit.hpp"
#include "engine/engine.hpp"
#include "math/constants.hpp"
// Pigweed
#include "pw_string/format.h"
#include "pw_string/string_builder.h"
// std
#include <chrono>
#include <stdint.h>
#include <string>
#include <string_view>

// Heliopause
#include "SimplexNoise.h"
#include "colors.hpp"
#include "draw.hpp"
#include "font_asset.hpp"
#include "planet_editor.hpp"
#include "random.hpp"

using namespace blit;

heliopause::Platform current_platform;

const Font custom_font(m3x6_font);

void init() {
#ifdef SCREEN_MODE_HIRES
  set_screen_mode(ScreenMode::hires);
#else
  set_screen_mode(ScreenMode::lores);
#endif

#if defined(TARGET_32BLIT_HW)
  current_platform = heliopause::stm32blit;
#elif defined(PICO_BOARD)
  current_platform = heliopause::picosystem;
#else
  current_platform = heliopause::sdl;
#endif

  Random::RestartSeed();
  heliopause::PlanetEditor::init();
}

void render(uint32_t time) { heliopause::PlanetEditor::render(time); }

void update(uint32_t time) { heliopause::PlanetEditor::update(time); }
