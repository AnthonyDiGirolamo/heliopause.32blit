#include "planet.hpp"
#include "colors.hpp"
#include "pw_random/xor_shift.h"
#include <cstdint>
#include <math.h>

// constexpr float kPi = blit::pi;
// constexpr float kHalfPi = blit::pi * 0.5f;
// constexpr float kTwoPi = blit::pi * 2.0f;
// constexpr float kFourPi = blit::pi * 4.0f;

// float fast_atan2f(float x) {
//   return kFourPi*x - x*(fabsf(x) - 1)*(0.2447 + 0.0663*fabsf(x));
// }

Planet::Planet(uint32_t seed_value, PlanetTerrain new_terrain)
    : seed(seed_value), rng(seed_value), terrain(new_terrain) {
  viewpoint_phi0 = 0;
  viewpoint_lambda0 = kPi;
  draw_position_x = 0;
  draw_position_y = 0;
  Regen();
}

void Planet::SetTerrain(PlanetTerrain new_terrain) {
  terrain = new_terrain;
  Regen();
}

void Planet::SetSeed(uint32_t seed_value) {
  seed = seed_value;
  Regen();
}

void Planet::SetTerrainAndSeed(uint32_t seed_value, PlanetTerrain new_terrain) {
  seed = seed_value;
  terrain = new_terrain;
  Regen();
}

void Planet::RebuildHeightMap() {
  terrain_heightmap_color_count = terrain.color_map.size();
  terrain_heightmap_color_count += terrain.color_padding_start;
  terrain_heightmap_color_count += terrain.color_padding_end;

  // 3       5           2
  // 0 1 2 - 3 4 5 6 7 - 8 9
  for (int i = 0; i < terrain_heightmap_color_count; i++) {
    if (i < terrain.color_padding_start) {
      // Use the first color
      height_map[i] = terrain.color_map[0];
    } else if (i >=
               terrain.color_padding_start + (int)terrain.color_map.size()) {
      // Use the last color
      height_map[i] = terrain.color_map[terrain.color_map.size() - 1];
    } else {
      height_map[i] = terrain.color_map[i - terrain.color_padding_start];
    }
  }
}

void Planet::Regen() {
  RebuildHeightMap();

  noise_factor_x = 1.0;
  noise_factor_y = 1.0;
  noise_factor_z = 1.0;

  rng = pw::random::XorShiftStarRng64(seed);

  simplex_noise =
      SimplexNoise(terrain.noise_zoom, 1.0f, 2.0f, terrain.noise_persistance);

  noise_offset.x = Random::GetRandomFloat(&rng, 1024);
  noise_offset.y = Random::GetRandomFloat(&rng, 1024);
  noise_offset.z = Random::GetRandomFloat(&rng, 1024);

  if (terrain.max_noise_stretch - terrain.min_noise_stretch > 0) {
    noise_factor_z = Random::GetRandomFloat(&rng, terrain.min_noise_stretch,
                                            terrain.max_noise_stretch);
  } else {
    float unused_result = Random::GetRandomFloat(&rng, 0, 20);
    unused_result += 1;
  }

  // blit::debugf(
  //     "noise_factor_z: %d.%.6d\n", (int)noise_factor_z,
  //     (int)((noise_factor_z - (int)noise_factor_z) * 1000000));
}

float Planet::GetNoise(float theta, float phi) {
  // clang-format off
  float cosf_phi = cosf(phi);
  float sinf_phi = sinf(phi);
  float cosf_theta = cosf(theta);
  float sinf_theta = sinf(theta);

  float noise = simplex_noise.fractal(
      terrain.noise_octaves,
      noise_offset.x + cosf_phi * cosf_theta * noise_factor_x,
      noise_offset.y + cosf_phi * sinf_theta * noise_factor_y,
      noise_offset.z + sinf_phi * noise_factor_z);
  // clang-format on

  float altitude_modifier = (phi * phi) * terrain.latitude_bias;
  noise = noise + altitude_modifier;

  if (theta > max_lambda)
    max_lambda = theta;
  if (theta < min_lambda)
    min_lambda = theta;
  if (noise > max_noise)
    max_noise = noise;
  if (noise < min_noise)
    min_noise = noise;

  if (noise > 1)
    noise = 1;
  if (noise < -1)
    noise = -1;

  return noise;

  // // Manual Octave Summation
  // float freq = current_planet.noise_zoom;
  // float max_amp = 0;
  // float amp = 1;
  // for (int n = 0; n < current_planet.noise_octaves; n++) {
  //   noise = noise + SimplexNoise::noise(
  //     noisedx + cosf(phi) * cosf(theta),
  //     noisedy + cosf(phi) * sinf(theta),
  //     noisedz + sinf(phi));
  //   max_amp = max_amp + amp;
  //   amp = amp * current_planet.noise_persistance;
  //   freq = freq * 2;
  // }
  // noise = noise / max_amp;
}

