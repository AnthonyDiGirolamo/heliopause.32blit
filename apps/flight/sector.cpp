#include "sector.hpp"

#include "32blit.hpp"
#include <cstdint>
#include <forward_list>

#include "colors.hpp"
#include "planet.hpp"
#include "planet_types.hpp"
#include "random.hpp"
#include "starfield.hpp"

namespace {

constexpr int kMinPlanetCount = 1;
constexpr int kMaxPlanetCount = 14;

} // namespace

SectorPlanet::SectorPlanet()
    : planet(), sector_position(0.0f, 0.0f), screen_position(0.0f, 0.0f) {}

SectorPlanet::SectorPlanet(uint32_t seed, PlanetTerrain terrain,
                           blit::Vec2 position)
    : planet(seed, terrain), sector_position(position.x, position.y),
      screen_position(0.0f, 0.0f) {}

void SectorPlanet::UpdatePosition(blit::Vec2 pilot_position,
                                  blit::Vec2 screen_center) {

  screen_position = screen_center - (pilot_position - sector_position);
}

Sector::Sector(uint32_t seed_value) : seed(seed_value), rng(seed_value) {
  planets.clear();

  int planet_count =
      Random::GetRandomInteger(&rng, kMinPlanetCount, kMaxPlanetCount);

  // TODO: Star drawing
  int star_radius = Random::GetRandomInteger(&rng, 64, 192);
  int star_class = Random::GetRandomInteger(&rng, 5);

  // Star Colors
  // Primary      Outline
  // ------------ ---------
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

    // TODO: Fix the -1 here for kAtmosphereTerranPlanet
    int planet_type = Random::GetRandomInteger(&rng, AllPlanetTypes.size() - 1);
    int min_radius = AllPlanetTypes[planet_type].min_size;
    int planet_radius =
        Random::GetRandomInteger(&rng, min_radius, MAX_PLANET_RADIUS);

    int npc_count = Random::GetRandomInteger(&rng, 4);
    total_npcs += npc_count;
    printf("Planet %d: seed=%u  type=%d  radius=%d  npcs=%d  ", i, planet_seed,
           planet_type, planet_radius, npc_count);

    float distance_from_star = Random::GetRandomFloat(&rng, 2048);
    float angle_around_star = Random::GetRandomFloat(&rng, blit::kTwoPi);

    blit::Vec2 planet_position =
        blit::Vec2(distance_from_star * cosf(angle_around_star),
                   distance_from_star * sinf(angle_around_star));

    // Squish vertically to make an elipse
    planet_position *= blit::Vec2(1.0f, 35.264f / 90.0f);

    printf("Position: Dist=%.2f Angle=%.2f [%.2f, %.2f]\n",
           static_cast<double>(distance_from_star),
           static_cast<double>(angle_around_star),
           static_cast<double>(planet_position.x),
           static_cast<double>(planet_position.y));

    planets.push_front(SectorPlanet(planet_seed, AllPlanetTypes[planet_type],
                                    planet_position));
  }

  int pirate_count = Random::GetRandomInteger(&rng, 1, 3);

  printf("NPCs=%d  Pirates=%d\n", total_npcs, pirate_count);
}

void Sector::SetScreenCenter(blit::Vec2 center) { screen_center = center; }

void Sector::Draw(blit::Surface *fb) {
  for (auto &&sector_planet : planets) {
    fb->pen = PICO8_WHITE;
    fb->rectangle(
        blit::Rect(sector_planet.screen_position, blit::Size(20, 20)));
  }
}

void Sector::Update(blit::Vec2 pilot_position) {
  for (auto &&sector_planet : planets) {
    sector_planet.UpdatePosition(pilot_position, screen_center);
  }
}
