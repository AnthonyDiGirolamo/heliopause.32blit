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
// #define SCREEN_MODE_HIRES 1
// #define SCREEN_MODE_LORES 1
#ifdef SCREEN_MODE_HIRES
#define PLANET_WIDTH 240
#define PLANET_HEIGHT 240
#else
#define PLANET_WIDTH 120
#define PLANET_HEIGHT 120
#endif

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
void previous_planet() {
  selected_planet_index -= 1;
  if (selected_planet_index < 0)
    selected_planet_index = PlanetSpan.size() - 1;
  current_planet.SetTerrain(AllPlanets[selected_planet_index]);
  current_planet.Regen();
}
void render_planet() {
  // Erase to black
  planet_famebuffer.pen = 0;
  planet_famebuffer.clear();

  // White box outline
  planet_famebuffer.pen = 7;
  // Left
  planet_famebuffer.v_span(Point(0, 0), PLANET_HEIGHT);
  // Top
  planet_famebuffer.h_span(Point(0, 0), PLANET_HEIGHT);
  // Right
  planet_famebuffer.v_span(Point(PLANET_HEIGHT - 1, 0), PLANET_HEIGHT);
  // Bottom
  planet_famebuffer.h_span(Point(0, PLANET_HEIGHT - 1), PLANET_HEIGHT);

  current_planet.render_orthographic(&planet_famebuffer);
  // current_planet.render_equirectangular(&planet_famebuffer);
}

} // namespace
///////////////////////////////////////////////////////////////////////////
void init() {
#ifdef SCREEN_MODE_HIRES
  set_screen_mode(ScreenMode::hires);
#else
  set_screen_mode(ScreenMode::lores);
#endif
  planet_famebuffer.palette = PICO8;

  Random::RestartSeed();

  current_planet.SetRadius((int)((float)PLANET_HEIGHT * 0.5f));
  render_planet();

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

  // clear the screen -- screen is a reference to the frame buffer and can be
  // used to draw all things with the 32blit

  screen.pen = Pen(0, 0, 0);
  screen.clear();
  screen.alpha = 255;
  screen.mask = nullptr;

  screen.blit(&planet_famebuffer, Rect(0, 0, PLANET_WIDTH, PLANET_HEIGHT),
              Point(0, 0));
  // blit::debugf("Hello from 32blit time = %lu - %d,%d\n", time,
  // screen.bounds.w, screen.bounds.h);

  // draw some text at the top of the screen
  screen.pen = Pen(255, 255, 255);
  screen.text("Planet!", minimal_font, Point(2, 2));

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

  if (rerender) {
    render_planet();
  }
}