// Return the height map color index given a noise value between [-1, 1].
int Planet::GetTerrainColorIndex(float noise) {
  // Shift noise value to [0, 1]
  noise = (noise + 1) * 0.5f;

  int index = (int)(noise * (float)terrain_heightmap_color_count);
  if (index < 0)
    index = 0;
  else if (index >= terrain_heightmap_color_count)
    index = terrain_heightmap_color_count - 1;

  if (index > max_color_index)
    max_color_index = index;
  if (index < min_color_index)
    min_color_index = index;

  return index;
}

void Planet::SetDrawPosition(int x, int y) {
  draw_position_x = x;
  draw_position_y = y;
}

void Planet::render_equirectangular(blit::Surface *framebuffer, int map_width,
                                    int map_height) {
  // Erase to non-existent color palette index
  framebuffer->pen = 49;
  // Clear draw area
  framebuffer->rectangle(
      blit::Rect(draw_position_x, draw_position_y, map_width, map_height));

  // Reset min/max tracking
  min_color_index = 255;
  max_color_index = 0;
  min_noise = 2;
  max_noise = -2;
  min_lambda = 1000;
  max_lambda = -1000;

  // Theta value (longitude)
  // We will map all the way around the sphere [0, 2pi]
  float theta_start = 0;
  float theta_end = 2.0f * kPi;
  float theta_increment = theta_end / map_width;
  float theta = theta_start;

  // Phi value (lattitude)
  // North to south pole [-pi/2, pi]
  float phi = kPi * -0.5f;
  float phi_increment = kPi / map_height;

  for (int y = 0; y < map_height; y++) {
    theta = 0;

    for (int x = 0; x < map_width; x++) {
      int dx = draw_position_x + x;
      int dy = draw_position_y + y;

      float noise = GetNoise(theta, phi);

      // Get indexed color value
      int heightmap_color_index = GetTerrainColorIndex(noise);

      framebuffer->pen = height_map[heightmap_color_index];
      framebuffer->pixel(blit::Point(dx, dy));
      theta += theta_increment;
    }

    phi += phi_increment;
  }

  // blit::debugf("min: %d.%.6d\n", (int)min_noise,
  //              (int)((min_noise - (int)min_noise) * 1000000));
  // blit::debugf("max: %d.%.6d\n", (int)max_noise,
  //              (int)((max_noise - (int)max_noise) * 1000000));
}

void Planet::AdjustViewpointLatitude(float amount) {
  viewpoint_phi0 += amount;

  if (viewpoint_phi0 > kHalfPi) {
    viewpoint_phi0 = kHalfPi;
  } else if (viewpoint_phi0 < -kHalfPi) {
    viewpoint_phi0 = -kHalfPi;
  }
}
void Planet::AdjustViewpointLongitude(float amount) {
  viewpoint_lambda0 += amount;
  if (viewpoint_lambda0 > kTwoPi) {
    viewpoint_lambda0 -= kTwoPi;
  } else if (viewpoint_lambda0 < 0) {
    viewpoint_lambda0 += kTwoPi;
  }
}

