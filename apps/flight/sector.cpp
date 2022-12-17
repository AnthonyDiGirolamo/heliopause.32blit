#include "sector.hpp"

#include "32blit.hpp"
#include <cstdint>
#include <forward_list>

#include "planet.hpp"
#include "planet_types.hpp"
#include "random.hpp"
#include "starfield.hpp"

namespace {

constexpr int kMinPlanetCount = 1;
constexpr int kMaxPlanetCount = 14;

} // namespace

Sector::Sector(uint32_t seed_value) : seed(seed_value), rng(seed_value) {
  planets.clear();

  int planet_count =
      Random::GetRandomInteger(&rng, kMinPlanetCount, kMaxPlanetCount);

  // TODO: Random stars
  int star_radius = Random::GetRandomInteger(&rng, 64, 192);
  int star_class = Random::GetRandomInteger(&rng, 5);
  // Star Colors
  // Primary      Outline
  // 6  lightgray 7  white
  // 14 pink      8  red
  // 10 yellow    9  orange
  // 9  orange    10 yellow
  // 13 indigo    12 blue

  printf("\nSector: seed=%u  planets=%d  star_radius=%d  star_class=%d\n",
         seed_value, planet_count, star_radius, star_class);

  int total_npcs = 0;

  for (int i = 0; i < planet_count; i++) {
    uint32_t planet_seed = Random::GetRandomNumber(&rng);
    int planet_type = Random::GetRandomInteger(
        &rng, AllPlanetTypes.size() -
                  1); // TODO: Fix the -1 here for kAtmosphereTerranPlanet
    int npc_count = Random::GetRandomInteger(&rng, 4);
    total_npcs += npc_count;
    printf("Planet %d: seed=%u  type=%d  npcs=%d  ", i, planet_seed,
           planet_type, npc_count);

    float angle_around_star = Random::GetRandomFloat(&rng, blit::kTwoPi);
    float distance_from_star = Random::GetRandomFloat(&rng, 2048);
    printf("Position=[%.2f, %.2f]\n",
           static_cast<double>(angle_around_star),
           static_cast<double>(distance_from_star));
  }

  int pirate_count = Random::GetRandomInteger(&rng, 1, 3);
  printf("NPCs=%d  Pirates=%d\n", total_npcs, pirate_count);
}

void Sector::Draw(blit::Surface *fb, blit::Vec2 pilot_position,
                  float delta_seconds) {}

void Sector::Update(blit::Vec2 pilot_position, float delta_seconds) {}
