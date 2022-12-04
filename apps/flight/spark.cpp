#include "spark.hpp"
#include "colors.hpp"
#include "random.hpp"

using namespace Random;

Spark::Spark(Vec2 p_velocity, Vec2 s_position, uint8_t c) {
  Spark(p_velocity, s_position, c, GetRandomInteger(5) + 2);
}

Spark::Spark(Vec2 p_velocity, Vec2 s_position, uint8_t c, uint8_t d) {
  particle_velocity = p_velocity;
  screen_position = s_position;
  color = c;
  duration = d;
}

bool Spark::IsOffscreen() const {
  return (screen_position.x < -SPARK_MARGIN ||
          screen_position.y < -SPARK_MARGIN ||
          screen_position.x > pw::display::GetWidth() + SPARK_MARGIN ||
          screen_position.y > pw::display::GetHeight() + SPARK_MARGIN);
}

void Spark::Update(Vec2 ship_velocity) {
  if (duration > 0) {
    Vec2 temp = particle_velocity;
    temp -= ship_velocity;
    screen_position += temp;
    duration -= 1;
  }
}

void Spark::Draw(Vec2 ship_velocity) {
  if (duration > 0) {
    pw::display::DrawPixel(round(screen_position.x), round(screen_position.y),
                           pw::framebuffer::colors_pico8[color]);
    Update(ship_velocity);
  }
}
