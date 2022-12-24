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
#include "menu.hpp"
#include "planet.hpp"
#include "planet_types.hpp"
#include "random.hpp"
#include "sector.hpp"
#include "ship.hpp"
#include "starfield.hpp"

using namespace blit;

namespace {

pw::StringBuffer<64> ship_speed;
pw::StringBuffer<64> ship_debug;

Starfield stars = Starfield(&blit::screen);
Planet current_planet = Planet(0xFE, AllPlanetTypes[6]);
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

Sector sector = Sector(0xEE);

// uint32_t last_planet_render_time = 0;
Vec2 screen_center = Vec2(0, 0);

Vec2 dpad_direction = Vec2(0.0f, 0.0f);
bool direction_input = false;
bool absolute_steering = true;
bool flight_assist = true;
bool auto_thrust = false;

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

static const Pen menu_colours[]{
    {0},
    {30, 30, 50, 200}, // background
    {255, 255, 255},   // foreground
    {40, 40, 60},      // bar background
    {50, 50, 70},      // selected item background
    {255, 128, 0},     // battery unknown
    {0, 255, 0},       // battery usb host/adapter port
    {255, 0, 0},       // battery otg
    {100, 100, 255},   // battery charging
    {235, 245, 255},   // header/footer bg
    {3, 5, 7},         // header/footer fg
    {245, 235, 0},     // header/footer fg warning
};

class GameMenu final : public blit::Menu {
public:
  using blit::Menu::Menu;

  void prepare();

protected:
  void item_activated(const Item &item) override;
  void render_item(const Item &item, int y, int index) const override;
  Pen bar_background_color;
};

enum MenuItem {
  M_CONTROL_MODE,
  M_FLIGHT_ASSIST_MODE,
  M_AUTO_THRUST,
};

static Menu::Item game_menu_items[]{
    {M_CONTROL_MODE, "Control Mode"},
    {Menu::Separator, nullptr},
    {M_FLIGHT_ASSIST_MODE, "Flight Assist"},
    {M_AUTO_THRUST, "Thrust on Direction Input"},
};

void GameMenu::prepare() {
  background_colour = menu_colours[1];
  foreground_colour = menu_colours[2];
  bar_background_color = menu_colours[3];
  selected_item_background = menu_colours[4];
  header_background = menu_colours[9];
  header_foreground = menu_colours[10];

  display_rect.w = blit::screen.bounds.w;
  display_rect.h = blit::screen.bounds.h;
}

void GameMenu::item_activated(const Item &item) {
  switch (item.id) {
  case M_CONTROL_MODE:
    absolute_steering = not absolute_steering;
    break;
  case M_FLIGHT_ASSIST_MODE:
    flight_assist = not flight_assist;
    break;
  case M_AUTO_THRUST:
    auto_thrust = not auto_thrust;
    break;
  }
}

void GameMenu::render_item(const Item &item, int y, int index) const {
  Menu::render_item(item, y, index);

  const auto screen_width = screen.bounds.w;

  const int bar_margin = 2;
  const int bar_width = 75;
  int bar_x = screen_width - bar_width - item_padding_x;

  switch (item.id) {
  case M_CONTROL_MODE:
    if (absolute_steering) {
      screen.pen = menu_colours[11];
      screen.text(absolute_steering_enabled_string, heliopause::kCustomFont,
                  Point(screen_width - item_padding_x, y + 1), true,
                  TextAlign::right);
    } else {
      screen.pen = menu_colours[11];
      screen.text(absolute_steering_disabled_string, heliopause::kCustomFont,
                  Point(screen_width - item_padding_x, y + 1), true,
                  TextAlign::right);
    }
    break;

  case M_FLIGHT_ASSIST_MODE:
    if (flight_assist) {
      screen.pen = menu_colours[11];
      screen.text(on_string, heliopause::kCustomFont,
                  Point(screen_width - item_padding_x, y + 1), true,
                  TextAlign::right);
    } else {
      screen.pen = menu_colours[11];
      screen.text(off_string, heliopause::kCustomFont,
                  Point(screen_width - item_padding_x, y + 1), true,
                  TextAlign::right);
    }
    break;

  case M_AUTO_THRUST:
    if (auto_thrust) {
      screen.pen = menu_colours[11];
      screen.text(on_string, heliopause::kCustomFont,
                  Point(screen_width - item_padding_x, y + 1), true,
                  TextAlign::right);
    } else {
      screen.pen = menu_colours[11];
      screen.text(off_string, heliopause::kCustomFont,
                  Point(screen_width - item_padding_x, y + 1), true,
                  TextAlign::right);
    }
    break;

  default:
    screen.pen = foreground_colour;
    screen.text("Press A", heliopause::kCustomFont,
                Point(screen_width - item_padding_x, y + 1), true,
                TextAlign::right);
    break;
  }
}

GameMenu game_menu("Flight Options", game_menu_items,
                   std::size(game_menu_items), heliopause::kCustomFont);

static constexpr heliopause::MenuItem main_menu_items[] = {
    {
        .name = std::string_view{"Control Mode "},
        .get_value =
            []() {
              if (absolute_steering)
                return absolute_steering_enabled_string;
              return absolute_steering_disabled_string;
            },
        .increase_function =
            []() { absolute_steering = not absolute_steering; },
        .decrease_function = nullptr,
    },
    {
        .name = std::string_view{"Flight Assist "},
        .get_value =
            []() {
              if (flight_assist)
                return on_string;
              return off_string;
            },
        .increase_function = []() { flight_assist = not flight_assist; },
        .decrease_function = nullptr,
    },
    {
        .name = std::string_view{"Thrust on Direction Input"},
        // .get_value = &get_auto_thrust_string,
        .get_value =
            []() {
              if (auto_thrust)
                return on_string;
              return off_string;
            },
        .increase_function = []() { auto_thrust = not auto_thrust; },
        .decrease_function = nullptr,
    },
};

constexpr std::span<const heliopause::MenuItem>
    main_menu_items_span(main_menu_items);

heliopause::Menu main_menu =
    heliopause::Menu(std::string_view{"Options"}, main_menu_items_span,
                     &heliopause::kCustomFont, 8, 3, 2, 0, 0);

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