void Planet::setup_render_orthographic(blit::Surface *framebuffer,
                                       int pixel_width, int pixel_height,
                                       float zoom, int zoom_pan_x,
                                       int zoom_pan_y, uint32_t start_time) {
  // Reset min/max tracking
  min_color_index = 255;
  max_color_index = 0;
  min_noise = 2;
  max_noise = -2;
  min_lambda = 1000;
  max_lambda = -1000;

  // Pic the smaller dimension to be the radius
  int map_size = 0;
  if (pixel_height <= pixel_width)
    map_size = pixel_height;
  else
    map_size = pixel_width;

  // floor(map_size / 2)
  int pixel_radius = (int)((float)map_size * 0.5f);
  // float r = (int)((float)map_size * 0.5f);
  if (zoom > 0)
    pixel_radius = (int)((float)pixel_radius * zoom);

  float r = pixel_radius;

  // phi0 = origin latitude
  float phi0 = viewpoint_phi0;
  // lambda0 = origin longitude
  float lambda0 = viewpoint_lambda0;

  // Calculate initial zoom offset
  int zoom_offset_x = 0;
  int zoom_offset_y = 0;
  if (2 * pixel_radius > pixel_width) {
    zoom_offset_x -= (int)((float)(2 * pixel_radius - pixel_width) * 0.5f);
  }
  if (2 * pixel_radius > pixel_height) {
    zoom_offset_y -= (int)((float)(2 * pixel_radius - pixel_height) * 0.5f);
  }

  // Pan the camera
  zoom_offset_x -= zoom_pan_x;
  zoom_offset_y -= zoom_pan_y;

  // int startx = draw_position_x + zoom_offset_x;
  // int starty = draw_position_y + zoom_offset_y;
  // int endx = startx + (pixel_radius * 2);
  // int endy = starty + (pixel_radius * 2);

  // // Erase to non-existent color palette index
  // framebuffer->pen = 49;
  // // Clear draw area
  // framebuffer->rectangle(
  //     blit::Rect(draw_position_x, draw_position_y,
  //                pixel_width, pixel_height));

  // // Debug outline of entire draw area
  // framebuffer->pen = 7;
  // Draw::rectangle(framebuffer, draw_position_x, draw_position_y, pixel_width,
  //                 pixel_height);

  // // Debug box outline of the planet
  // framebuffer->pen = 9;
  // // Rectangle defined by top-left corner, width, and height.
  // Draw::rectangle(framebuffer, startx, starty, pixel_radius * 2,
  //                 pixel_radius * 2);

  // // Debug draw the circle for the planet
  // framebuffer->pen = 0;
  // Draw::circle(framebuffer,
  //              draw_position_x + zoom_offset_x + pixel_radius, // center x
  //              draw_position_y + zoom_offset_y + pixel_radius, // center y
  //              pixel_radius - 1,                          // radius
  //              true                                       // filled
  // );
  // framebuffer->pen = terrain.map_icon_color;
  // Draw::circle(framebuffer,
  //              draw_position_x + zoom_offset_x + pixel_radius, // center x
  //              draw_position_y + zoom_offset_y + pixel_radius, // center y
  //              pixel_radius - 1,                          // radius
  //              false                                      // filled
  // );

  // printf("\nWxH: [%d, %d] radius %d\n", pixel_width, pixel_height,
  //        pixel_radius);
  // printf("ZoomOffset: [%d, %d]\n", zoom_offset_x, zoom_offset_y);

  ortho_render.pixel_radius = pixel_radius;
  ortho_render.pixel_width = pixel_width;
  ortho_render.pixel_height = pixel_height;

  ortho_render.r = r;
  ortho_render.phi0 = phi0;
  ortho_render.lambda0 = lambda0;

  ortho_render.zoom_offset_x = zoom_offset_x;
  ortho_render.zoom_offset_y = zoom_offset_y;
  ortho_render.done = false;
  ortho_render.start_time = start_time;
  ortho_render.framebuffer = framebuffer;

  ortho_render.current_y = -1;
  // Start from the correct y coord if there is a negative y zoom offset.
  if (zoom_offset_y < 0)
    ortho_render.current_y = (zoom_offset_y * -1) - 1;
}

