#include "game.hpp"
#include "SimplexNoise.h"
#include "engine/engine.hpp"
#include <cstdint>

using namespace blit;

namespace {
#define PLANET_WIDTH 64
#define PLANET_HEIGHT 32

uint8_t planet_pixel_data[PLANET_WIDTH * PLANET_HEIGHT];
Surface planet_terrain((uint8_t *)planet_pixel_data, PixelFormat::P,
                       Size(PLANET_WIDTH, PLANET_HEIGHT));
} // namespace

///////////////////////////////////////////////////////////////////////////
void init() {
  set_screen_mode(ScreenMode::lores);
  planet_terrain.palette = PICO8;

  Planet planet;

  int terrain_color_count = 19;
  float noisedx = 42.0f;
  float noisedy = 16.0f;
  float noisedz = 32.0f;

  // TODO: Convert to 3D noise sampled in the shape of a sphere.
  for (int x = 0; x < PLANET_WIDTH; x++) {
    for (int y = 0; y < PLANET_HEIGHT; y++) {
      float freq = planet.noise_zoom;
      float max_amp = 0;
      float amp = 1;
      float noise = 0;

      for (int n = 0; n < planet.noise_octaves; n++) {
        // 2D Noise for now
        noise = noise + SimplexNoise::noise(noisedx + freq * (x * 0.02f),
                                            noisedy + freq * (y * 0.02f));
        max_amp = max_amp + amp;
        amp = amp * planet.noise_persistance;
        freq = freq * 2;
      }
      noise = noise / max_amp;
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
    }
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

  screen.blit(&planet_terrain, Rect(0, 0, 64, 32), Point(0, 44));
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