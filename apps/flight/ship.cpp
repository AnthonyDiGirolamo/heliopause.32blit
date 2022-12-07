#include "ship.hpp"
#include "colors.hpp"
#include "random.hpp"

using namespace blit;

Ship::Ship(void) {
  cur_deltav = 0.0;
  cur_gees = 0.0;
  angle_degrees = 0;
  angle_radians = 0.0;
  velocity_angle = 0.0;
  velocity_angle_opposite = kPi;
  velocity = 0.0;
  turn_rate = 270.0; // 270 degrees per second
  deltav = 1.0;
  last_fire_time = -6;
}

void Ship::Rotate(float signed_degrees) {
  angle_degrees = fmod((angle_degrees + signed_degrees), 360);
  if (angle_degrees < 0) {
    angle_degrees = 360 + angle_degrees;
  }
  angle_radians = ((float)angle_degrees) / 180.0f * kPi;
}

void Ship::TurnLeft(float delta_seconds) { Rotate(-turn_rate * delta_seconds); }

void Ship::TurnRight(float delta_seconds) { Rotate(turn_rate * delta_seconds); }

bool Ship::ReverseDirection(float delta_seconds) {
  return velocity > 0
             ? RotateTowardsHeading(velocity_angle_opposite, delta_seconds)
             : false;
}

bool Ship::RotateTowardsHeading(float heading, float delta_seconds) {
  float delta = degrees(heading) - angle_degrees + 180.0f;
  if (delta >= 360.0f)
    delta -= 360.0f;
  delta -= 180.0f;

  if (delta < -1.0f || delta > 1.0f) {
    float abs_delta = delta;
    if (delta < 0) {
      abs_delta *= -1;
    }
    float r = (turn_rate * delta_seconds) * delta / abs_delta;
    float abs_r = r;
    if (r < 0) {
      abs_r *= -1;
    }
    if (abs_delta > abs_r) {
      delta = r;
    }
    // If between 270 and 360 and we are going to 0-45, pick the reverse
    // direction since it's shorter.
    if (degrees(heading) < 45.0f && angle_degrees > 269.0f) {
      delta *= -1;
    }
    Rotate(delta);
  }
  return delta < 0.2f && delta > -0.2f;
}

