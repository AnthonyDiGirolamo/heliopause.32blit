#include "game.hpp"
#include "SimplexNoise.h"
#include "colors.hpp"
#include "draw.hpp"
#include "engine/engine.hpp"
#include "math/constants.hpp"
#include "planet.hpp"
#include "planet_types.hpp"
#include "random.hpp"
#include <chrono>
#include <stdint.h>
#include <string>

using namespace blit;

namespace {
#define SCREEN_MODE_HIRES 1
// #define SCREEN_MODE_LORES 1
#ifdef SCREEN_MODE_HIRES
#define PLANET_WIDTH 240
#define PLANET_HEIGHT 240
#else
#define PLANET_WIDTH 120
#define PLANET_HEIGHT 120
#endif

uint8_t planet_pixel_data[PLANET_WIDTH * PLANET_HEIGHT];

Surface planet_framebuffer((uint8_t *)planet_pixel_data, PixelFormat::P,
                           Size(PLANET_WIDTH, PLANET_HEIGHT));

int selected_planet_index = 0;

Planet current_planet = Planet(0x64063701, AllPlanets[selected_planet_index]);
bool not_rendered = true;

void next_planet() {
  selected_planet_index = (selected_planet_index + 1) % PlanetSpan.size();
  current_planet.SetTerrain(AllPlanets[selected_planet_index]);
  current_planet.Regen();
}
void previous_planet() {
  selected_planet_index -= 1;
  if (selected_planet_index < 0)
    selected_planet_index = PlanetSpan.size() - 1;
  current_planet.SetTerrain(AllPlanets[selected_planet_index]);
  current_planet.Regen();
}

uint32_t last_rotation = 0;
uint32_t last_render_duration = 0;
std::string last_render_duration_string;

void render_planet() {
  uint32_t start_time = blit::now();
  // planet_framebuffer.clip = Rect(0, 0, 10, 10);

  current_planet.SetDrawOffset(-1, -1);
  current_planet.render_orthographic(&planet_framebuffer, 120);

  float original_lambda = current_planet.viewpoint_lambda0;
  current_planet.viewpoint_lambda0 += blit::pi;
  current_planet.SetDrawOffset(-1, 120);
  current_planet.render_orthographic(&planet_framebuffer, 120);
  current_planet.viewpoint_lambda0 = original_lambda;
  // current_planet.SetDrawOffset(0, 120);
  // current_planet.render_equirectangular(&planet_framebuffer, 240, 120);

  last_rotation = blit::now();
  last_render_duration = last_rotation - start_time;
  // blit::debugf("Render time: %d\n", last_render_duration);
  last_render_duration_string = std::to_string(last_render_duration);
}

} // namespace
///////////////////////////////////////////////////////////////////////////
void init() {
#ifdef SCREEN_MODE_HIRES
  set_screen_mode(ScreenMode::hires);
#else
  set_screen_mode(ScreenMode::lores);
#endif
  planet_framebuffer.palette = PICO8;
  // This color should exist in the palette
  planet_framebuffer.transparent_index = 48;
  planet_framebuffer.pen = 255;
  planet_framebuffer.clear();

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

  screen.pen = PICO8[13];
  int xoffset = 32;
  Draw::rectangle(&screen, xoffset + 0, 0, PLANET_WIDTH, PLANET_HEIGHT);

  screen.blit(&planet_framebuffer, Rect(0, 0, PLANET_WIDTH, PLANET_HEIGHT),
              Point(xoffset + 0, 0));

  // Text Shadow
  screen.pen = PICO8[current_planet.terrain.map_icon_color + 16];
  screen.text(current_planet.terrain.type_string, minimal_font, Point(3, 2));
  // Text
  screen.pen = PICO8[current_planet.terrain.map_icon_color];
  screen.text(current_planet.terrain.type_string, minimal_font, Point(2, 1));

  screen.text(last_render_duration_string, minimal_font,
              Point(2, PLANET_HEIGHT - 8));

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

  if (buttons.pressed & Button::Y) {
    Random::IncrementSeed(1);
    blit::debugf("Seed: %d\n", Random::GetCurrentSeed());
    current_planet.SetTerrainAndSeed(Random::GetCurrentSeed(),
                                     AllPlanets[selected_planet_index]);
    rerender = true;

  } else if (buttons.pressed & Button::X) {
    Random::IncrementSeed(-1);
    blit::debugf("Seed: %d\n", Random::GetCurrentSeed());
    current_planet.SetTerrainAndSeed(Random::GetCurrentSeed(),
                                     AllPlanets[selected_planet_index]);
    rerender = true;
  } else if (buttons.pressed & Button::A) {
    next_planet();
    rerender = true;
  } else if (buttons.pressed & Button::B) {
    previous_planet();
    rerender = true;
  } else if (buttons.pressed & Button::DPAD_UP) {
    current_planet.AdjustViewpointLatitude(blit::pi * 0.1f);
    rerender = true;
  } else if (buttons.pressed & Button::DPAD_DOWN) {
    current_planet.AdjustViewpointLatitude(blit::pi * -0.1f);
    rerender = true;
  } else if (buttons.pressed & Button::DPAD_LEFT) {
    current_planet.AdjustViewpointLongitude(blit::pi * -0.1f);
    rerender = true;
  } else if (buttons.pressed & Button::DPAD_RIGHT) {
    current_planet.AdjustViewpointLongitude(blit::pi * 0.1f);
    rerender = true;
  }

  // // Auto rotate - only good on host machine
  // uint32_t time_now = blit::now();
  // if (time_now > last_rotation + 40) {
  //   current_planet.AdjustViewpointLongitude(blit::pi * 0.01f);
  //   rerender = true;
  // }

  if (rerender) {
    render_planet();
  }
}
