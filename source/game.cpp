#include "game.hpp"
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
#include "planet_editor.hpp"
#include "random.hpp"

using namespace blit;

bool not_rendered = true;

///////////////////////////////////////////////////////////////////////////
void init() {
#ifdef SCREEN_MODE_HIRES
  set_screen_mode(ScreenMode::hires);
#else
  set_screen_mode(ScreenMode::lores);
#endif
  heliopause::PlanetEditor::planet_framebuffer.palette = PICO8;
  // This color should exist in the palette
  heliopause::PlanetEditor::planet_framebuffer.transparent_index = 48;
  heliopause::PlanetEditor::planet_framebuffer.pen = 255;
  heliopause::PlanetEditor::planet_framebuffer.clear();

  Random::RestartSeed();

  // blit::debugf("PlanetSpanCount: %d\n", PlanetSpan.size());

  // for (Planet *p : PlanetSpan) {
  //   blit::debugf("Type: %d = %s\n", p->type, p->type_name);

  //   blit::debugf("  ColorMap: ");
  //   for (int i : p->color_map) {
  //     blit::debugf("%d, ", i);
  //   }
  //   blit::debugf("\n");
  // }

  // for (int i : current_planet.color_map) {
  //   blit::debugf("kTerranColorMap: %d\n", i);
  // }

  // for (int i = 0; i < kTerranColorMap.size(); i++) {
  //   blit::debugf("kTerranColorMap: %d = %d\n", i, kTerranColorMap[i]);
  // }
}

///////////////////////////////////////////////////////////////////////////
void render(uint32_t time) {
  screen.pen = Pen(64, 64, 64);
  screen.clear();
  screen.alpha = 255;
  screen.mask = nullptr;

  // Debug: Show planet_framebuffer draw bounds
  screen.pen = PICO8[13];
  int xoffset = 32;
  Draw::rectangle(&screen, xoffset + 0, 0, PLANET_WIDTH, PLANET_HEIGHT);

  screen.blit(&heliopause::PlanetEditor::planet_framebuffer,
              Rect(0, 0, PLANET_WIDTH, PLANET_HEIGHT), Point(xoffset + 0, 0));

  // Text Shadow
  int color_index =
      heliopause::PlanetEditor::current_planet.terrain.map_icon_color + 16;
  screen.pen = PICO8[color_index];
  screen.text(heliopause::PlanetEditor::current_planet.terrain.type_string,
              minimal_font, Point(3, 2));
  // Text
  color_index = heliopause::PlanetEditor::current_planet.terrain.map_icon_color;
  screen.pen = PICO8[color_index];
  screen.text(heliopause::PlanetEditor::current_planet.terrain.type_string,
              minimal_font, Point(2, 1));

  screen.text(heliopause::PlanetEditor::last_render_update_message.view(),
              minimal_font, Point(2, PLANET_HEIGHT - 8));

  // int rect_width = 8;
  // for (int i = 0; i < 16; i++) {
  //   screen.pen = PICO8[i];
  //   screen.rectangle(
  //       Rect(2 + (i * (rect_width + 1)), 16, rect_width, rect_width));
  // }

  // int x_offset = 0;
  // for (int i = 0; i < 32; i++) {
  //   screen.pen = ENDESGA32[i];
  //   int x = (i * (rect_width + 1));
  //   int y = 25;
  //   if (i > 15) {
  //     y = 34;
  //     x -= x_offset + rect_width + 1;
  //   } else {
  //     x_offset = x;
  //   }
  //   screen.rectangle(Rect(2 + x, y, rect_width, rect_width));
  // }

  // Draw the menu if active
  heliopause::PlanetEditor::planet_menu.Draw(&screen, 0, 0);
}

// ///////////////////////////////////////////////////////////////////////////
// //
// update(time)
//
// This is called to update your game state. time is the
// amount if milliseconds elapsed since the start of your game
//
void update(uint32_t time) {
  bool rerender = false;
  if (not_rendered) {
    rerender = true;
    not_rendered = false;
  }

  if (heliopause::PlanetEditor::planet_menu.active) {
    rerender = heliopause::PlanetEditor::planet_menu.Update();
  } else if (buttons.pressed & Button::X) {
    // Activate menu
    heliopause::PlanetEditor::planet_menu.ToggleActive();
  } else if (buttons.pressed & Button::Y) {
    rerender = true;
  } else if (buttons.pressed & Button::A) {
    heliopause::PlanetEditor::next_planet();
    rerender = true;
  } else if (buttons.pressed & Button::B) {
    heliopause::PlanetEditor::previous_planet();
    rerender = true;
  } else if (buttons.pressed & Button::DPAD_UP) {
    heliopause::PlanetEditor::current_planet.AdjustViewpointLatitude(blit::pi *
                                                                     0.1f);
    rerender = true;
  } else if (buttons.pressed & Button::DPAD_DOWN) {
    heliopause::PlanetEditor::current_planet.AdjustViewpointLatitude(blit::pi *
                                                                     -0.1f);
    rerender = true;
  } else if (buttons.pressed & Button::DPAD_LEFT) {
    heliopause::PlanetEditor::current_planet.AdjustViewpointLongitude(blit::pi *
                                                                      -0.1f);
    rerender = true;
  } else if (buttons.pressed & Button::DPAD_RIGHT) {
    heliopause::PlanetEditor::current_planet.AdjustViewpointLongitude(blit::pi *
                                                                      0.1f);
    rerender = true;
  }

  // // Auto rotate - only good on host machine
  // uint32_t time_now = blit::now();
  // if (time_now > last_rotation + 40) {
  //   current_planet.AdjustViewpointLongitude(blit::pi * 0.01f);
  //   rerender = true;
  // }

  if (rerender) {
    heliopause::PlanetEditor::render_planet();
  }
}
