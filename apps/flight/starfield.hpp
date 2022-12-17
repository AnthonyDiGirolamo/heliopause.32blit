#pragma once

#include <cstdint>

#include "32blit.hpp"
#include "types/vec2.hpp"

#define STARFIELD_COUNT 64
#define STARFIELD_MARGIN 12
#define STAR_COLOR_LENGTH 6

const uint8_t star_colors[6][STAR_COLOR_LENGTH] = {
    {0xa, 0xe, 0xc, 0xd, 0x7, 0x6}, {0x9, 0x8, 0xd, 0x1, 0x6, 0x5},
    {0x4, 0x2, 0x1, 0x0, 0x5, 0x1}, {0x7, 0x6, 0x7, 0x6, 0x7, 0x6},
    {0x6, 0x5, 0x6, 0x5, 0x6, 0x5}, {0x5, 0x1, 0x5, 0x1, 0x5, 0x1}};

class Star {
public:
  uint8_t color;
  float speed;
  blit::Vec2 position;
  void Reset(int16_t, int16_t);
};

class Starfield {
public:
  Starfield(blit::Surface *frame_buffer);

  void Scroll(blit::Vec2, float delta_seconds);
  void Draw(blit::Vec2, float delta_seconds);
  void ResetAll();

private:
  blit::Surface *frame_buffer;
  float last_delta_seconds;
  Star _stars[STARFIELD_COUNT];
};
