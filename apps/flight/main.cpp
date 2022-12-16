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

pw::StringBuffer<64> ship_speed;
pw::StringBuffer<64> ship_debug;

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
Vec2 planet_sector_pos = Vec2(32.0, 32.0);
Vec2 planet_screen_pos = Vec2(0, 0);

uint32_t last_planet_render_time = 0;
Vec2 screen_center = Vec2(0, 0);
float delta_seconds;

std::string_view text_test = {" !\"#$%@'()*+,-.012345679:;<=>?\n"
                              "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]\n"
                              "^_`abcdefghijklmnopqrstuvwxyz{|}~"};

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

Vec2 dpad_direction = Vec2(0.0f, 0.0f);
bool direction_input = false;

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
  blit::screen.blit(&planet_framebuffer,
                    blit::Rect(0, 0, planet_width, planet_width),
                    planet_screen_pos);

  int char_h_offset = -5;

  // blit::screen.pen = PICO8_BLACK;
  // blit::screen.text(
  //     ship_speed.view(),
  //     heliopause::kCustomFont,
  //     blit::Point(2 + 1, blit::screen.bounds.h - 8 + 1 + char_h_offset));
  blit::screen.pen = PICO8_WHITE;
  blit::screen.text(text_test, heliopause::kCustomFont, blit::Point(0, 0),
                    false // variable width?
  );

  blit::screen.text(ship_speed.view(), heliopause::kCustomFont,
                    blit::Point(2, blit::screen.bounds.h - 8 + char_h_offset),
                    false // variable width?
  );
  blit::screen.text(ship_debug.view(), heliopause::kCustomFont,
                    blit::Point(2, blit::screen.bounds.h - 16 + char_h_offset),
                    false // variable width?
  );

  if (direction_input) {
    blit::screen.pen = PICO8_WHITE;
    Vec2 arrow_start = screen_center + (dpad_direction * 20.0f);
    Vec2 arrow_end = screen_center + (dpad_direction * 30.0f);
    blit::screen.line(arrow_start, arrow_end);
  }

  pilot.Draw(&blit::screen, screen_center);
  last_render_time = time;
}

void update(uint32_t time) {
  float delta_seconds = (time - last_update_time) / 1000.0f;
  // float gees = 0;

  // if (buttons & Button::DPAD_UP || buttons & Button::Y) {
  if (buttons & Button::Y) {
    pilot.ApplyThrust(4.0, delta_seconds);
    // gees = pilot.cur_gees;
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

  direction_input = true;
  dpad_direction = joystick;
  float dpad_angle = 0.0f;
  // Absolute Dpad Steering
  if (buttons & Button::DPAD_RIGHT && buttons & Button::DPAD_UP) {
    pilot.RotateTowardsHeading(radians(315.0f), delta_seconds);
    dpad_direction = Vec2(1, -1);
  } else if (buttons & Button::DPAD_LEFT && buttons & Button::DPAD_UP) {
    pilot.RotateTowardsHeading(radians(225.0f), delta_seconds);
    dpad_direction = Vec2(-1, -1);
  } else if (buttons & Button::DPAD_LEFT && buttons & Button::DPAD_DOWN) {
    pilot.RotateTowardsHeading(radians(135.0f), delta_seconds);
    dpad_direction = Vec2(-1, 1);
  } else if (buttons & Button::DPAD_RIGHT && buttons & Button::DPAD_DOWN) {
    pilot.RotateTowardsHeading(radians(45.0f), delta_seconds);
    dpad_direction = Vec2(1, 1);
  } else if (buttons & Button::DPAD_RIGHT) {
    pilot.RotateTowardsHeading(radians(0.0f), delta_seconds);
    dpad_direction = Vec2(1, 0);
  } else if (buttons & Button::DPAD_DOWN) {
    pilot.RotateTowardsHeading(radians(90.0f), delta_seconds);
    dpad_direction = Vec2(0, 1);
  } else if (buttons & Button::DPAD_LEFT) {
    pilot.RotateTowardsHeading(radians(180.0f), delta_seconds);
    dpad_direction = Vec2(-1, 0);
  } else if (buttons & Button::DPAD_UP) {
    pilot.RotateTowardsHeading(radians(270.0f), delta_seconds);
    dpad_direction = Vec2(0, -1);
  } else {
    if (joystick.x >= 0.1f || joystick.x <= -0.1f || joystick.y >= 0.1f ||
        joystick.y <= -0.1f) {
      dpad_direction = joystick;
      dpad_angle = atan2f(dpad_direction.y, dpad_direction.x);
      if (dpad_angle < 0) {
        dpad_angle = kTwoPi + dpad_angle;
      }
      pilot.RotateTowardsHeading(dpad_angle, delta_seconds);
    } else {
      direction_input = false;
    }
  }

  pilot.UpdateLocation(delta_seconds);

  planet_screen_pos =
      screen_center - (pilot.sector_position - planet_sector_pos);

  stars.Scroll(pilot.velocity_vector, delta_seconds);

  ship_speed.clear();
  ship_speed.Format("v:%.2f s:[%.2f, %.2f] s-p[%.2f, %.2f]",
                    static_cast<double>(pilot.velocity),
                    static_cast<double>(pilot.sector_position.x),
                    static_cast<double>(pilot.sector_position.y),
                    static_cast<double>(planet_screen_pos.x),
                    static_cast<double>(planet_screen_pos.y));

  ship_debug.clear();
  ship_debug.Format("Heading: %.2f %.2f [%.2f, %.2f] a:%.2f",
                    static_cast<double>(pilot.angle_degrees),
                    static_cast<double>(pilot.angle_radians),
                    static_cast<double>(dpad_direction.x),
                    static_cast<double>(dpad_direction.y),
                    static_cast<double>(degrees(dpad_angle)));

  last_update_time = time;
}
