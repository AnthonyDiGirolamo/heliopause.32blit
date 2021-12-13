#pragma once

#include "graphics/surface.hpp"
#include "menu.hpp"
#include "planet.hpp"
#include "pw_string/string_builder.h"

namespace heliopause::PlanetEditor {

extern blit::Surface planet_framebuffer;
extern Planet current_planet;
extern heliopause::Menu planet_menu;
extern pw::StringBuffer<24> last_render_update_message;
extern bool display_mode_orthographic;

void init();
void render(uint32_t time);
void update(uint32_t time);
void render_planet();
bool auto_rotate();

} // namespace heliopause::PlanetEditor
