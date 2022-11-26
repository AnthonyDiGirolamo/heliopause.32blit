#include "planet.hpp"
#include "pw_string/string_builder.h"
#include <cstdint>
#include <math.h>


constexpr float kPi = blit::pi;
constexpr float kHalfPi = blit::pi * 0.5f;
constexpr float kTwoPi = blit::pi * 2.0f;
constexpr float kFourPi = blit::pi * 4.0f;

// >>> [int(65535*math.sin(math.pi * 2 * i / 1024)) for i in range(257)]
static const uint16_t sintab[257] = {
  0, 402, 804, 1206, 1608, 2010, 2412, 2813, 3215, 3617, 4018, 4419, 4821, 5221, 5622, 6023, 6423, 6823, 7223, 7622, 8022, 8421, 8819, 9218, 9615, 10013, 10410, 10807, 11203, 11599, 11995, 12390, 12785, 13179, 13573, 13966, 14358, 14750, 15142, 15533, 15923, 16313, 16702, 17091, 17479, 17866, 18252, 18638, 19023, 19408, 19791, 20174, 20557, 20938, 21319, 21699, 22078, 22456, 22833, 23210, 23585, 23960, 24334, 24707, 25079, 25450, 25820, 26189, 26557, 26924, 27290, 27655, 28019, 28382, 28744, 29105, 29465, 29823, 30181, 30537, 30892, 31247, 31599, 31951, 32302, 32651, 32999, 33346, 33691, 34035, 34378, 34720, 35061, 35400, 35737, 36074, 36409, 36742, 37075, 37406, 37735, 38063, 38390, 38715, 39039, 39361, 39682, 40001, 40319, 40635, 40950, 41263, 41574, 41885, 42193, 42500, 42805, 43109, 43411, 43711, 44010, 44307, 44603, 44896, 45189, 45479, 45768, 46055, 46340, 46623, 46905, 47185, 47463, 47739, 48014, 48287, 48558, 48827, 49094, 49360, 49623, 49885, 50145, 50403, 50659, 50913, 51165, 51415, 51664, 51910, 52155, 52397, 52638, 52876, 53113, 53347, 53580, 53810, 54039, 54265, 54490, 54712, 54933, 55151, 55367, 55581, 55793, 56003, 56211, 56416, 56620, 56821, 57021, 57218, 57413, 57606, 57796, 57985, 58171, 58355, 58537, 58717, 58894, 59069, 59242, 59413, 59582, 59748, 59912, 60074, 60234, 60391, 60546, 60699, 60849, 60997, 61143, 61287, 61428, 61567, 61704, 61838, 61970, 62100, 62227, 62352, 62474, 62595, 62713, 62828, 62941, 63052, 63161, 63267, 63370, 63472, 63570, 63667, 63761, 63853, 63942, 64029, 64114, 64196, 64275, 64353, 64427, 64500, 64570, 64637, 64702, 64765, 64825, 64883, 64938, 64991, 65042, 65090, 65135, 65178, 65219, 65257, 65293, 65326, 65357, 65385, 65411, 65435, 65456, 65474, 65490, 65504, 65515, 65523, 65530, 65533, 65535
};

int16_t rsin(int16_t r, uint16_t th) {
  th >>= 6; // angle 0-1023
  // return int(r * sin((2 * M_PI) * th / 1024.));
  int th4 = th & 511;
  if (th4 & 256)
    th4 = 512 - th4; // 256->256 257->255, etc
  uint16_t s = sintab[th4];
  int16_t p = ((uint32_t)s * r) >> 16;
  if (th & 512)
    p = -p;
  return p;
}

int16_t rcos(int16_t r, uint16_t th) {
  return rsin(r, th + 0x4000);
}

void polar(int &x, int &y, int16_t r, uint16_t th) {
  x = (int)(-rsin(r, th));
  y = (int)(rcos(r, th));
}

