#pragma once

#include "pw_random/xor_shift.h"
#include <stdint.h>

namespace Random {

uint32_t GetCurrentSeed();
void RestartSeed();
void IncrementSeed(int diff);
void SetSeed(uint32_t seed);
uint32_t GetRandomNumber();
int GetRandomInteger(uint32_t max_value);
int GetRandomInteger(uint32_t min_value, uint32_t max_value);
float GetRandomFloat(float max_value);
float GetRandomFloat(float min_value, float max_value);

inline uint32_t GetRandomNumber(pw::random::XorShiftStarRng64 *rng) {
  uint32_t random_value = 0;
  rng->GetInt(random_value);
  return random_value;
}

int GetRandomInteger(pw::random::XorShiftStarRng64 *rng, uint32_t max_value);
int GetRandomInteger(pw::random::XorShiftStarRng64 *rng, uint32_t min_value,
                     uint32_t max_value);
float GetRandomFloat(pw::random::XorShiftStarRng64 *rng, float max_value);
float GetRandomFloat(pw::random::XorShiftStarRng64 *rng, float min_value,
                     float max_value);

} // namespace Random