float rand_float() {
  return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void Ship::Draw(blit::Surface *frame_buffer, Vec2 screen_position) {
  Vec2 ship_top(8, 0);
  Vec2 ship_left_wing(-6, 4);
  Vec2 ship_right_wing(-6, -4);
  Vec2 ship_engine(-2, 0);
  ship_top.rotate(angle_radians);
  ship_top += screen_position;
  ship_left_wing.rotate(angle_radians);
  ship_left_wing += screen_position;
  ship_right_wing.rotate(angle_radians);
  ship_right_wing += screen_position;
  ship_engine.rotate(angle_radians);
  ship_engine += screen_position;

  // Draw ship outline.
  frame_buffer->pen = PICO8[12];
  frame_buffer->line(ship_top, ship_left_wing);
  // pw::display::DrawLine(ship_top.x, ship_top.y, ship_left_wing.x,
  //                       ship_left_wing.y, pw::framebuffer::colors_pico8[12]);
  frame_buffer->line(ship_top, ship_right_wing);
  // pw::display::DrawLine(ship_top.x, ship_top.y, ship_right_wing.x,
  //                       ship_right_wing.y,
  //                       pw::framebuffer::colors_pico8[12]);
  frame_buffer->line(ship_engine, ship_left_wing);
  // pw::display::DrawLine(ship_engine.x, ship_engine.y, ship_left_wing.x,
  //                       ship_left_wing.y, pw::framebuffer::colors_pico8[12]);
  frame_buffer->line(ship_engine, ship_right_wing);
  // pw::display::DrawLine(ship_engine.x, ship_engine.y, ship_right_wing.x,
  //                       ship_right_wing.y,
  //                       pw::framebuffer::colors_pico8[12]);

  // Draw engine flame.
  if (accelerating) {
    Vec2 engine_location(14 * -0.5, 0);
    engine_location.rotate(angle_radians);
    engine_location += screen_position;

    // Vary the flame size based on how long thrust has been applied.
    Vec2 particle_velocity = add_vv;
    particle_velocity *= -0.4 * 14;

    // Orange or yellow flame.
    int8_t color = 9 + Random::GetRandomInteger(2);

    // Randomly shift the flame position slightly.
    Vec2 deflection = Vec2(-particle_velocity.y, particle_velocity.x);
    deflection *= 0.7;
    Vec2 flicker = particle_velocity;
    flicker *= rand_float() + 2.0f;
    Vec2 temp = deflection;
    temp *= (rand_float() - 0.5f);
    flicker += temp;

    // Set the flame vertex positions.
    Vec2 p0 = flicker;
    Vec2 p1 = particle_velocity;
    p0 += engine_location;
    p1 += engine_location;
    Vec2 p2 = p1;
    Vec2 p3 = p1;
    p2 += deflection;
    temp = deflection;
    temp *= -1;
    p3 += temp;

    // Draw flame outline.
    frame_buffer->pen = PICO8[color];
    frame_buffer->line(p2, p0);
    frame_buffer->line(p2, engine_location);
    frame_buffer->line(p3, p0);
    frame_buffer->line(p3, engine_location);

    // pw::display::DrawLine(p2.x, p2.y, p0.x, p0.y,
    //                       pw::framebuffer::colors_pico8[color]);
    // pw::display::DrawLine(p2.x, p2.y, engine_location.x, engine_location.y,
    //                       pw::framebuffer::colors_pico8[color]);
    // pw::display::DrawLine(p3.x, p3.y, p0.x, p0.y,
    //                       pw::framebuffer::colors_pico8[color]);
    // pw::display::DrawLine(p3.x, p3.y, engine_location.x, engine_location.y,
    //                       pw::framebuffer::colors_pico8[color]);
  }
}

void Ship::ApplyThrust(float max_deltav, float delta_seconds) {
  accelerating = true;
  if (cur_deltav < deltav) {
    cur_deltav = cur_deltav + (deltav * delta_seconds);
  } else {
    cur_deltav = deltav;
  }
  float dv = cur_deltav;
  if (dv > max_deltav) {
    dv = max_deltav;
  }

  cur_gees = dv * (30.0f / 9.806f);
  Vec2 additional_velocity_vector(cosf(angle_radians) * dv,
                                  sinf(angle_radians) * dv);
  // Save the additional_velocity_vector to vary the thrust flame size.
  add_vv = additional_velocity_vector;

  velocity_vector += additional_velocity_vector;
  velocity = velocity_vector.length();

  velocity_angle = atan2f(velocity_vector.y, velocity_vector.x);
  if (velocity_angle < 0) {
    velocity_angle = kTwoPi + velocity_angle;
  }

  velocity_angle_opposite = velocity_angle + kPi;
  if (velocity_angle_opposite > kTwoPi) {
    velocity_angle_opposite -= kTwoPi;
  }
}

void Ship::DampenSpeed(float delta_seconds) {
  if (velocity < 1.2f * deltav) {
    ResetVelocity();
    return;
  }

  Vec2 cvn = velocity_vector;
  cvn.normalize();
  // 30.0 here should be influenced by the ship mass
  velocity_vector += (cvn * delta_seconds * -30.0f);
  velocity = velocity_vector.length();
}

void Ship::ResetVelocity() {
  velocity_vector.x = 0.0f;
  velocity_vector.y = 0.0f;
  velocity = 0.0f;
}

void Ship::CutThrust() {
  accelerating = false;
  cur_deltav = 0;
}

void Ship::UpdateLocation(float delta_seconds) {
  if (velocity > 0) {
    sector_position += (velocity_vector * delta_seconds);
  }
}
