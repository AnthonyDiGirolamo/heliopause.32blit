#pragma once

#include "32blit.hpp"
#include <stdint.h>

namespace Draw {

void rectangle(blit::Surface *fb, int x1, int y1, int x2, int y2,
               bool filled = false);
void circle(blit::Surface *framebuffer, int center_x, int center_y, int radius,
            bool filled = false);

} // namespace Draw
