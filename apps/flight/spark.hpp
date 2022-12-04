#pragma once

#include "32blit.hpp"
#include "types/vec2.hpp"

using namespace blit;

#define SPARK_MARGIN 12

class Spark {
public:
  Spark(Vec2 particle_velocity, Vec2 screen_position, uint8_t color);
  Spark(Vec2 particle_velocity, Vec2 screen_position, uint8_t color,
        uint8_t duration);
  uint8_t color;
  uint8_t duration; // 0 to 255;
  Vec2 screen_position;
  Vec2 particle_velocity;
  bool IsOffscreen() const;
  void Update(Vec2 ship_velocity);
  void Draw(Vec2 ship_velocity);
};
