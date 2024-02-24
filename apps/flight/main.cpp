#include "main.hpp"
#include "engine/input.hpp"
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
#include "menu.hpp"
#include "planet.hpp"
#include "planet_types.hpp"
#include "random.hpp"
#include "sector.hpp"
#include "ship.hpp"
#include "starfield.hpp"
#include "trig.hpp"

using namespace blit;
using namespace lib_trig;

namespace {

pw::StringBuffer<64> ship_speed;
pw::StringBuffer<64> ship_debug;

Starfield stars = Starfield(&blit::screen);
Planet current_planet = Planet(0xFE, AllPlanetTypes[6]);
Ship pilot = Ship();
uint32_t last_update_time = 0;
uint32_t last_render_time = 0;

const int scratch_planet_width = 120;
const int scratch_planet_height = 240;
uint8_t planet_pixel_data[scratch_planet_width * scratch_planet_height];
blit::Surface scratch_planet_framebuffer((uint8_t *)planet_pixel_data,
                                         blit::PixelFormat::P,
                                         blit::Size(scratch_planet_width,
                                                    scratch_planet_height));
Vec2 scratch_planet_sector_pos = Vec2(32.0, 32.0);
Vec2 scratch_planet_screen_pos = Vec2(0, 0);

Sector sector = Sector(0xE2);

// uint32_t last_planet_render_time = 0;
Vec2 screen_center = Vec2(0, 0);

Vec2 dpad_direction = Vec2(0.0f, 0.0f);
bool direction_input = false;
bool absolute_steering = true;
bool flight_assist = true;
bool auto_thrust = true;
bool hires_mode = true;

// std::string_view text_test = {" !\"#$%@'()*+,-./\n"
//                               "0123456789:;<=>?\n"
//                               "@ABCDEFGHIJKLMNO\n"
//                               "PQRSTUVWXYZ[\\]^_\n"
//                               "`abcdefghijklmno\n"
//                               "pqrstuvwxyz{|}~\n"};

std::string_view absolute_steering_enabled_string = {"Absolute"};
std::string_view absolute_steering_disabled_string = {"Relative"};

std::string_view on_string = {"On"};
std::string_view off_string = {"Off"};

} // namespace

void reinit_screen(bool hires_mode) {
  if (hires_mode)
    set_screen_mode(ScreenMode::hires);
  else
    set_screen_mode(ScreenMode::lores);

  screen_center =
      Vec2(float(blit::screen.bounds.w) / 2, float(blit::screen.bounds.h) / 2);
  sector.SetScreenCenter(screen_center);

  stars.ResetAll();
  sector.Draw(&blit::screen);
}

static constexpr heliopause::MenuItem main_menu_items[] = {
    {
        .name = std::string_view{"Control Mode "},
        .get_value =
            []() {
              if (absolute_steering)
                return absolute_steering_enabled_string;
              return absolute_steering_disabled_string;
            },
        .increase_function = []() { absolute_steering = true; },
        .decrease_function = []() { absolute_steering = false; },
    },
    {
        .name = std::string_view{"Flight Assist "},
        .get_value =
            []() {
              if (flight_assist)
                return on_string;
              return off_string;
            },
        .increase_function = []() { flight_assist = true; },
        .decrease_function = []() { flight_assist = false; },
    },
    {
        .name = std::string_view{"Auto Thrust"},
        .get_value =
            []() {
              if (auto_thrust)
                return on_string;
              return off_string;
            },
        .increase_function = []() { auto_thrust = true; },
        .decrease_function = []() { auto_thrust = false; },
    },
    {
        .name = std::string_view{"Hi-Res"},
        .get_value =
            []() {
              if (hires_mode)
                return on_string;
              return off_string;
            },
        .increase_function =
            []() {
              hires_mode = true;
              reinit_screen(hires_mode);
            },
        .decrease_function =
            []() {
              hires_mode = false;
              reinit_screen(hires_mode);
            },
    },
};

constexpr pw::span<const heliopause::MenuItem>
    main_menu_items_span(main_menu_items);

heliopause::Menu main_menu;

void init() {
  // #ifdef SCREEN_MODE_HIRES
  // set_screen_mode(ScreenMode::hires);
  // #else
  set_screen_mode(ScreenMode::lores);
  // #endif

#if defined(TARGET_32BLIT_HW)
  heliopause::kCurrentPlatform = heliopause::stm32blit;
#elif defined(PICO_BOARD)
  heliopause::kCurrentPlatform = heliopause::picosystem;
#else
  heliopause::kCurrentPlatform = heliopause::sdl;
#endif

  scratch_planet_framebuffer.palette = PICO8;
  scratch_planet_framebuffer.alpha = 0;
  scratch_planet_framebuffer.transparent_index = 48;
  scratch_planet_framebuffer.pen = 49;
  scratch_planet_framebuffer.clear();

  main_menu = heliopause::Menu( // Pause menu
      std::string_view{"Options"},
      main_menu_items_span,     // items
      &heliopause::kCustomFont, // font
      0,                        // text row height
      3,                        // item top padding
      2,                        // item bottom padding
      2,                        // left margin
      2                         // right margin
  );
  main_menu.SetButtons(blit::Button::Y, blit::Button::A);
  main_menu.color_title_foreground = ENDESGA64[34];
  main_menu.color_title_shadow = ENDESGA64[39];
  main_menu.color_title_background = ENDESGA64[12];
  main_menu.color_background = ENDESGA64[14];
  main_menu.color_background.a = 128;
  main_menu.color_border = blit::Pen(0, 0, 0, 0);
  main_menu.border_size = 20;

  Random::RestartSeed();
  reinit_screen(hires_mode);

  sector.RenderPlanets(&scratch_planet_framebuffer);

  sector.Update(pilot.sector_position);

  scratch_planet_screen_pos =
      screen_center - (pilot.sector_position - scratch_planet_sector_pos);
}

