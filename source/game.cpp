#include "game.hpp"
#include "SimplexNoise.h"
#include "engine/engine.hpp"
#include "math/constants.hpp"
#include "planet.hpp"
#include "planet_types.hpp"
#include "random.hpp"
#include <chrono>
#include <stdint.h>

using namespace blit;

namespace {
#define PLANET_WIDTH 320
#define PLANET_HEIGHT 160

uint8_t planet_pixel_data[PLANET_WIDTH * PLANET_HEIGHT];

Surface planet_famebuffer((uint8_t *)planet_pixel_data, PixelFormat::P,
                          Size(PLANET_WIDTH, PLANET_HEIGHT));

int selected_planet_index = 0;

Planet current_planet = Planet(0x64063701, AllPlanets[selected_planet_index]);

void next_planet() {
  selected_planet_index = (selected_planet_index + 1) % PlanetSpan.size();
  current_planet.SetTerrain(AllPlanets[selected_planet_index]);
  current_planet.Regen();
}

} // namespace
///////////////////////////////////////////////////////////////////////////
void init() {
  // set_screen_mode(ScreenMode::lores);
  set_screen_mode(ScreenMode::hires);
  planet_famebuffer.palette = PICO8;

  Random::ResetSeed();

  current_planet.render_equirectangular(&planet_famebuffer);

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
  float r = 0;
  for (int i = 0; i < 100; i++) {
    r = Random::GetRandomFloat(0, 1024.0);
    blit::debugf("%d.%.6d\n", (int)r, (int)((r - (int)r) * 1000000));
  }
}

///////////////////////////////////////////////////////////////////////////
void render(uint32_t time) {

  // clear the screen -- screen is a reference to the frame buffer and can be
  // used to draw all things with the 32blit
  screen.clear();
  // blit::debugf("Hello from 32blit time = %lu - %d,%d\n", time,
  // screen.bounds.w, screen.bounds.h);

  // draw some text at the top of the screen
  screen.alpha = 255;
  screen.mask = nullptr;
  screen.pen = Pen(255, 255, 255);
  screen.rectangle(Rect(0, 0, 320, 14));
  screen.pen = Pen(0, 0, 0);
  screen.text("Planet!", minimal_font, Point(5, 4));

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

  screen.blit(&planet_famebuffer, Rect(0, 0, PLANET_WIDTH, PLANET_HEIGHT),
              Point(0, 44));
  screen.pen = Pen(0, 0, 0);
}

// ///////////////////////////////////////////////////////////////////////////
// //
// update(time)
//
// This is called to update your game state. time is the
// amount if milliseconds elapsed since the start of your game
//
void update(uint32_t time) {

  if (buttons.pressed & Button::DPAD_UP) {
    Random::IncrementSeed(1);
    blit::debugf("Seed: %d\n", Random::GetCurrentSeed());
    current_planet.SetTerrainAndSeed(Random::GetCurrentSeed(),
                                     AllPlanets[selected_planet_index]);
    current_planet.render_equirectangular(&planet_famebuffer);
  } else if (buttons.pressed & Button::DPAD_DOWN) {
    Random::IncrementSeed(-1);
    blit::debugf("Seed: %d\n", Random::GetCurrentSeed());
    current_planet.SetTerrainAndSeed(Random::GetCurrentSeed(),
                                     AllPlanets[selected_planet_index]);
    current_planet.render_equirectangular(&planet_famebuffer);
  } else if (buttons.pressed & Button::DPAD_RIGHT) {
    next_planet();
    current_planet.render_equirectangular(&planet_famebuffer);
  }
  // blit::debugf("Hello from 32blit time = %lu - %d,%d\n", time,
  //              screen.bounds.w, screen.bounds.h);
}
