#include "game.hpp"
#include "SimplexNoise.h"
#include "engine/engine.hpp"
#include "math/constants.hpp"
#include <cstdint>

using namespace blit;

namespace {
#define PLANET_WIDTH 120
#define PLANET_HEIGHT 60

uint8_t planet_pixel_data[PLANET_WIDTH * PLANET_HEIGHT];
Surface planet_terrain((uint8_t *)planet_pixel_data, PixelFormat::P,
                       Size(PLANET_WIDTH, PLANET_HEIGHT));

#define RANDOM_TYPE_HRNG 0
#define RANDOM_TYPE_PRNG 1

uint8_t current_random_source = RANDOM_TYPE_PRNG;
uint32_t current_random_seed = 0x64063701;

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

} // namespace

///////////////////////////////////////////////////////////////////////////
void init() {
  set_screen_mode(ScreenMode::lores);

  reset_seed();

  planet_terrain.palette = PICO8;

  Planet planet;

  int terrain_color_count = 19;

  float noisedx = (float)(get_random_number() % 1024); // 12.0f;
  float noisedy = (float)(get_random_number() % 1024); // 33.0f;
  float noisedz = (float)(get_random_number() % 1024); // 8.0f;

  // noise_factor_vert=random_int(planet_type.max_noise_stretch_factor + 1,
  // planet_type.min_noise_stretch_factor);

  float min_noise = 2;
  float max_noise = -2;

  float theta_start = 0;
  float theta_end = 2.0f * pi;
  float theta_increment = theta_end / PLANET_WIDTH;
  float theta = theta_start;
  float phi = pi * -0.5f;
  float phi_increment = pi / PLANET_HEIGHT;

  SimplexNoise simplex_noise(planet.noise_zoom, 1.0, 3.0,
                             planet.noise_persistance);

  for (int y = 0; y < PLANET_HEIGHT; y++) {

    theta = 0;

    for (int x = 0; x < PLANET_WIDTH; x++) {
      // float freq = planet.noise_zoom;
      // float max_amp = 0;
      // float amp = 1;
      float noise = 0;

      noise = simplex_noise.fractal(
          planet.noise_octaves, noisedx + cosf(phi) * cosf(theta),
          noisedy + cosf(phi) * sinf(theta), noisedz + sinf(phi));

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

      planet_terrain.pen = terran_color_map[terrain_color_index];
      planet_terrain.pixel(Point(x, y));

      theta += theta_increment;
    }

    phi += phi_increment;
  }

  blit::debugf("min: %d.%.6d\n", (int)min_noise,
               (int)((min_noise - (int)min_noise) * 1000000));
  blit::debugf("max: %d.%.6d\n", (int)max_noise,
               (int)((max_noise - (int)max_noise) * 1000000));
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
  screen.text("Hello 32blit!", minimal_font, Point(5, 4));

  int rect_width = 8;

  for (int i = 0; i < 16; i++) {
    screen.pen = PICO8[i];
    screen.rectangle(
        Rect(2 + (i * (rect_width + 1)), 16, rect_width, rect_width));
  }

  int x_offset = 0;
  for (int i = 0; i < 32; i++) {
    screen.pen = ENDESGA32[i];

    int x = (i * (rect_width + 1));
    int y = 25;
    if (i > 15) {
      y = 34;
      x -= x_offset + rect_width + 1;
    } else {
      x_offset = x;
    }
    screen.rectangle(Rect(2 + x, y, rect_width, rect_width));
  }

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
  // blit::debugf("Hello from 32blit time = %lu - %d,%d\n", time,
  //              screen.bounds.w, screen.bounds.h);
  // float x     = 0.123f;                   // Define a float coordinate
  // float noise = SimplexNoise::noise(x, x, x);   // Get the noise value for
  // the coordinate blit::debugf("%d.%.6d\n", (int)noise,
  //              (int)((noise-(int)noise)*1000000));
}
