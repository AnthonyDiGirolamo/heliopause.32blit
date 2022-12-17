#pragma once

#include "32blit.hpp"
#include "graphics/color.hpp"
#include "planet.hpp"
#include "types/vec3.hpp"
#include <sys/types.h>

#include <span>

#define PICO8_DARK_OFFSET 16
#define ENDESGA32_DARK_OFFSET 33
#define ENDESGA64_DARK_OFFSET 64

extern const std::span<const PlanetTerrain> AllPlanetTypes;
