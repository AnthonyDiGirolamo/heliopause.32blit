#include "main.hpp"
#include "platform.hpp"
// 32blit
#include "32blit.hpp"
#include "engine/engine.hpp"
#include "math/constants.hpp"
// std
#include <chrono>
#include <stdint.h>
#include <string>
#include <string_view>

#include "colors.hpp"
#include "planet.hpp"
#include "planet_types.hpp"
#include "random.hpp"
#include "starfield.hpp"

using namespace blit;

Starfield stars = Starfield(&blit::screen);
Vec2 velocity = Vec2(1.0, 0);
Planet current_planet = Planet(0x64063701, AllPlanets[6]);

const int planet_width = 100;

uint8_t planet_pixel_data[planet_width * planet_width];
blit::Surface planet_framebuffer((uint8_t *)planet_pixel_data,
                                 blit::PixelFormat::P,
                                 blit::Size(planet_width, planet_width));

uint32_t last_render_time = 0;

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

  planet_framebuffer.palette = PICO8;
  planet_framebuffer.alpha = 0;
  planet_framebuffer.transparent_index = 48;
  planet_framebuffer.pen = 49;
  planet_framebuffer.clear();

  Random::RestartSeed();
  stars.ResetAll();

  // Tilt the planet down a bit
  current_planet.AdjustViewpointLatitude(blit::pi * -0.1f);
}

void render(uint32_t time) {
  // Clear screen
  blit::screen.pen = blit::Pen(0, 0, 0, 255);
  blit::screen.alpha = 255;
  blit::screen.clear();
  blit::screen.mask = nullptr;

  stars.Scroll(velocity);
  stars.Draw(velocity);

  uint32_t time_now = blit::now();
  if (time_now > last_render_time + 50) {
    last_render_time = time_now;
    // Rotate the planet
    current_planet.AdjustViewpointLongitude(blit::pi * 0.01f);

    // Render the planet into the dedicated framebuffer
    current_planet.SetDrawPosition(0, 0);
    current_planet.setup_render_orthographic(&planet_framebuffer,
                                             planet_width, // width
                                             planet_width, // height
                                             0,            // camera_zoom,
                                             0,            // camera_pan_x,
                                             0,            // camera_pan_y,
                                             blit::now());
    current_planet.render_orthographic_all();
  }

  // Copy the planet_framebuffer onto the screen
  blit::screen.blit(&planet_framebuffer,
                    blit::Rect(0, 0, planet_width, planet_width),
                    blit::Point((blit::screen.bounds.w - planet_width) / 2,
                                (blit::screen.bounds.h - planet_width) / 2));
}

void update(uint32_t time) {}
