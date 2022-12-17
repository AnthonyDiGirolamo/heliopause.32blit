#pragma once

#include "32blit.hpp"
#include <cstdint>

#include "planet.hpp"
#include "pw_random/random.h"
#include "pw_random/xor_shift.h"

class Sector {
public:
  Sector(uint32_t initial_seed);

  uint32_t seed;
  pw::random::XorShiftStarRng64 rng;
  // std::array<Planet, 16> planets;

  void Draw(blit::Surface *fb, blit::Vec2 pilot_position, float delta_seconds);
  void Update(blit::Vec2 pilot_position, float delta_seconds);
};
