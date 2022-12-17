#include "starfield.hpp"
#include "colors.hpp"
#include "random.hpp"

#include <cstdint>

using namespace Random;

Starfield::Starfield(blit::Surface *fb) {
  frame_buffer = fb;
  last_delta_seconds = 0.0166; // assuming 60fps
}

void Starfield::ResetAll() {
  // Set initial random star locations.
  for (uint8_t i = 0; i < STARFIELD_COUNT; i++) {
    _stars[i].Reset(
        GetRandomInteger(2 * STARFIELD_MARGIN + frame_buffer->bounds.w) -
            STARFIELD_MARGIN,
        GetRandomInteger(2 * STARFIELD_MARGIN + frame_buffer->bounds.h) -
            STARFIELD_MARGIN);
  }
}

void Star::Reset(int16_t x, int16_t y) {
  position.x = x;
  position.y = y;
  color = GetRandomInteger(STAR_COLOR_LENGTH);
  speed = (((float)GetRandomInteger(75)) / 100) + 0.25f;
}

// Move each star based on ship and star speed.
void Starfield::Scroll(blit::Vec2 ship_velocity, float delta_seconds) {
  blit::Vec2 vel;
  for (uint8_t i = 0; i < STARFIELD_COUNT; i++) {
    vel = ship_velocity;
    vel *= _stars[i].speed * -1;
    vel *= delta_seconds;
    _stars[i].position += vel;

    // If a star has scrolled off-screen on any edge, reset it's position to the
    // opposite edge.
    if (_stars[i].position.x > frame_buffer->bounds.w + STARFIELD_MARGIN) {
      _stars[i].Reset(-STARFIELD_MARGIN,
                      GetRandomInteger(frame_buffer->bounds.h));
    } else if (_stars[i].position.x < -STARFIELD_MARGIN) {
      _stars[i].Reset(frame_buffer->bounds.w + STARFIELD_MARGIN,
                      GetRandomInteger(frame_buffer->bounds.h));
    } else if (_stars[i].position.y >
               frame_buffer->bounds.h + STARFIELD_MARGIN) {
      _stars[i].Reset(GetRandomInteger(frame_buffer->bounds.w),
                      -STARFIELD_MARGIN);
    } else if (_stars[i].position.y < -STARFIELD_MARGIN) {
      _stars[i].Reset(GetRandomInteger(frame_buffer->bounds.w),
                      frame_buffer->bounds.h + STARFIELD_MARGIN);
    }
  }
}

void Starfield::Draw(blit::Vec2 ship_velocity, float delta_seconds) {
  blit::Vec2 lstart, lend;
  blit::Vec2 star_velocity;
  blit::Vec2 position;

  if (delta_seconds > 0)
    last_delta_seconds = delta_seconds;

  for (uint8_t i = 0; i < STARFIELD_COUNT; i++) {
    position = _stars[i].position;
    star_velocity = ship_velocity * _stars[i].speed;
    star_velocity *= last_delta_seconds;
    lstart = (star_velocity * -2.5) + position;
    lend = (star_velocity * 2.5) + position;

    frame_buffer->pen = PICO8[star_colors[1][_stars[i].color]];
    frame_buffer->line(position, lend);
    frame_buffer->pen = PICO8[star_colors[0][_stars[i].color]];
    frame_buffer->line(lstart, position);

    // pw::draw::DrawLine(
    //     frame_buffer,
    //     round(position.x),
    //     round(position.y),
    //     round(lend.x),
    //     round(lend.y),
    //     pw::color::colors_pico8_rgb565[star_colors[1][_stars[i].color]]);
    // pw::draw::DrawLine(
    //     frame_buffer,
    //     round(lstart.x),
    //     round(lstart.y),
    //     round(position.x),
    //     round(position.y),
    //     pw::color::colors_pico8_rgb565[star_colors[0][_stars[i].color]]);
  }
}