// >>> [int(round(1024 * math.atan(i / 256.) / math.pi)) for i in range(256)]
static const uint8_t atan8[] = {
  0,1,3,4,5,6,8,9,10,11,13,14,15,17,18,19,20,22,23,24,25,27,28,29,30,32,33,34,36,37,38,39,41,42,43,44,46,47,48,49,51,52,53,54,55,57,58,59,60,62,63,64,65,67,68,69,70,71,73,74,75,76,77,79,80,81,82,83,85,86,87,88,89,91,92,93,94,95,96,98,99,100,101,102,103,104,106,107,108,109,110,111,112,114,115,116,117,118,119,120,121,122,124,125,126,127,128,129,130,131,132,133,134,135,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,177,178,179,180,181,182,183,184,185,186,187,188,188,189,190,191,192,193,194,195,195,196,197,198,199,200,201,201,202,203,204,205,206,206,207,208,209,210,211,211,212,213,214,215,215,216,217,218,219,219,220,221,222,222,223,224,225,225,226,227,228,228,229,230,231,231,232,233,234,234,235,236,236,237,238,239,239,240,241,241,242,243,243,244,245,245,246,247,248,248,249,250,250,251,251,252,253,253,254,255,255
};

uint16_t atan2(int16_t y, int16_t x) {
  uint16_t a;
  uint16_t xx = 0;

  /* These values are tricky. So pretend they are not */
  if (x == -32768)
    x++;
  if (y == -32768)
    y++;

  if ((x <= 0) ^ (y > 0)) {
    int16_t t; t = x; x = y; y = t;
    xx ^= 0x4000;
  }
  if (x <= 0) {
    x = -x;
  } else {
    xx ^= 0x8000;
  }
  y = abs(y);
  if (x > y) {
    int16_t t; t = x; x = y; y = t;
    xx ^= 0x3fff;
  }
  while ((x | y) & 0xff80) {
    x >>= 1;
    y >>= 1;
  }
  if (y == 0) {
    a = 0;
  } else if (x == y) {
    a = 0x2000;
  } else {
    // assert(x <= y);
    int r = ((x << 8) / y);
    // assert(0 <= r);
    // assert(r < 256);
    a = atan8[r] << 5;
  }
  a ^= xx;
  return a;
}

std::unordered_map<int16_t, int16_t> sin_cache;
std::unordered_map<int16_t, int16_t> cos_cache;
int cache_hit = 0;
int cache_misses = 0;

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

// float min_phi = 999999;
// float max_phi = 0;
// float min_theta = 999999;
// float max_theta = 0;

