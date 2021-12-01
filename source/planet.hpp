#pragma once

#include "32blit.hpp"
#include "graphics/color.hpp"
#include "types/vec3.hpp"

#include "SimplexNoise.h"
#include "random.hpp"

#include "math.h"
#include <cstdint>
#include <span>
#include <string>

enum PlanetTerrainType {
  tundra,
  desert,
  barren,
  lava,
  terran,
  island,
  gas_giant_red,
  gas_giant_blue,
  gas_giant_yellow,
  gas_giant_rainbow,
};

struct PlanetTerrain {
  PlanetTerrainType type;
  std::string_view type_string;
  int noise_octaves;
  float noise_zoom;
  float noise_persistance;
  uint8_t map_icon_color;
  uint8_t full_shadow;
  uint8_t transparent_color;
  float min_noise_stretch;
  float max_noise_stretch;
  int min_size;
  std::span<const uint8_t> color_map;
  uint8_t color_padding_start;
  uint8_t color_padding_end;
};

class Planet {
public:
  uint32_t seed;
  int radius;
  blit::Vec3 noise_offset;
  float noise_factor_vertical;
  float viewpoint_phi0;
  float viewpoint_lambda0;
  PlanetTerrain terrain;
  SimplexNoise simplex_noise;
  uint8_t height_map[32];

  Planet(uint32_t seed_value, PlanetTerrain terrain);
  void RebuildHeightMap();
  void SetTerrainAndSeed(uint32_t seed_value, PlanetTerrain new_terrain);
  void SetRadius(int new_radius);
  void SetSeed(uint32_t seed_value);
  void SetTerrain(PlanetTerrain new_terrain);
  void Regen();
  int PixelHeight();
  int PixelWidth();
  void AdjustViewpointLatitude(float amount);
  void AdjustViewpointLongitude(float amount);

  float GetNoise(float theta, float phi);
  int GetTerrainColorIndex(float noise);

  void render_equirectangular(blit::Surface *framebuffer);
  void render_orthographic(blit::Surface *framebuffer);

private:
  int min_color_index;
  int max_color_index;
  float max_noise;
  float min_noise;
  float min_lambda;
  float max_lambda;
  int terrain_heightmap_color_count;
};
