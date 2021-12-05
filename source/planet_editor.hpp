#pragma once

#include "graphics/surface.hpp"
#include "menu.hpp"
#include "planet.hpp"
#include "pw_string/string_builder.h"

namespace heliopause::PlanetEditor {

extern blit::Surface planet_framebuffer;
extern Planet current_planet;
extern heliopause::Menu planet_menu;
extern pw::StringBuffer<64> last_render_update_message;

void next_planet();
void previous_planet();
void render_planet();

} // namespace heliopause::PlanetEditor
