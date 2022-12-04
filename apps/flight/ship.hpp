#pragma once

#include "pw_color/color.h"
#include "pw_color/colors_pico8.h"
#include "pw_coordinates/vec2.h"
#include "pw_display/display.h"
#include "pw_log/log.h"
#include "random.h"

class Ship {
public:
  Ship();

  bool npc;
  bool hostile;
  Vec2 screen_position;
  Vec2 sector_position;
  float cur_deltav;
  float cur_gees;
  float angle_degrees;
  float angle_radians;
  float velocity_angle;
  float velocity_angle_opposite;
  float velocity;
  Vec2 velocity_vector;
  float turn_rate;
  float deltav;
  bool accelerating;
  Vec2 add_vv;
  int16_t last_fire_time;

  void TurnLeft(float delta_seconds);
  void TurnRight(float delta_seconds);
  void Rotate(float signed_degrees);
  void UpdateLocation();
  void ResetVelocity();
  void Draw();
  bool ReverseDirection(float delta_seconds);
  bool RotateTowardsHeading(float heading, float delta_seconds);
  void ApplyThrust(float max_deltav, float delta_seconds);
  void CutThrust();
};
