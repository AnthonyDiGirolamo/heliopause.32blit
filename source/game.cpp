#include "game.hpp"
#include "SimplexNoise.h"
#include "engine/engine.hpp"
#include "math/constants.hpp"
#include "planet.hpp"
#include "planet_types.hpp"
#include <chrono>
#include <stdint.h>

using namespace blit;

namespace {
#define PLANET_WIDTH 320
#define PLANET_HEIGHT 160

uint8_t planet_pixel_data[PLANET_WIDTH * PLANET_HEIGHT];
Surface planet_terrain((uint8_t *)planet_pixel_data, PixelFormat::P,
                       Size(PLANET_WIDTH, PLANET_HEIGHT));

#define RANDOM_TYPE_HRNG 0
#define RANDOM_TYPE_PRNG 1

uint8_t current_random_source = RANDOM_TYPE_PRNG;
// uint32_t current_random_seed = 0x64063701;
uint32_t current_random_seed = 0x3701;

uint32_t prng_lfsr = 0;
const uint16_t prng_tap = 0x74b8;

void reset_seed() { prng_lfsr = current_random_seed; }

uint32_t get_random_number() {
  switch (current_random_source) {
  default:
    return 0;
  case RANDOM_TYPE_HRNG:
    return blit::random();
  case RANDOM_TYPE_PRNG:
    uint8_t lsb = prng_lfsr & 1;
    prng_lfsr >>= 1;

    if (lsb) {
      prng_lfsr ^= prng_tap;
    }
    return prng_lfsr;
  }
}

int selected_planet_index = 0;

void render_planet() {
  Planet *current_planet = PlanetSpan[selected_planet_index];
  planet_terrain.palette = PICO8;
  // Planet current_planet = kTerranPlanet;

  int terrain_color_count = current_planet->color_map.size();

  float noisedx = (float)(get_random_number() % 1024);
  float noisedy = (float)(get_random_number() % 1024);
  float noisedz = (float)(get_random_number() % 1024);

  float noise_factor_vert = 1.0;
  if (current_planet->max_noise_stretch - current_planet->min_noise_stretch >
      0) {
    int stretch_factor =
        current_planet->min_noise_stretch +
        (get_random_number() % (current_planet->max_noise_stretch -
                                current_planet->min_noise_stretch));
    noise_factor_vert = (float)stretch_factor;
  }
  // blit::debugf("noise_factor_vert: %d\n", (int)noise_factor_vert);
  blit::debugf("noise_factor_vert: %d.%.6d\n", (int)noise_factor_vert,
               (int)((noise_factor_vert - (int)noise_factor_vert) * 1000000));

  float min_noise = 2;
  float max_noise = -2;

  float theta_start = 0;
  float theta_end = 2.0f * pi;
  float theta_increment = theta_end / PLANET_WIDTH;
  float theta = theta_start;
  float phi = pi * -0.5f;
  float phi_increment = pi / PLANET_HEIGHT;

  SimplexNoise simplex_noise(current_planet->noise_zoom, 1.0, 2.0,
                             current_planet->noise_persistance);

  for (int y = 0; y < PLANET_HEIGHT; y++) {

    theta = 0;

    for (int x = 0; x < PLANET_WIDTH; x++) {

      float noise = 0;

      // clang-format off
      noise = simplex_noise.fractal(
          current_planet->noise_octaves,
          noisedx + cosf(phi) * cosf(theta),
          noisedy + cosf(phi) * sinf(theta),
          noisedz + sinf(phi) * noise_factor_vert);
      // clang-format on

      // // Manual Octave Summation
      // float freq = current_planet.noise_zoom;
      // float max_amp = 0;
      // float amp = 1;
      // for (int n = 0; n < current_planet.noise_octaves; n++) {
      //   noise = noise + SimplexNoise::noise(
      //     noisedx + cosf(phi) * cosf(theta),
      //     noisedy + cosf(phi) * sinf(theta),
      //     noisedz + sinf(phi));
      //   max_amp = max_amp + amp;
      //   amp = amp * current_planet.noise_persistance;
      //   freq = freq * 2;
      // }
      // noise = noise / max_amp;

      if (noise > max_noise)
        max_noise = noise;
      if (noise < min_noise)
        min_noise = noise;

      if (noise > 1)
        noise = 1;
      if (noise < -1)
        noise = -1;
      noise = noise + 1;

      int terrain_color_index =
          (int)(noise * 0.5f * (float)terrain_color_count);
      if (terrain_color_index < 0)
        terrain_color_index = 0;
      if (terrain_color_index >= terrain_color_count)
        terrain_color_index = terrain_color_count - 1;

      // blit::debugf("%d.%.6d -> %d\n", (int)noise,
      //              (int)((noise - (int)noise) * 1000000),
      //              (int)terrain_color_index);
      // planet_terrain.pen = PICO8[terran_color_map[terrain_color_index]];

      // Get indexed color value
      planet_terrain.pen = current_planet->color_map[terrain_color_index];
      planet_terrain.pixel(Point(x, y));

      theta += theta_increment;
    }

    phi += phi_increment;
  }

  blit::debugf("min: %d.%.6d\n", (int)min_noise,
               (int)((min_noise - (int)min_noise) * 1000000));
  blit::debugf("max: %d.%.6d\n", (int)max_noise,
               (int)((max_noise - (int)max_noise) * 1000000));

  blit::debugf("PlanetSpanCount: %d\n", PlanetSpan.size());
  for (Planet *p : PlanetSpan) {
    blit::debugf("Type: %d = %s\n", p->type, p->type_name);

    blit::debugf("  ColorMap: ");
    for (int i : p->color_map) {
      blit::debugf("%d, ", i);
    }
    blit::debugf("\n");
  }

  // for (int i : current_planet.color_map) {
  //   blit::debugf("kTerranColorMap: %d\n", i);
  // }

  // for (int i = 0; i < kTerranColorMap.size(); i++) {
  //   blit::debugf("kTerranColorMap: %d = %d\n", i, kTerranColorMap[i]);
  // }
}

void next_planet() {
  selected_planet_index = (selected_planet_index + 1) % PlanetSpan.size();
}

} // namespace
///////////////////////////////////////////////////////////////////////////
void init() {
  // set_screen_mode(ScreenMode::lores);
  set_screen_mode(ScreenMode::hires);

  reset_seed();

  render_planet();
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

  screen.blit(&planet_terrain, Rect(0, 0, PLANET_WIDTH, PLANET_HEIGHT),
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
    current_random_seed += 1;
    reset_seed();
    render_planet();
  } else if (buttons.pressed & Button::DPAD_DOWN) {
    current_random_seed -= 1;
    reset_seed();
    render_planet();
  } else if (buttons.pressed & Button::DPAD_RIGHT) {
    next_planet();
    render_planet();
  }
  // blit::debugf("Hello from 32blit time = %lu - %d,%d\n", time,
  //              screen.bounds.w, screen.bounds.h);
  // float x     = 0.123f;                   // Define a float coordinate
  // float noise = SimplexNoise::noise(x, x, x);   // Get the noise value for
  // the coordinate blit::debugf("%d.%.6d\n", (int)noise,
  //              (int)((noise-(int)noise)*1000000));
}
