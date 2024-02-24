#pragma once

#include <stdint.h>

namespace lib_trig {

constexpr float kPi = 3.1415926535897932384626433832795f;
constexpr float kHalfPi = 1.5707963267948966192313216916398f;
constexpr float kTwoPi = 6.283185307179586476925286766559f;
constexpr float DegreesToRadians = 0.017453292519943295769236907684886f;
constexpr float RadiansToDegrees = 57.295779513082320876798154814105f;

inline float radians(float deg) { return DegreesToRadians * deg; }
inline float degrees(float rad) { return RadiansToDegrees * rad; }

float rsin(float theta);
float rcos(float theta);
int16_t rsin(int16_t r, uint16_t th);
int16_t rcos(int16_t r, uint16_t th);
void polar(int &x, int &y, int16_t r, uint16_t th);
uint16_t atan2(int16_t y, int16_t x);

} // namespace lib_trig