void render(uint32_t time) {
  float delta_seconds = (time - last_render_time) / 1000.0f;
  // Clear screen
  blit::screen.pen = blit::Pen(0, 0, 0, 255);
  blit::screen.alpha = 255;
  blit::screen.clear();
  blit::screen.mask = nullptr;

  // TODO: The star trails can wobble if game is paused due to variability in
  // delta_seconds.
  if (main_menu.active)
    stars.Draw(pilot.velocity_vector, 0.0f);
  else
    stars.Draw(pilot.velocity_vector, delta_seconds);

  // uint32_t time_now = blit::now();
  // if (time_now > last_planet_render_time + 50) {
  //   last_planet_render_time = time_now;
  //   // Rotate the planet
  //   current_planet.AdjustViewpointLongitude(blit::pi * 0.01f);
  //   // Render the planet into the dedicated framebuffer
  //   current_planet.SetDrawPosition(0, 0);
  //   current_planet.setup_render_orthographic(&scratch_planet_framebuffer,
  //                                            scratch_planet_width, // width
  //                                            scratch_planet_height, // height
  //                                            0,            // camera_zoom,
  //                                            0,            // camera_pan_x,
  //                                            0,            // camera_pan_y,
  //                                            blit::now());
  //   current_planet.render_orthographic_all();
  // }

  // Copy the scratch_planet_framebuffer onto the screen
  blit::screen.blit(
      &scratch_planet_framebuffer,
      blit::Rect(0, 0, scratch_planet_width, scratch_planet_height),
      scratch_planet_screen_pos);

  sector.Draw(&blit::screen);

  int char_h_offset = -5;

  // blit::screen.pen = PICO8_BLACK;
  // blit::screen.text(
  //     ship_speed.view(),
  //     heliopause::kCustomFont,
  //     blit::Point(2 + 1, blit::screen.bounds.h - 8 + 1 + char_h_offset));
  blit::screen.pen = PICO8_WHITE;

  // blit::screen.text(text_test,
  //                   // minimal_font,
  //                   heliopause::kCustomFont, blit::Point(0, 0),
  //                   false // variable width?
  // );

  blit::screen.text(ship_speed.view(), heliopause::kCustomFont,
                    blit::Point(2, blit::screen.bounds.h - 8 + char_h_offset),
                    false // variable width?
  );
  blit::screen.text(ship_debug.view(), heliopause::kCustomFont,
                    blit::Point(2, blit::screen.bounds.h - 16 + char_h_offset),
                    false // variable width?
  );

  if (direction_input) {
    blit::screen.pen = PICO8_GREEN;
    Vec2 arrow_start = screen_center + (dpad_direction * 20.0f);
    Vec2 arrow_end = screen_center + (dpad_direction * 30.0f);
    blit::screen.line(arrow_start, arrow_end);
  }

  pilot.Draw(&blit::screen, screen_center);

  if (main_menu.active) {
    main_menu.Draw(&blit::screen, 0, 0);
  }

  last_render_time = time;
}

void update_input_steering(float delta_seconds) {
  // Tank steering
  if (buttons & Button::DPAD_DOWN) {
    pilot.ReverseDirection(delta_seconds);
  }
  if (buttons & Button::DPAD_LEFT) {
    pilot.TurnLeft(delta_seconds);
  } else if (buttons & Button::DPAD_RIGHT) {
    pilot.TurnRight(delta_seconds);
  }
}

void update_input_absolute(float delta_seconds) {
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
}

void update(uint32_t time) {
  float delta_seconds = (time - last_update_time) / 1000.0f;
  // float gees = 0;

  if (main_menu.active) {
    main_menu.Update(time);
    last_update_time = time;
    return;
  }

  if (buttons.pressed & Button::Y) {
    main_menu.ToggleActive();
    last_update_time = time;
    return;
  }

  if (absolute_steering)
    update_input_absolute(delta_seconds);
  else
    update_input_steering(delta_seconds);

  if (buttons & Button::X || (auto_thrust && direction_input) ||
      (not absolute_steering && buttons & Button::DPAD_UP)) {
    pilot.ApplyThrust(4.0, delta_seconds);
    // gees = pilot.cur_gees;
  } else if (pilot.accelerating) {
    // Up not being pressed shut the engine down.
    pilot.CutThrust();
  } else {
    if (flight_assist)
      pilot.DampenSpeed(delta_seconds);
  }

  pilot.UpdateLocation(delta_seconds);

  sector.Update(pilot.sector_position);

  scratch_planet_screen_pos =
      screen_center - (pilot.sector_position - scratch_planet_sector_pos);

  stars.Scroll(pilot.velocity_vector, delta_seconds);

  ship_speed.clear();
  ship_speed.Format(
      "v:%.2f s:[%.2f, %.2f] p8: %.2f", static_cast<double>(pilot.velocity),
      static_cast<double>(pilot.sector_position.x),
      static_cast<double>(pilot.sector_position.y),
      static_cast<double>(sector.closest_planet->distance_to_pilot));

  ship_debug.clear();
  ship_debug.Format("Heading: %.2f %.2f [%.2f, %.2f]",
                    static_cast<double>(pilot.angle_degrees),
                    static_cast<double>(pilot.angle_radians),
                    static_cast<double>(dpad_direction.x),
                    static_cast<double>(dpad_direction.y)
                    // static_cast<double>(degrees(dpad_angle))
  );

  // sector.RenderPlanets(&scratch_planet_framebuffer);

  last_update_time = time;
}
