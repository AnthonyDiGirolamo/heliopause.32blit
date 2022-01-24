#pragma once

#include "graphics/surface.hpp"
#include "menu.hpp"
#include "planet.hpp"
#include "pw_string/string_builder.h"

namespace heliopause::Game {

extern blit::Surface planet_framebuffer;

void init();
void render(uint32_t time);
void update(uint32_t time);

} // namespace heliopause::Game
