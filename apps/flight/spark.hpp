#pragma once

#include "pw_color/color.h"
#include "pw_color/colors_pico8.h"
#include "pw_coordinates/vec2.h"
#include "pw_display/display.h"
#include "pw_log/log.h"
#include "random.h"

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