int Planet::circle_start_x_coord(int y_coord, int radius) {
  int x = -radius, y = 0;
  int error_value = 2 - 2 * radius;

  // printf("radius = %d, y_coord = %d\n", radius, y_coord);

  y_coord -= radius;
  if (y_coord < 0)
    y_coord *= -1;

  int start_x = 0;

  while (x < 0) {
    radius = error_value;

    // printf("y = %d, x = %d\n", y, x);
    // ortho_render.framebuffer->pen = 7;
    // ortho_render.framebuffer->pixel(blit::Point(x +
    //   ortho_render.pixel_radius,
    //   y + ortho_render.pixel_radius));
    if (y == y_coord) {
      start_x = x;
      break;
    }

    if (radius <= y) {
      y++;
      error_value += y * 2 + 1;
    }
    if (radius > x || error_value > y) {
      x++;
      error_value += x * 2 + 1;
    }
  }
  return start_x;
}

bool Planet::render_orthographic_done() { return ortho_render.done; }

uint32_t Planet::render_orthographic_start_time() {
  return ortho_render.start_time;
}

void Planet::render_orthographic_all() {
  while (!ortho_render.done) {
    render_orthographic_line();
  }
}

void Planet::render_orthographic_line() {
  if (ortho_render.current_y <
      ortho_render.pixel_height - ortho_render.zoom_offset_y) {
    ortho_render.current_y++;
  } else {
    ortho_render.done = true;
  }
  if (ortho_render.done)
    return;

  // printf("y = %d, modified = %d", ortho_render.current_y, draw_position_y +
  //        ortho_render.zoom_offset_y + ortho_render.current_y);

  // If off screen, skip
  if (ortho_render.zoom_offset_y + ortho_render.current_y < 0 ||
      ortho_render.zoom_offset_y + ortho_render.current_y >=
          (ortho_render.pixel_height - 1)) {
    // printf(" skip\n");
    return;
  }
  // printf("\n");

  int radius = ortho_render.pixel_radius - 1;
  int y_coord = ortho_render.current_y;

  int start_x_coord =
      circle_start_x_coord(y_coord, radius) + ortho_render.pixel_radius;
  int end_x_coord = (ortho_render.pixel_radius * 2) - start_x_coord;

  // debug draw x points
  // ortho_render.framebuffer->pen = 10;

  // Stop if there's no pixels to draw
  if (start_x_coord == end_x_coord) {
    ortho_render.done = true;
    return;
  }

  for (int x = start_x_coord; x <= end_x_coord; x++) {
    if (ortho_render.zoom_offset_x + x < 0)
      continue;
    if (ortho_render.zoom_offset_x + x > (ortho_render.pixel_width - 1))
      break;

    int palette_color_index = terrain_color_index(x, y_coord + 1);
    if (palette_color_index == 48 || palette_color_index == 49)
      continue;

    int draw_x = draw_position_x + ortho_render.zoom_offset_x + x;
    int draw_y = draw_position_y + ortho_render.zoom_offset_y + y_coord + 1;

    // Directly set the color in the framebuffer data
    uint32_t data_offset = ortho_render.framebuffer->offset(draw_x, draw_y);
    ortho_render.framebuffer->data[data_offset] = palette_color_index;

    // Set the color using pen and point
    // ortho_render.framebuffer->pen = palette_color_index;
    // ortho_render.framebuffer->pixel(blit::Point(draw_x, draw_y));
  }

  // // debug draw start+end points
  // ortho_render.framebuffer->pen = 12;
  // ortho_render.framebuffer->pixel(
  //     blit::Point(draw_position_x + ortho_render.zoom_offset_x +
  //     start_x_coord,
  //                 draw_position_y + ortho_render.zoom_offset_y + y_coord +
  //                 1));
  // ortho_render.framebuffer->pen = 8;
  // ortho_render.framebuffer->pixel(
  //     blit::Point(draw_position_x + ortho_render.zoom_offset_x + end_x_coord,
  //                 draw_position_y + ortho_render.zoom_offset_y + y_coord +
  //                 1));

  return;
}

