#include "sector.hpp"
#include "32blit.hpp"
#include "planet.hpp"
#include "planet_types.hpp"
#include "random.hpp"
#include <cstdint>

Sector::Sector(uint32_t seed_value) : seed(seed_value), rng(seed_value) {
  int planet_count = Random::GetRandomInteger(&rng, 10
                                              // planets.max_size()
  );

  // std::span<PlanetTerrain> AllPlanetTypes(AllPlanets);
  // for (int i = 0; i<planet_count; i++) {
  //   uint32_t planet_seed = Random::GetRandomNumber(&rng);
  //   auto planet_type = Random::GetRandomInteger(&rng, AllPlanetTypes.size());
  //   planets[i] = Planet(planet_seed, AllPlanets[planet_type]);
  // }
}

void Sector::Draw(blit::Surface *fb, blit::Vec2 pilot_position,
                  float delta_seconds) {}

void Sector::Update(blit::Vec2 pilot_position, float delta_seconds) {}
