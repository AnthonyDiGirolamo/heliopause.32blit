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
#include "font_asset.hpp"
#include "planet_editor.hpp"
#include "random.hpp"

using namespace blit;

bool not_rendered = true;
static pw::StringBuffer<40> planet_metadata;
const Font custom_font(m3x6_font);

void init() {
#ifdef SCREEN_MODE_HIRES
  set_screen_mode(ScreenMode::hires);
#else
  set_screen_mode(ScreenMode::lores);
#endif

  heliopause::PlanetEditor::planet_framebuffer.palette = PICO8;
  // This color should exist in the palette
  heliopause::PlanetEditor::planet_framebuffer.transparent_index = 48;
  // Set to non-existent index and clear so pixels are not updaed when calling
  // screen.blit().
  heliopause::PlanetEditor::planet_framebuffer.pen = 255;
  heliopause::PlanetEditor::planet_framebuffer.clear();
  planet_metadata.Format("Rendering...");

  Random::RestartSeed();
}

///////////////////////////////////////////////////////////////////////////
void render(uint32_t time) {
  screen.pen = Pen(64, 64, 64, 255);
  screen.alpha = 255;
  screen.clear();
  screen.mask = nullptr;

  int char_h_offset = -5;
  int xoffset = 0;
  // // Debug: Show planet_framebuffer draw bounds
  // screen.pen = PICO8_INDIGO;
  // int xoffset = 32;
  // Draw::rectangle(&screen, xoffset + 0, 0, PLANET_WIDTH, PLANET_HEIGHT);

  screen.blit(&heliopause::PlanetEditor::planet_framebuffer,
              Rect(0, 0, PLANET_WIDTH, PLANET_HEIGHT), Point(xoffset + 0, 0));

  if (heliopause::PlanetEditor::planet_menu.active) {
    // Draw the menu if active
    heliopause::PlanetEditor::planet_menu.Draw(&screen, 0, 0);

  } else {
    // Planet Type name in upper left.
    // Text Shadow
    int color_index =
        heliopause::PlanetEditor::current_planet.terrain.map_icon_color + 16;
    screen.pen = PICO8[color_index];
    screen.text(heliopause::PlanetEditor::current_planet.terrain.type_string,
                custom_font,
                Point(3, screen.bounds.h - 24 + 1 + char_h_offset));
    // Text
    color_index =
        heliopause::PlanetEditor::current_planet.terrain.map_icon_color;
    screen.pen = PICO8[color_index];
    screen.text(heliopause::PlanetEditor::current_planet.terrain.type_string,
                custom_font, Point(2, screen.bounds.h - 24 + char_h_offset));
  }

  // Render time
  screen.pen = PICO8_BLACK;
  screen.text(heliopause::PlanetEditor::last_render_update_message.view(),
              custom_font,
              Point(2 + 1, screen.bounds.h - 16 + 1 + char_h_offset));
  screen.pen = PICO8_WHITE;
  screen.text(heliopause::PlanetEditor::last_render_update_message.view(),
              custom_font, Point(2, screen.bounds.h - 16 + char_h_offset));

  // Planet info
  screen.pen = PICO8_BLACK;
  screen.text(planet_metadata.view(), custom_font,
              Point(2 + 1, screen.bounds.h - 8 + 1 + char_h_offset));
  screen.pen = PICO8_WHITE;
  screen.text(planet_metadata.view(), custom_font,
              Point(2, screen.bounds.h - 8 + char_h_offset));

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
}

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

  if (not rerender)
    rerender = heliopause::PlanetEditor::auto_rotate();

  if (rerender) {
    heliopause::PlanetEditor::current_planet.Regen();
    heliopause::PlanetEditor::render_planet();

    planet_metadata.clear();
    planet_metadata.Format(
        "Noise Range: [%.2f, %.2f]",
        (double)heliopause::PlanetEditor::current_planet.min_noise,
        (double)heliopause::PlanetEditor::current_planet.max_noise);
  }
}
