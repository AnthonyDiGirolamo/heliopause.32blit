#pragma once

#include <stdint.h>

namespace lib_trig {

float rsin(float theta);
float rcos(float theta);
int16_t rsin(int16_t r, uint16_t th);
int16_t rcos(int16_t r, uint16_t th);
void polar(int &x, int &y, int16_t r, uint16_t th);
uint16_t atan2(int16_t y, int16_t x);

} // namespace lib_trig
