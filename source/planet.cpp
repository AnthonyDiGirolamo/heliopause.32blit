#include "planet.hpp"
#include <cstdint>
#include <math.h>

constexpr float kPi = blit::pi;
constexpr float kHalfPi = blit::pi * 0.5f;
constexpr float kTwoPi = blit::pi * 2.0f;

Planet::Planet(uint32_t seed_value, PlanetTerrain new_terrain) {
  seed = seed_value;
  terrain = new_terrain;
  viewpoint_phi0 = 0;
  viewpoint_lambda0 = kPi;
  draw_offsetx = 0;
  draw_offsety = 0;
  Regen();
}

void Planet::SetTerrain(PlanetTerrain new_terrain) { terrain = new_terrain; }

void Planet::SetSeed(uint32_t seed_value) { seed = seed_value; }

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
    } else if (i >= terrain.color_padding_start + terrain.color_map.size()) {
      // Use the last color
      height_map[i] = terrain.color_map[terrain.color_map.size() - 1];
    } else {
      height_map[i] = terrain.color_map[i - terrain.color_padding_start];
    }
  }
}

void Planet::Regen() {
  RebuildHeightMap();

  noise_factor_vertical = 1.0;

  Random::SetSeed(seed);

  simplex_noise =
      SimplexNoise(terrain.noise_zoom, 1.0f, 2.0f, terrain.noise_persistance);

  noise_offset.x = Random::GetRandomFloat(1024);
  noise_offset.y = Random::GetRandomFloat(1024);
  noise_offset.z = Random::GetRandomFloat(1024);

  if (terrain.max_noise_stretch - terrain.min_noise_stretch > 0) {
    noise_factor_vertical = Random::GetRandomFloat(terrain.min_noise_stretch,
                                                   terrain.max_noise_stretch);
  } else {
    float unused_result = Random::GetRandomFloat(0, 20);
  }

  // blit::debugf(
  //     "noise_factor_vertical: %d.%.6d\n", (int)noise_factor_vertical,
  //     (int)((noise_factor_vertical - (int)noise_factor_vertical) * 1000000));
}

float Planet::GetNoise(float theta, float phi) {
  // clang-format off
  float noise = simplex_noise.fractal(
      terrain.noise_octaves,
      noise_offset.x + cosf(phi) * cosf(theta),
      noise_offset.y + cosf(phi) * sinf(theta),
      noise_offset.z + sinf(phi) * noise_factor_vertical);
  // clang-format on

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

void Planet::SetDrawOffset(int x, int y) {
  draw_offsetx = x;
  draw_offsety = y;
}

void Planet::render_equirectangular(blit::Surface *framebuffer, int map_width,
                                    int map_height) {
  // Erase to non-existent color palette index
  framebuffer->pen = 255;
  // Clear draw area
  framebuffer->rectangle(
      blit::Rect(draw_offsetx, draw_offsety, map_width, map_height));

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
      int dx = draw_offsetx + x;
      int dy = draw_offsety + y;

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

void Planet::render_orthographic(blit::Surface *framebuffer, int map_size) {
  // https://en.wikipedia.org/wiki/Orthographic_map_projection

  // Reset min/max tracking
  min_color_index = 255;
  max_color_index = 0;
  min_noise = 2;
  max_noise = -2;
  min_lambda = 1000;
  max_lambda = -1000;

  // floor(map_size / 2)
  int pixel_radius = (int)((float)map_size * 0.5f);
  // float r = (int)((float)map_size * 0.5f);
  float r = pixel_radius;

  // phi0 = origin latitude
  float phi0 = viewpoint_phi0;
  // lambda0 = origin longitude
  float lambda0 = viewpoint_lambda0;
  float centerx = pixel_radius;
  float centery = pixel_radius;

  // Erase to non-existent color palette index
  framebuffer->pen = 255;
  // Clear draw area
  framebuffer->rectangle(
      blit::Rect(draw_offsetx, draw_offsety, map_size, map_size));

  // // Debug box outline
  // framebuffer->pen = 7;
  // // Rectangle defined by top-left corner to bottom-right corner
  // Draw::rectangle(framebuffer, draw_offsetx + 0, draw_offsety + 0, map_size,
  //                 map_size);

  // Plot a circle we wish to fill with the planet image
  framebuffer->pen = 254;
  Draw::circle(framebuffer, draw_offsetx + centerx, draw_offsety + centery,
               pixel_radius - 1, true);

  for (int y = 0; y < map_size; y++) {
    for (int x = 0; x < map_size; x++) {
      // Get the current pixel value.
      uint8_t pixel_value =
          *framebuffer->ptr(draw_offsetx + x, draw_offsety + y);

      // If pixel is 254, we are inside the circle.
      if (pixel_value == 254) {

        float xf = (float)x - centerx;
        float yf = (float)y - centery;
        // Some trig function breaks if xf and xy are both zero. Set a slight
        // offset.
        if ((y - centery == 0) && (x - centerx == 0)) {
          xf = 0.0001;
          yf = 0.0001;
        }
        // p (rho) = sqrt(x*x + y*y)
        float p = sqrtf(xf * xf + yf * yf);

        // R = radius
        // float p = sqrtf(xf*xf + yf*yf);
        // c = arcsin(p/R)
        float c = asinf(p / r);

        float phi =
            asinf(cosf(c) * sinf(phi0) + ((yf * sinf(c) * cosf(phi0)) / p));

        // longitude (lambda)
        float lambda =
            lambda0 + atan2f(xf * sinf(c), ((p * cosf(c) * cosf(phi0)) -
                                            (yf * sinf(c) * sinf(phi0))));
        float noise = GetNoise(lambda, phi);

        int heightmap_color_index = GetTerrainColorIndex(noise);
        // TODO: should be transparent color
        int palette_color_index = 0;
        // TODO: Only update this if valid height_map color?
        palette_color_index = height_map[heightmap_color_index];

        // TODO: Calculate phase based on solar system sun position
        // Darken colors if phase of the planet is facing the sun
        float phase_offset = kPi * -0.2f;
        // Negative: left terminator shadow goes away from camera, right
        // terminator gets closer to camera Positive: left terminator shadow
        // gets closer to the camera, right terminator goes farther away
        if ((lambda > (viewpoint_lambda0 + phase_offset + kHalfPi)) ||
            (lambda < (viewpoint_lambda0 + phase_offset - kHalfPi)))
          // This color should be darker
          palette_color_index += 16;

        // Set color and draw the pixel
        framebuffer->pen = palette_color_index;
        int dx = draw_offsetx + x;
        int dy = draw_offsety + y;
        framebuffer->pixel(blit::Point(dx, dy));
      }

      // If out of bounds
    }
  }

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
