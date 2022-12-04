// Copyright 2020 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.
#include "ship.h"

Ship::Ship(void) {
  screen_position.set(pw::display::GetWidth() / 2,
                      pw::display::GetHeight() / 2);
  cur_deltav = 0.0;
  cur_gees = 0.0;
  angle_degrees = 0;
  angle_radians = 0.0;
  velocity_angle = 0.0;
  velocity_angle_opposite = kPi;
  velocity = 0.0;
  turn_rate = 180.0; // 180 degrees per second
  deltav = 1.0;
  last_fire_time = -6;
}

void Ship::Rotate(float signed_degrees) {
  angle_degrees = fmod((angle_degrees + signed_degrees), 360);
  if (angle_degrees < 0) {
    angle_degrees = 360 + angle_degrees;
  }
  angle_radians = ((float)angle_degrees) / 180.0 * kPi;
}

void Ship::TurnLeft(float delta_seconds) { Rotate(turn_rate * delta_seconds); }

void Ship::TurnRight(float delta_seconds) {
  Rotate(-turn_rate * delta_seconds);
}

bool Ship::ReverseDirection(float delta_seconds) {
  return velocity > 0
             ? RotateTowardsHeading(velocity_angle_opposite, delta_seconds)
             : false;
}

bool Ship::RotateTowardsHeading(float heading, float delta_seconds) {
  float delta = fmod((degrees(heading) - angle_degrees + 180), 360) - 180;
  if (delta < -1 || delta > 1) {
    float r = (turn_rate * delta_seconds) * delta / abs(delta);
    if (abs(delta) > abs(r)) {
      delta = r;
    }
    Rotate(delta);
  }
  return delta < 0.2 && delta > -0.2;
}

float rand_float() {
  return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void Ship::Draw() {
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
  pw::display::DrawLine(ship_top.x, ship_top.y, ship_left_wing.x,
                        ship_left_wing.y, pw::framebuffer::colors_pico8[12]);
  pw::display::DrawLine(ship_top.x, ship_top.y, ship_right_wing.x,
                        ship_right_wing.y, pw::framebuffer::colors_pico8[12]);
  pw::display::DrawLine(ship_engine.x, ship_engine.y, ship_left_wing.x,
                        ship_left_wing.y, pw::framebuffer::colors_pico8[12]);
  pw::display::DrawLine(ship_engine.x, ship_engine.y, ship_right_wing.x,
                        ship_right_wing.y, pw::framebuffer::colors_pico8[12]);

  // Draw engine flame.
  if (accelerating) {
    Vec2 engine_location(14 * -0.5, 0);
    engine_location.rotate(angle_radians);
    engine_location += screen_position;

    // Vary the flame size based on how long thrust has been applied.
    Vec2 particle_velocity = add_vv;
    particle_velocity *= -0.4 * 14;

    // Orange or yellow flame.
    int8_t color = 9 + RandomInt(2);

    // Randomly shift the flame position slightly.
    Vec2 deflection = particle_velocity.perpendicular();
    deflection *= 0.7;
    Vec2 flicker = particle_velocity;
    flicker *= rand_float() + 2.0;
    Vec2 temp = deflection;
    temp *= (rand_float() - 0.5);
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
    pw::display::DrawLine(p2.x, p2.y, p0.x, p0.y,
                          pw::framebuffer::colors_pico8[color]);
    pw::display::DrawLine(p2.x, p2.y, engine_location.x, engine_location.y,
                          pw::framebuffer::colors_pico8[color]);
    pw::display::DrawLine(p3.x, p3.y, p0.x, p0.y,
                          pw::framebuffer::colors_pico8[color]);
    pw::display::DrawLine(p3.x, p3.y, engine_location.x, engine_location.y,
                          pw::framebuffer::colors_pico8[color]);
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

  cur_gees = dv * (30 / 9.806);
  Vec2 additional_velocity_vector(cos(angle_radians) * dv,
                                  sin(angle_radians) * dv);
  // Save the additional_velocity_vector to vary the thrust flame size.
  add_vv = additional_velocity_vector;

  velocity_vector += additional_velocity_vector;
  velocity = velocity_vector.length();
  velocity_angle = velocity_vector.angle();
  if (velocity_angle < 0) {
    velocity_angle = kTwoPi + velocity_angle;
  }

  velocity_angle_opposite = velocity_angle + kPi;
  if (velocity_angle_opposite > kTwoPi) {
    velocity_angle_opposite -= kTwoPi;
  }
}

void Ship::CutThrust() {
  accelerating = false;
  cur_deltav = 0;
}