int Planet::terrain_color_index(int x, int y_coord) {
  // https://en.wikipedia.org/wiki/Orthographic_map_projection
  // Get x,y coords for map projection.
  // xf, yf = 0, 0 should be at the center of the globe
  float xf = (float)x - ortho_render.r;
  float yf = (float)y_coord - ortho_render.r;

  // Some trig function breaks if xf and xy are both zero. Set a slight
  // offset.
  if ((y_coord - ortho_render.pixel_radius == 0) &&
      (x - ortho_render.pixel_radius == 0)) {
    xf = 0.0001;
    yf = 0.0001;
  }

  // p (rho) = sqrt(x*x + y*y)
  float p = sqrtf(xf * xf + yf * yf);

  // R = radius
  // float p = sqrtf(xf*xf + yf*yf);
  // c = arcsin(p/R)
  float c = asinf(p / ortho_render.r);

  float cosf_c = cosf(c);
  float sinf_c = sinf(c);
  float cosf_phi0 = cosf(ortho_render.phi0);
  float sinf_phi0 = sinf(ortho_render.phi0);
  float phi = asinf(cosf_c * sinf_phi0 + ((yf * sinf_c * cosf_phi0) / p));

  // longitude (lambda)

  // DEBUG atan2 print
  // float atan2y = xf * sinf(c);
  // float atan2x = ((p * cosf(c) * cosf(phi0)) - (yf * sinf(c) *
  // sinf(phi0))); float atan2r = atan2f(atan2y, atan2x); printf("atan2(%f,
  // %f) = %f\n", (float)atan2y, (float)atan2x, (float)atan2r); float lambda
  // = lambda0 + atan2r;
  float lambda = ortho_render.lambda0 +
                 atan2f(xf * sinf_c,
                        ((p * cosf_c * cosf_phi0) - (yf * sinf_c * sinf_phi0)));
  float noise = GetNoise(lambda, phi);

  int heightmap_color_index = GetTerrainColorIndex(noise);

  // TODO: should be transparent color
  int palette_color_index = 0;
  // TODO: Only update this if valid height_map color?
  palette_color_index = height_map[heightmap_color_index];
  if (palette_color_index == 48 || palette_color_index == 49)
    return palette_color_index;

  // TODO: Calculate phase based on solar system sun position
  // Darken colors if phase of the planet is facing the sun
  float phase_offset = kPi * -0.2f;
  // Negative: left terminator shadow goes away from camera, right
  // terminator gets closer to camera Positive: left terminator shadow
  // gets closer to the camera, right terminator goes farther away
  if ((lambda > (viewpoint_lambda0 + phase_offset + kHalfPi)) ||
      (lambda < (viewpoint_lambda0 + phase_offset - kHalfPi)))
    // This color should be darker
    palette_color_index += terrain.palette_dark_offset;

  return palette_color_index;
}

void Planet::render_orthographic(blit::Surface *framebuffer, int pixel_width,
                                 int pixel_height, float zoom, int zoom_pan_x,
                                 int zoom_pan_y) {

  // // Loop over every pixel in the draw buffer
  // for (int y = 0; y < y_size; y++) {
  // }

  // Debugging info

  // blit::debugf("min_noise: %d.%.6d\n", (int)min_noise,
  //              (int)((min_noise - (int)min_noise) * 1000000));
  // blit::debugf("max_noise: %d.%.6d\n", (int)max_noise,
  //              (int)((max_noise - (int)max_noise) * 1000000));

  // blit::debugf("color_index min, max: %d, %d\n", min_color_index,
  //              max_color_index);

  // blit::debugf("viewpoint_lambda0: %d.%.6d\n", (int)viewpoint_lambda0,
  //              (int)((viewpoint_lambda0 - (int)viewpoint_lambda0) *
  //              1000000));
  // blit::debugf("min_lambda: %d.%.6d\n", (int)min_lambda,
  //              (int)((min_lambda - (int)min_lambda) * 1000000));
  // blit::debugf("max_lambda: %d.%.6d\n", (int)max_lambda,
  //              (int)((max_lambda - (int)max_lambda) * 1000000));
  // float lambda_diff = max_lambda - min_lambda;
  // blit::debugf("lambda_diff: %d.%.6d\n", (int)lambda_diff,
  //              (int)((lambda_diff - (int)lambda_diff) * 1000000));
}