  screen_center =
      Vec2(float(blit::screen.bounds.w) / 2, float(blit::screen.bounds.h) / 2);
  sector.SetScreenCenter(screen_center);

  planet_framebuffer.palette = PICO8;
  planet_framebuffer.alpha = 0;
  planet_framebuffer.transparent_index = 48;
  planet_framebuffer.pen = 49;
  planet_framebuffer.clear();

  Random::RestartSeed();
  stars.ResetAll();

  // Tilt the planet down a bit
  current_planet.AdjustViewpointLatitude(blit::pi * -0.1f);
  // current_planet.AdjustViewpointLongitude(blit::pi * 0.01f);
  // Render the planet into the dedicated framebuffer
  current_planet.SetDrawPosition(0, 0);
  current_planet.setup_render_orthographic(&planet_framebuffer,
                                           planet_width, // width
                                           planet_width, // height
                                           0,            // camera_zoom,
                                           0,            // camera_pan_x,
                                           0,            // camera_pan_y,
                                           blit::now());
  current_planet.Regen();
  current_planet.render_orthographic_all();

  sector.Update(pilot.sector_position);

  planet_screen_pos =
      screen_center - (pilot.sector_position - planet_sector_pos);

  game_menu.prepare();
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
    blit::screen.pen = PICO8_WHITE;
    Vec2 arrow_start = screen_center + (dpad_direction * 20.0f);
    Vec2 arrow_end = screen_center + (dpad_direction * 30.0f);
    blit::screen.line(arrow_start, arrow_end);
  }

  pilot.Draw(&blit::screen, screen_center);

  if (main_menu.active) {
    // main_menu.Draw(&blit::screen, 0, 0);
    game_menu.render();
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
    game_menu.update(time);
    last_update_time = time;
    return;
  }

  if (buttons.pressed & Button::X) {
    main_menu.ToggleActive();
    last_update_time = time;
    return;
  }

  if (absolute_steering)
    update_input_absolute(delta_seconds);
  else
    update_input_steering(delta_seconds);

  if (buttons & Button::Y || (auto_thrust && direction_input) ||
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
  ship_debug.Format("Heading: %.2f %.2f [%.2f, %.2f]",
                    static_cast<double>(pilot.angle_degrees),
                    static_cast<double>(pilot.angle_radians),
                    static_cast<double>(dpad_direction.x),
                    static_cast<double>(dpad_direction.y)
                    // static_cast<double>(degrees(dpad_angle))
  );

  last_update_time = time;
}
