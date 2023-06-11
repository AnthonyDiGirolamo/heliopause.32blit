#pragma once

#include "32blit.hpp"
#include <cstdint>
#include <forward_list>

#include "planet.hpp"
#include "pw_random/random.h"
#include "pw_random/xor_shift.h"

class SectorPlanet {
public:
  SectorPlanet();
  SectorPlanet(uint32_t seed, PlanetTerrain terrain, int radius,
               blit::Vec2 position);

  Planet planet;
  int planet_radius;
  blit::Vec2 sector_position;
  blit::Vec2 screen_position;

  void UpdatePosition(blit::Vec2 pilot_position, blit::Vec2 screen_center);
};

class Sector {
public:
  Sector(uint32_t initial_seed);

  uint32_t seed;
  pw::random::XorShiftStarRng64 rng;
  std::forward_list<SectorPlanet> planets;
  blit::Vec2 screen_center;

  void SetScreenCenter(blit::Vec2 center);
  void RenderPlanets(blit::Surface *fb);
  void Draw(blit::Surface *fb);
  void Update(blit::Vec2 pilot_position);
};
