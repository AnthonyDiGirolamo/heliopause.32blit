#include "random.hpp"
#include "32blit.hpp"

#include <stdint.h>

namespace Random {

namespace {

#define RANDOM_TYPE_HRNG 0
#define RANDOM_TYPE_PRNG 1

uint8_t current_random_source = RANDOM_TYPE_PRNG;
uint32_t current_random_seed = 0x64063701;
uint32_t prng_lfsr = 0;
const uint16_t prng_tap = 0x74b8;

} // namespace

uint32_t GetCurrentSeed() { return current_random_seed; }

void ResetSeed() { prng_lfsr = current_random_seed; }

void IncrementSeed(int diff) {
  current_random_seed += diff;
  ResetSeed();
}

void SetSeed(uint32_t seed) { current_random_seed = seed; }

uint32_t GetRandomNumber() {
  switch (current_random_source) {
  default:
    return 0;
  case RANDOM_TYPE_HRNG:
    return blit::random();
  case RANDOM_TYPE_PRNG:
    uint8_t lsb = prng_lfsr & 1;
    prng_lfsr >>= 1;

    if (lsb) {
      prng_lfsr ^= prng_tap;
    }
    return prng_lfsr;
  }
}

int GetRandomInteger(uint32_t max_value) {
  return (int)(GetRandomNumber() % max_value);
}

int GetRandomInteger(uint32_t min_value, uint32_t max_value) {
  int diff = max_value - min_value;
  if (diff < 0)
    diff *= -1;

  int r = GetRandomNumber() % (uint32_t)(diff);
  r += min_value;

  return r;
}

float GetRandomFloat(float max_value) {
  uint32_t r = GetRandomNumber() % (uint32_t)(max_value);
  uint32_t d = GetRandomNumber() % 1000000;
  float decimal_part = (float)d / 1000000.0f;
  float x = (float)r + decimal_part;
  return x;
}

float GetRandomFloat(float min_value, float max_value) {
  float diff = max_value - min_value;
  float r = GetRandomFloat(diff);
  r += min_value;
  return r;
}

} // namespace Random
