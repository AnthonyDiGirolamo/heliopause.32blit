#include "planet_editor.hpp"
#include "game.hpp"
#include "planet_types.hpp"

namespace heliopause::PlanetEditor {

namespace {
int selected_planet_index = 0;
uint8_t planet_pixel_data[PLANET_WIDTH * PLANET_HEIGHT];
} // namespace

blit::Surface planet_framebuffer((uint8_t *)planet_pixel_data,
                                 blit::PixelFormat::P,
                                 blit::Size(PLANET_WIDTH, PLANET_HEIGHT));

Planet current_planet = Planet(0x64063701, AllPlanets[selected_planet_index]);

void next_planet() {
  selected_planet_index = (selected_planet_index + 1) % PlanetSpan.size();
  current_planet.SetTerrain(AllPlanets[selected_planet_index]);
  current_planet.Regen();
}

void previous_planet() {
  selected_planet_index -= 1;
  if (selected_planet_index < 0)
    selected_planet_index = PlanetSpan.size() - 1;
  current_planet.SetTerrain(AllPlanets[selected_planet_index]);
  current_planet.Regen();
}

std::string_view get_noise_octaves_string() {
  static pw::StringBuffer<16> noise_octaves_value_string;
  noise_octaves_value_string.clear();
  noise_octaves_value_string.Format("%d", current_planet.terrain.noise_octaves);
  return noise_octaves_value_string.view();
}

void increase_noise_octaves() {
  current_planet.terrain.noise_octaves += 1;
  // Arbitrary upper limit
  if (current_planet.terrain.noise_octaves > 16)
    current_planet.terrain.noise_octaves = 16;
}

void decrease_noise_octaves() {
  int octaves = current_planet.terrain.noise_octaves - 1;
  if (octaves < 1)
    octaves = 1;
  current_planet.terrain.noise_octaves = octaves;
}

std::string_view get_noise_zoom_string() {
  static pw::StringBuffer<16> noise_zoom_value_string;
  noise_zoom_value_string.clear();
  noise_zoom_value_string.Format("%.2f", current_planet.terrain.noise_zoom);
  return noise_zoom_value_string.view();
}

void increase_noise_zoom() {}

void decrease_noise_zoom() {}

std::string_view get_noise_persistance_string() {
  static pw::StringBuffer<16> noise_persistance_value_string;
  noise_persistance_value_string.clear();
  noise_persistance_value_string.Format(
      "%.2f", current_planet.terrain.noise_persistance);
  return noise_persistance_value_string.view();
}

void increase_noise_persistance() {}

void decrease_noise_persistance() {}

// TODO: Add seed display+editing to planet_menu

// Random::IncrementSeed(1);
// // blit::debugf("Seed: %d\n", Random::GetCurrentSeed());
// heliopause::PlanetEditor::current_planet.SetTerrainAndSeed(
//     Random::GetCurrentSeed(),
//     AllPlanets[selected_planet_index]);

static constexpr heliopause::MenuItem planet_menu_items[] = {
    {
        .name = std::string_view{"noise octaves"},
        .get_value = &get_noise_octaves_string,
        .increase_function = &increase_noise_octaves,
        .decrease_function = &decrease_noise_octaves,
    },
    {
        .name = std::string_view{"noise zoom"},
        .get_value = &get_noise_zoom_string,
        .increase_function = &increase_noise_zoom,
        .decrease_function = &decrease_noise_zoom,
    },
    {
        .name = std::string_view{"noise persistance"},
        .get_value = &get_noise_persistance_string,
        .increase_function = &increase_noise_persistance,
        .decrease_function = &decrease_noise_persistance,
    },
};

constexpr std::span<const heliopause::MenuItem>
    planet_menu_items_span(planet_menu_items);
heliopause::Menu planet_menu = heliopause::Menu(planet_menu_items_span);

uint32_t last_rotation = 0;
uint32_t last_render_duration = 0;
// std::string last_render_duration_string;
pw::StringBuffer<64> last_render_update_message;

void render_planet() {
  uint32_t start_time = blit::now();
  // planet_framebuffer.clip = Rect(0, 0, 10, 10);

  // Black outline around planet
  // planet_framebuffer.pen = 0;
  // Draw::circle(&planet_framebuffer, 60, 60, 60, true);

  current_planet.SetDrawOffset(0, 0);
  current_planet.render_orthographic(&planet_framebuffer, PLANET_HEIGHT);

  /*
  // Render the planet in 3d
  current_planet.SetDrawOffset(0, 0);
  current_planet.render_orthographic(&planet_framebuffer, 120);

  // Render the planet in 3d facing the opposite side
  float original_lambda = current_planet.viewpoint_lambda0;
  current_planet.viewpoint_lambda0 += blit::pi;
  current_planet.SetDrawOffset(120, 0);
  current_planet.render_orthographic(&planet_framebuffer, 120);
  current_planet.viewpoint_lambda0 = original_lambda;

  // Render full flattened map
  current_planet.SetDrawOffset(0, 120);
  current_planet.render_equirectangular(&planet_framebuffer, 240, 120);
  */

  last_rotation = blit::now();
  last_render_duration = last_rotation - start_time;
  // blit::debugf("Render time: %d\n", last_render_duration);
  // last_render_duration_string = std::to_string(last_render_duration);
  last_render_update_message.clear();
  last_render_update_message.Format("%d ms", (int)last_render_duration);
}

} // namespace heliopause::PlanetEditor
