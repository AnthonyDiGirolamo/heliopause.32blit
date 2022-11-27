#include "main.hpp"
#include "platform.hpp"
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
#include "planet_editor.hpp"
#include "random.hpp"

using namespace blit;

void init() {
#ifdef SCREEN_MODE_HIRES
  set_screen_mode(ScreenMode::hires);
#else
  set_screen_mode(ScreenMode::lores);
#endif

#if defined(TARGET_32BLIT_HW)
  heliopause::kCurrentPlatform = heliopause::stm32blit;
#elif defined(PICO_BOARD)
  heliopause::kCurrentPlatform = heliopause::picosystem;
#else
  heliopause::kCurrentPlatform = heliopause::sdl;
#endif

  Random::RestartSeed();
  heliopause::PlanetEditor::init();
}

void render(uint32_t time) { heliopause::PlanetEditor::render(time); }

void update(uint32_t time) { heliopause::PlanetEditor::update(time); }
