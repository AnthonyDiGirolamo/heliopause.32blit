#include "sector.hpp"

#include "32blit.hpp"
#include <cstdint>
#include <forward_list>

#include "colors.hpp"
#include "planet.hpp"
#include "planet_types.hpp"
#include "random.hpp"
#include "starfield.hpp"
#include "trig.hpp"
#include "types/rect.hpp"
#include "types/vec2.hpp"

using namespace lib_trig;

namespace {

constexpr int kMinPlanetCount = 1;
constexpr int kMaxPlanetCount = 14;
constexpr int kMaxPlanetRadius = 64;

} // namespace

SectorPlanet::SectorPlanet()
    : planet(), sector_position(0.0f, 0.0f), screen_position(0.0f, 0.0f),
      distance_to_pilot(-1.0f) {}

SectorPlanet::SectorPlanet(uint32_t seed, PlanetTerrain terrain, int radius,
                           blit::Vec2 position)
    : planet(seed, terrain), planet_radius(radius),
      sector_position(position.x, position.y), screen_position(0.0f, 0.0f),
      distance_to_pilot(-1.0f) {}

void SectorPlanet::UpdatePosition(blit::Vec2 pilot_position,
                                  blit::Vec2 screen_center) {

  Vec2 pilot_vector = pilot_position - sector_position;
  screen_position = screen_center - pilot_vector;
  distance_to_pilot = pilot_vector.length();
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
    int planet_radius = Random::GetRandomInteger(&rng, min_radius, 24);

    int npc_count = Random::GetRandomInteger(&rng, 4);
    total_npcs += npc_count;
    printf("Planet %d: seed=%u  type=%d  radius=%d  npcs=%d  ", i, planet_seed,
           planet_type, planet_radius, npc_count);

    float distance_from_star = Random::GetRandomFloat(&rng, 400);
    float angle_around_star = Random::GetRandomFloat(&rng, kTwoPi);

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
                                    planet_radius, planet_position));
  }
  for (auto &&sector_planet : planets) {
    sector_planet.planet.AdjustViewpointLatitude(blit::pi * -0.1f);
  }

  int pirate_count = Random::GetRandomInteger(&rng, 1, 3);

  printf("NPCs=%d  Pirates=%d\n", total_npcs, pirate_count);
}

void Sector::SetScreenCenter(blit::Vec2 center) { screen_center = center; }

void Sector::RenderPlanets(blit::Surface *fb) {
  // TODO:
  // Render the closesed planets into scratch fb
  Point position(0, 0);

  int i = 1;
  for (auto &&sector_planet : planets) {
    int planet_width = sector_planet.planet_radius * 2;

    if (position.x + planet_width >= fb->bounds.w) {
      position.x = 0;
      position.y += planet_width;
    }
    if (position.y + planet_width >= fb->bounds.h)
      break;

    // printf("Render Planet %d  ", i);
    // printf("Position: [%.2f, %.2f]\n", static_cast<double>(position.x),
    //        static_cast<double>(position.y));
    // Tilt the planet down a bit
    // sector_planet.planet.AdjustViewpointLatitude(blit::pi * -0.1f);
    sector_planet.planet.AdjustViewpointLongitude(blit::pi * 0.004f);
    // Render the planet into the dedicated framebuffer
    sector_planet.planet.SetDrawPosition(position.x, position.y);
    sector_planet.planet.setup_render_orthographic(fb,
                                                   planet_width, // width
                                                   planet_width, // height
                                                   0,            // camera_zoom,
                                                   0, // camera_pan_x,
                                                   0, // camera_pan_y,
                                                   blit::now());
    sector_planet.planet.Regen();
    sector_planet.planet.render_orthographic_all();

    position.x += planet_width;
    if (position.x >= fb->bounds.w) {
      position.x = 0;
      position.y += planet_width;
    }
    if (position.y >= fb->bounds.h)
      break;

    i++;
  }
}

void Sector::Draw(blit::Surface *fb) {
  for (auto &&sector_planet : planets) {
    int planet_diameter = sector_planet.planet_radius * 2;
    blit::Rect planet_rect = blit::Rect(
        // Upper left corner
        sector_planet.screen_position -
            Point(sector_planet.planet_radius, sector_planet.planet_radius),
        blit::Size(planet_diameter, planet_diameter));

    Rect cr = fb->clip.intersection(planet_rect);
    // Out of bounds check.
    if (cr.empty())
      continue;

    // Get the center for drawing the placeholder circle
    Vec2 center = sector_planet.screen_position;
    fb->pen = PICO8_WHITE;
    fb->circle(center, sector_planet.planet_radius);

    // Draw the center as a blue rect
    fb->pen = PICO8_BLUE;
    fb->rectangle(blit::Rect(center - blit::Vec2(1, 1), blit::Size(3, 3)));
    fb->pen = PICO8_WHITE;
    fb->pixel(center);

    // Draw a line to the pilot
    fb->pen = PICO8_RED;
    Point quarter_screen = Point(fb->bounds.w / 2, fb->bounds.h / 2);
    blit::screen.line(quarter_screen, center);
  }
}

void Sector::Update(blit::Vec2 pilot_position) {
  float closest_planet_distance = -1;
  for (auto &&sector_planet : planets) {
    sector_planet.UpdatePosition(pilot_position, screen_center);
    if (closest_planet_distance < 0 ||
        sector_planet.distance_to_pilot < closest_planet_distance) {
      closest_planet_distance = sector_planet.distance_to_pilot;
      closest_planet = &sector_planet;
    }
  }
}
