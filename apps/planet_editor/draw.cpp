#include "32blit.hpp"
#include <stdint.h>

namespace Draw {
void rectangle(blit::Surface *fb, int x1, int y1, int w, int h,
               bool filled = false) {
  if (filled) {
  } else {
    // Top
    fb->h_span(blit::Point(x1, y1), w);
    // Left
    fb->v_span(blit::Point(x1, y1), h);
    // Right
    fb->v_span(blit::Point(x1 + w - 1, y1), h);
    // Bottom
    fb->h_span(blit::Point(x1, y1 + h - 1), w);
  }
}

void circle(blit::Surface *fb, int center_x, int center_y, int radius,
            bool filled = false) {
  int fx = 0, fy = 0;
  int x = -radius, y = 0;
  int error_value = 2 - 2 * radius;
  while (x < 0) {
    if (!filled) {
      fx = x;
      fy = y;
    }
    // Draw each quarter circle
    for (int i = x; i <= fx; i++) {
      // Lower right
      fb->pixel(blit::Point(center_x - i, center_y + y));
      // Upper left
      fb->pixel(blit::Point(center_x + i, center_y - y));
    }
    for (int i = fy; i <= y; i++) {
      // Lower left
      fb->pixel(blit::Point(center_x - i, center_y - x));
      // Upper right
      fb->pixel(blit::Point(center_x + i, center_y + x));
    }
    radius = error_value;
    if (radius <= y) {
      y++;
      error_value += y * 2 + 1;
    }
    if (radius > x || error_value > y) {
      x++;
      error_value += x * 2 + 1;
    }
  }
}

} // namespace Draw
