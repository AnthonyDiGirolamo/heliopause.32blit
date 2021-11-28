#include "planet.hpp"

Planet::Planet(uint32_t seed_value, PlanetTerrain new_terrain) {
  seed = seed_value;
  terrain = new_terrain;
  Regen();
}

void Planet::SetTerrain(PlanetTerrain new_terrain) { terrain = new_terrain; }

void Planet::SetSeed(uint32_t seed_value) { seed = seed_value; }

void Planet::SetTerrainAndSeed(uint32_t seed_value, PlanetTerrain new_terrain) {
  seed = seed_value;
  terrain = new_terrain;
  Regen();
}

void Planet::Regen() {
  noise_factor_vertical = 1.0;
  radius = 80; // pixel height = 160 width = 320

  Random::SetSeed(seed);

  simplex_noise =
      SimplexNoise(terrain.noise_zoom, 1.0f, 2.0f, terrain.noise_persistance);

  terrain_heightmap_color_count = terrain.color_map.size();

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

int Planet::PixelHeight() { return radius * 2; }
int Planet::PixelWidth() { return radius * 4; }

float Planet::GetNoise(float theta, float phi) {
  // clang-format off
  float noise = simplex_noise.fractal(
      terrain.noise_octaves,
      noise_offset.x + cosf(phi) * cosf(theta),
      noise_offset.y + cosf(phi) * sinf(theta),
      noise_offset.z + sinf(phi) * noise_factor_vertical);
  // clang-format on

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
  if (index >= terrain_heightmap_color_count)
    index = terrain_heightmap_color_count - 1;

  return index;
}

void Planet::render_equirectangular(blit::Surface *framebuffer) {

  min_noise = 2;
  max_noise = -2;

  int map_width = PixelWidth();
  int map_height = PixelHeight();
  float theta_start = 0;
  float theta_end = 2.0f * blit::pi;
  float theta_increment = theta_end / map_width;
  float theta = theta_start;
  float phi = blit::pi * -0.5f;
  float phi_increment = blit::pi / map_height;

  for (int y = 0; y < map_height; y++) {
    theta = 0;

    for (int x = 0; x < map_width; x++) {
      float noise = GetNoise(theta, phi);

      // Get indexed color value
      int color_index = GetTerrainColorIndex(noise);

      framebuffer->pen = terrain.color_map[color_index];
      framebuffer->pixel(blit::Point(x, y));
      theta += theta_increment;
    }

    phi += phi_increment;
  }

  // blit::debugf("min: %d.%.6d\n", (int)min_noise,
  //              (int)((min_noise - (int)min_noise) * 1000000));
  // blit::debugf("max: %d.%.6d\n", (int)max_noise,
  //              (int)((max_noise - (int)max_noise) * 1000000));
}
