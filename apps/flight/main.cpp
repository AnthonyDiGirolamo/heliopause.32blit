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

#include "pw_string/string_builder.h"

#include "colors.hpp"
#include "planet.hpp"
#include "planet_types.hpp"
#include "random.hpp"
#include "ship.hpp"
#include "starfield.hpp"

using namespace blit;

pw::StringBuffer<50> ship_speed;
pw::StringBuffer<50> ship_debug;

Starfield stars = Starfield(&blit::screen);
Vec2 velocity = Vec2(4.0, 0);
Planet current_planet = Planet(0x64063701, AllPlanets[6]);
Ship pilot = Ship();
uint32_t last_update_time = 0;
uint32_t last_render_time = 0;

const int planet_width = 100;
uint8_t planet_pixel_data[planet_width * planet_width];
blit::Surface planet_framebuffer((uint8_t *)planet_pixel_data,
                                 blit::PixelFormat::P,
                                 blit::Size(planet_width, planet_width));

uint32_t last_planet_render_time = 0;
Vec2 screen_center = Vec2(0, 0);
float delta_seconds;

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
  screen_center = Vec2(blit::screen.bounds.w / 2, blit::screen.bounds.h / 2);

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
  float delta_seconds = (time - last_render_time) / 1000.0f;
  // Clear screen
  blit::screen.pen = blit::Pen(0, 0, 0, 255);
  blit::screen.alpha = 255;
  blit::screen.clear();
  blit::screen.mask = nullptr;

  stars.Draw(pilot.velocity_vector, delta_seconds);

  // uint32_t time_now = blit::now();
  // if (time_now > last_planet_render_time + 50) {
  //   last_planet_render_time = time_now;
  //   // Rotate the planet
  //   current_planet.AdjustViewpointLongitude(blit::pi * 0.01f);
  //   // Render the planet into the dedicated framebuffer
  //   current_planet.SetDrawPosition(0, 0);
  //   current_planet.setup_render_orthographic(&planet_framebuffer,
  //                                            planet_width, // width
  //                                            planet_width, // height
  //                                            0,            // camera_zoom,
  //                                            0,            // camera_pan_x,
  //                                            0,            // camera_pan_y,
  //                                            blit::now());
  //   current_planet.render_orthographic_all();
  // }

  // Copy the planet_framebuffer onto the screen
  // blit::screen.blit(&planet_framebuffer,
  //                   blit::Rect(0, 0, planet_width, planet_width),
  //                   blit::Point((blit::screen.bounds.w - planet_width) / 2,
  //                               (blit::screen.bounds.h - planet_width) / 2));

  int char_h_offset = -5;

  // blit::screen.pen = PICO8_BLACK;
  // blit::screen.text(
  //     ship_speed.view(),
  //     heliopause::kCustomFont,
  //     blit::Point(2 + 1, blit::screen.bounds.h - 8 + 1 + char_h_offset));
  blit::screen.pen = PICO8_WHITE;
  blit::screen.text(ship_speed.view(), heliopause::kCustomFont,
                    blit::Point(2, blit::screen.bounds.h - 8 + char_h_offset));
  blit::screen.text(ship_debug.view(), heliopause::kCustomFont,
                    blit::Point(2, blit::screen.bounds.h - 16 + char_h_offset));

  pilot.Draw(&blit::screen, screen_center);
  last_render_time = time;
}

void update(uint32_t time) {
  float delta_seconds = (time - last_update_time) / 1000.0f;
  float gees = 0;

  // if (buttons & Button::DPAD_UP || buttons & Button::Y) {
  if (buttons & Button::Y) {
    pilot.ApplyThrust(4.0, delta_seconds);
    gees = pilot.cur_gees;
  } else if (pilot.accelerating) {
    // Up not being pressed shut the engine down.
    pilot.CutThrust();
  } else {
    pilot.DampenSpeed(delta_seconds);
  }

#if 0
  // Tank steering
  if (buttons & Button::DPAD_DOWN) {
    pilot.ReverseDirection(delta_seconds);
  }
  if (buttons & Button::DPAD_LEFT) {
    pilot.TurnLeft(delta_seconds);
  } else if (buttons & Button::DPAD_RIGHT) {
    pilot.TurnRight(delta_seconds);
  }
#endif

  // Absolute Dpad Steering
  if (buttons & Button::DPAD_RIGHT && buttons & Button::DPAD_UP) {
    pilot.RotateTowardsHeading(radians(315.0f), delta_seconds);
  } else if (buttons & Button::DPAD_LEFT && buttons & Button::DPAD_UP) {
    pilot.RotateTowardsHeading(radians(225.0f), delta_seconds);
  } else if (buttons & Button::DPAD_LEFT && buttons & Button::DPAD_DOWN) {
    pilot.RotateTowardsHeading(radians(135.0f), delta_seconds);
  } else if (buttons & Button::DPAD_RIGHT && buttons & Button::DPAD_DOWN) {
    pilot.RotateTowardsHeading(radians(45.0f), delta_seconds);
  }

  else if (buttons & Button::DPAD_RIGHT) {
    pilot.RotateTowardsHeading(radians(0.0), delta_seconds);
  } else if (buttons & Button::DPAD_DOWN) {
    pilot.RotateTowardsHeading(radians(90.0), delta_seconds);
  } else if (buttons & Button::DPAD_LEFT) {
    pilot.RotateTowardsHeading(radians(180.0), delta_seconds);
  } else if (buttons & Button::DPAD_UP) {
    pilot.RotateTowardsHeading(radians(270.0), delta_seconds);
  }

  pilot.UpdateLocation(delta_seconds);

  stars.Scroll(pilot.velocity_vector, delta_seconds);

  ship_speed.clear();
  ship_speed.Format(
      "v:%.2f g:%.2f p:[%.2f, %.2f]", static_cast<double>(pilot.velocity),
      static_cast<double>(gees), static_cast<double>(pilot.sector_position.x),
      static_cast<double>(pilot.sector_position.y));

  ship_debug.clear();
  ship_debug.Format("Heading: %.2f %.2f",
                    static_cast<double>(pilot.angle_degrees),
                    static_cast<double>(pilot.angle_radians));

  last_update_time = time;
}
