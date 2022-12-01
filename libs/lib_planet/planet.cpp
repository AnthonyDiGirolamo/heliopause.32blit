#include "planet.hpp"
#include <cstdint>
#include <math.h>

constexpr float kPi = blit::pi;
constexpr float kHalfPi = blit::pi * 0.5f;
constexpr float kTwoPi = blit::pi * 2.0f;
constexpr float kFourPi = blit::pi * 4.0f;

// float fast_atan2f(float x) {
//   return kFourPi*x - x*(fabsf(x) - 1)*(0.2447 + 0.0663*fabsf(x));
// }

Planet::Planet(uint32_t seed_value, PlanetTerrain new_terrain) {
  seed = seed_value;
  terrain = new_terrain;
  viewpoint_phi0 = 0;
  viewpoint_lambda0 = kPi;
  draw_offsetx = 0;
  draw_offsety = 0;
  Regen();
  // ortho_render = PlanetOrthographicRenderValues();
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
    unused_result += 1;
  }

  // blit::debugf(
  //     "noise_factor_vertical: %d.%.6d\n", (int)noise_factor_vertical,
  //     (int)((noise_factor_vertical - (int)noise_factor_vertical) * 1000000));
}

float Planet::GetNoise(float theta, float phi) {
  // clang-format off
  float cosf_phi = cosf(phi);
  float sinf_phi = sinf(phi);
  float cosf_theta = cosf(theta);
  float sinf_theta = sinf(theta);

  float noise = simplex_noise.fractal(
      terrain.noise_octaves,
      noise_offset.x + cosf_phi * cosf_theta,
      noise_offset.y + cosf_phi * sinf_theta,
      noise_offset.z + sinf_phi * noise_factor_vertical);
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

void Planet::SetDrawOffset(int x, int y) {
  draw_offsetx = x;
  draw_offsety = y;
}

void Planet::render_equirectangular(blit::Surface *framebuffer, int map_width,
                                    int map_height) {
  // Erase to non-existent color palette index
  framebuffer->pen = 49;
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

void Planet::setup_render_orthographic(blit::Surface *framebuffer, int x_size,
                                       int y_size, float zoom, int zoom_pan_x,
                                       int zoom_pan_y, uint32_t start_time) {
  // Erase to non-existent color palette index
  framebuffer->pen = 49;
  // Clear draw area
  framebuffer->rectangle(
      blit::Rect(draw_offsetx, draw_offsety, x_size, y_size));

  // https://en.wikipedia.org/wiki/Orthographic_map_projection
  int map_size = 0;
  if (y_size <= x_size)
    map_size = y_size;
  else
    map_size = x_size;

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
  if (zoom > 0)
    pixel_radius = (int)((float)pixel_radius * zoom);

  float r = pixel_radius;

  // phi0 = origin latitude
  float phi0 = viewpoint_phi0;
  // lambda0 = origin longitude
  float lambda0 = viewpoint_lambda0;
  float centerx = pixel_radius;
  float centery = pixel_radius;

  // Clear draw area
  // framebuffer->rectangle(
  //     blit::Rect(draw_offsetx, draw_offsety, map_size, map_size));

  // // Debug box outline
  // framebuffer->pen = 7;
  // // Rectangle defined by top-left corner to bottom-right corner
  // Draw::rectangle(framebuffer, draw_offsetx + 0, draw_offsety + 0, map_size,
  //                 map_size);

  // Calculate initial zoom offset
  int zoom_offset_x = 0;
  int zoom_offset_y = 0;
  if (2 * pixel_radius > x_size) {
    zoom_offset_x -= (int)((float)(2 * pixel_radius - x_size) * 0.5f);
  }
  if (2 * pixel_radius > y_size) {
    zoom_offset_y -= (int)((float)(2 * pixel_radius - y_size) * 0.5f);
  }

  // Pan the camera
  zoom_offset_x -= zoom_pan_x;
  zoom_offset_y -= zoom_pan_y;

  // Plot a circle we wish to fill with the planet image
  framebuffer->pen = 0;
  Draw::circle(framebuffer, draw_offsetx + zoom_offset_x + centerx,
               draw_offsety + zoom_offset_y + centery, pixel_radius - 1, true);

  ortho_render.pixel_radius = pixel_radius;
  ortho_render.r = r;
  ortho_render.phi0 = phi0;
  ortho_render.lambda0 = lambda0;
  ortho_render.zoom_offset_x = zoom_offset_x;
  ortho_render.zoom_offset_y = zoom_offset_y;
  ortho_render.x_size = x_size;
  ortho_render.y_size = y_size;
  ortho_render.current_y = -1;
  ortho_render.done = false;
  ortho_render.start_time = start_time;
  ortho_render.framebuffer = framebuffer;
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
  if (ortho_render.current_y < ortho_render.y_size) {
    ortho_render.current_y++;
  } else {
    ortho_render.done = true;
  }
  if (ortho_render.done)
    return;

  int y_coord = ortho_render.current_y;
  for (int x = 0; x < ortho_render.x_size; x++) {
    // Get the current pixel value.
    uint8_t pixel_value = *ortho_render.framebuffer->ptr(x, y_coord);

    // If pixel isn't transparent, we are inside the circle.
    if (pixel_value != 49) {
      // Get x,y coords for map projection.
      // xf, yf = 0, 0 should be at the center of the globe
      float xf = (float)x - ortho_render.r - ortho_render.zoom_offset_x;
      float yf = (float)y_coord - ortho_render.r - ortho_render.zoom_offset_y;

      // Some trig function breaks if xf and xy are both zero. Set a slight
      // offset.
      if ((y_coord - ortho_render.pixel_radius - ortho_render.zoom_offset_y ==
           0) &&
          (x - ortho_render.pixel_radius - ortho_render.zoom_offset_x == 0)) {
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
                     atan2f(xf * sinf_c, ((p * cosf_c * cosf_phi0) -
                                          (yf * sinf_c * sinf_phi0)));
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
        palette_color_index += terrain.palette_dark_offset;

      // Set color and draw the pixel
      ortho_render.framebuffer->pen = palette_color_index;
      ortho_render.framebuffer->pixel(
          blit::Point(draw_offsety + x, draw_offsety + y_coord));
    }
    // If out of bounds
  }
}

void Planet::render_orthographic(blit::Surface *framebuffer, int x_size,
                                 int y_size, float zoom, int zoom_pan_x,
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