float Planet::GetNoise(float theta, float phi) {
  // clang-format off
  // float cos_phi = cosf(phi);
  // float sin_phi = sinf(phi);
  // float cos_theta = cosf(theta);
  // float sin_theta = sinf(theta);

  // if (phi > max_phi)
  //   max_phi = phi;
  // if (phi < min_phi)
  //   min_phi = phi;
  // if (theta > max_theta)
  //   max_theta = theta;
  // if (theta < min_theta)
  //   min_theta = theta;
  // printf("   phi: (min max %f, %f) -- theta: (min max %f, %f)\n", min_phi, max_phi, min_theta, max_theta);

  // printf("FLO phi: %f (cos sin %f, %f) -- theta: %f (cos sin %f, %f)\n", phi, cos_phi, sin_phi, theta, cos_theta, sin_theta);

  int16_t iphi = ((65536L * (phi)) / kTwoPi);
  int16_t itheta = ((65536L * (theta)) / kTwoPi);
  int16_t scale = 15'000;
  float scalef = 15'000;

  int16_t prev_sin_phi = sin_cache[iphi];
  int16_t prev_sin_theta = sin_cache[itheta];
  int16_t prev_cos_phi = cos_cache[iphi];
  int16_t prev_cos_theta = cos_cache[itheta];

  int16_t icos_phi, isin_phi, icos_theta, isin_theta;

  if (prev_sin_phi == 0){
    isin_phi = rsin(scale, iphi);
    sin_cache[iphi] = isin_phi;
    cache_misses++;
  }
  else {
    isin_phi = prev_sin_phi;
    cache_hit++;
  }

  if (prev_cos_phi == 0) {
    icos_phi = rcos(scale, iphi);
    cos_cache[iphi] = icos_phi;
    cache_misses++;
  }
  else {
    icos_phi = prev_cos_phi;
    cache_hit++;
  }

  if (prev_sin_theta == 0) {
    isin_theta = rsin(scale, itheta);
    sin_cache[itheta] = isin_theta;
    cache_misses++;
  }
  else {
    isin_theta = prev_sin_theta;
    cache_hit++;
  }

  if (prev_cos_theta == 0) {
    icos_theta = rcos(scale, itheta);
    cos_cache[itheta] = icos_theta;
    cache_misses++;
  }
  else {
    icos_theta = prev_cos_theta;
    cache_hit++;
  }

  float fcos_phi = (float)icos_phi / scalef;
  float fsin_phi = (float)isin_phi / scalef;
  float fcos_theta = (float)icos_theta / scalef;
  float fsin_theta = (float)isin_theta / scalef;

  // printf("INT phi: %d (cos sin %d, %d = %f, %f) -- theta: %d (cos sin %d, %d = %f, %f)\n",
  //        iphi, icos_phi, isin_phi, fcos_phi, fsin_phi,
  //        itheta, icos_theta, isin_theta, fcos_theta, fsin_theta);

  // printf("DIFF phi: (cos sin %f, %f) -- theta: (cos sin %f, %f)\n",
  //        cos_phi - fcos_phi,
  //        sin_phi - fsin_phi,
  //        cos_theta - fcos_theta,
  //        sin_theta - fsin_theta);
  return -.25;

  // float noise = simplex_noise.fractal(
  //     terrain.noise_octaves,
  //     noise_offset.x + fcos_phi * fcos_theta,
  //     noise_offset.y + fcos_phi * fsin_theta,
  //     noise_offset.z + fsin_phi * noise_factor_vertical);
  // // clang-format on

  // float altitude_modifier = (phi * phi) * terrain.latitude_bias;
  // noise = noise + altitude_modifier;

  // if (theta > max_lambda)
  //   max_lambda = theta;
  // if (theta < min_lambda)
  //   min_lambda = theta;
  // if (noise > max_noise)
  //   max_noise = noise;
  // if (noise < min_noise)
  //   min_noise = noise;

  // if (noise > 1)
  //   noise = 1;
  // if (noise < -1)
  //   noise = -1;

  // return noise;

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

  // if (index > max_color_index)
  //   max_color_index = index;
  // if (index < min_color_index)
  //   min_color_index = index;

  return index;
}

void Planet::SetDrawOffset(int x, int y) {
  draw_offsetx = x;
  draw_offsety = y;
}

void Planet::render_equirectangular(blit::Surface *framebuffer, int map_width,
                                    int map_height) {

  // min_phi = 999999;
  // max_phi = 0;
  // min_theta = 999999;
  // max_theta = 0;

  // Erase to non-existent color palette index
  framebuffer->pen = 255;
  // Clear draw area
  framebuffer->rectangle(
      blit::Rect(draw_offsetx, draw_offsety, map_width, map_height));

  // Reset min/max tracking
  // min_color_index = 255;
  // max_color_index = 0;
  min_noise = 2;
  max_noise = -2;
  // min_lambda = 1000;
  // max_lambda = -1000;

  // Theta value (longitude)
  // We will map all the way around the sphere [0, 2pi]
  float theta_start = 0;
  float theta_end = 2.0f * kPi;
  float theta_increment = theta_end / map_width;
  float theta = theta_start;

  // Phi value (lattitude)
  // North to south pole [-pi/2, pi/2]
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

void Planet::render_orthographic(blit::Surface *framebuffer, int x_size,
                                 int y_size, float zoom, int zoom_pan_x,
                                 int zoom_pan_y) {

  sin_cache.clear();
  cos_cache.clear();

  cache_hit = 0;
  cache_misses = 0;
  // min_phi = 999999;
  // max_phi = 0;
  // min_theta = 999999;
  // max_theta = 0;

  // Erase to non-existent color palette index
  framebuffer->pen = 255;
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
  // min_color_index = 255;
  // max_color_index = 0;
  min_noise = 2;
  max_noise = -2;
  // min_lambda = 1000;
  // max_lambda = -1000;

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

  // Erase to non-existent color palette index
  framebuffer->pen = 255;
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
  framebuffer->pen = 254;
  Draw::circle(framebuffer, draw_offsetx + zoom_offset_x + centerx,
               draw_offsety + zoom_offset_y + centery, pixel_radius - 1, true);

  // Loop over every pixel in the draw buffer
  for (int y = 0; y < y_size; y++) {
    sin_cache.clear();
    cos_cache.clear();
    for (int x = 0; x < x_size; x++) {
      // Get the current pixel value.
      uint8_t pixel_value = *framebuffer->ptr(x, y);

      // If pixel is 254, we are inside the circle.
      if (pixel_value == 254) {
        // Get x,y coords for map projection.
        // xf, yf = 0, 0 should be at the center of the globe
        float xf = (float)x - r - zoom_offset_x;
        float yf = (float)y - r - zoom_offset_y;

        // Some trig function breaks if xf and xy are both zero. Set a slight
        // offset.
        if ((y - pixel_radius - zoom_offset_y == 0) &&
            (x - pixel_radius - zoom_offset_x == 0)) {
          xf = 0.0001;
          yf = 0.0001;
        }

        // p (rho) = sqrt(x*x + y*y)
        float p = sqrtf(xf * xf + yf * yf);

        // R = radius
        // float p = sqrtf(xf*xf + yf*yf);
        // c = arcsin(p/R)
        float c = asinf(p / r);

        // COMPUTE:
        // cosf(c)
        // sinf(c)
        // cosf(phi0)
        // sinf(phi0)

        int16_t iphi0 = ((65536L * (phi0)) / kTwoPi);
        int16_t ic = ((65536L * (c)) / kTwoPi);
        int16_t scale = 15'000;
        float scalef = 15'000;

        int16_t prev_sin_phi0 = sin_cache[iphi0];
        int16_t prev_sin_c = sin_cache[ic];
        int16_t prev_cos_phi0 = cos_cache[iphi0];
        int16_t prev_cos_c = cos_cache[ic];

        int16_t icos_phi0, isin_phi0, icos_c, isin_c;

        if (prev_sin_phi0 == 0){
          isin_phi0 = rsin(scale, iphi0);
          sin_cache[iphi0] = isin_phi0;
          cache_misses++;
        }
        else {
          isin_phi0 = prev_sin_phi0;
          cache_hit++;
        }

        if (prev_cos_phi0 == 0) {
          icos_phi0 = rcos(scale, iphi0);
          cos_cache[iphi0] = icos_phi0;
          cache_misses++;
        }
        else {
          icos_phi0 = prev_cos_phi0;
          cache_hit++;
        }

        if (prev_sin_c == 0) {
          isin_c = rsin(scale, ic);
          sin_cache[ic] = isin_c;
          cache_misses++;
        }
        else {
          isin_c = prev_sin_c;
          cache_hit++;
        }

        if (prev_cos_c == 0) {
          icos_c = rcos(scale, ic);
          cos_cache[ic] = icos_c;
          cache_misses++;
        }
        else {
          icos_c = prev_cos_c;
          cache_hit++;
        }

        float fcos_phi0 = (float)icos_phi0 / scalef;
        float fsin_phi0 = (float)isin_phi0 / scalef;
        float fcos_c = (float)icos_c / scalef;
        float fsin_c = (float)isin_c / scalef;

        float phi =
            asinf(fcos_c * fsin_phi0 + ((yf * fsin_c * fcos_phi0) / p));
        // float phi =
        //     asinf(cosf(c) * sinf(phi0) + ((yf * sinf(c) * cosf(phi0)) / p));

        // longitude (lambda)

        // DEBUG atan2 print
        float atan2y = xf * fsin_c;
        // float atan2y = xf * sinf(c);
        float atan2x = ((p * fcos_c * fcos_phi0) - (yf * fsin_c * fsin_phi0));
        // float atan2x = ((p * cosf(c) * cosf(phi0)) - (yf * sinf(c) * sinf(phi0)));
        float atan2r = atan2f(atan2y, atan2x);
        // printf("atan2(%f, %f) = %f\n", (float)atan2y, (float)atan2x, (float)atan2r);
        float lambda = lambda0 + atan2r;

        // float lambda =
        //     lambda0 + atan2f(xf * sinf(c), ((p * cosf(c) * cosf(phi0)) -
        //                                     (yf * sinf(c) * sinf(phi0))));

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
        framebuffer->pen = palette_color_index;
        framebuffer->pixel(blit::Point(draw_offsety + x, draw_offsety + y));
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
  // printf("Cache Hits: %d  Misses: %d\n", cache_hit, cache_misses);
}
