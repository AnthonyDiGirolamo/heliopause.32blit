#include "planet_editor.hpp"
#include "game.hpp"
#include "planet_types.hpp"
#include "random.hpp"

namespace heliopause::PlanetEditor {

namespace {

std::string_view enabled_string = {"true"};
std::string_view disabled_string = {"false"};
std::string_view orthographic_string = {"3D"};
std::string_view equirectangular_string = {"Flat"};

bool auto_rotation = false;
bool display_mode_orthographic = true;

int selected_planet_index = 0;

uint8_t planet_pixel_data[PLANET_WIDTH * PLANET_HEIGHT];

} // namespace

blit::Surface planet_framebuffer((uint8_t *)planet_pixel_data,
                                 blit::PixelFormat::P,
                                 blit::Size(PLANET_WIDTH, PLANET_HEIGHT));

Planet current_planet = Planet(0x64063701, AllPlanets[0]);

namespace {

std::string_view get_planet_type_string() {
  return current_planet.terrain.type_string;
}

void next_planet() {
  selected_planet_index = (selected_planet_index + 1) % PlanetSpan.size();
  current_planet.SetTerrain(AllPlanets[selected_planet_index]);
}

void previous_planet() {
  selected_planet_index -= 1;
  if (selected_planet_index < 0)
    selected_planet_index = PlanetSpan.size() - 1;
  current_planet.SetTerrain(AllPlanets[selected_planet_index]);
}

std::string_view get_noise_octaves_string() {
  static pw::StringBuffer<16> noise_octaves_value_string;
  noise_octaves_value_string.clear();
  noise_octaves_value_string.Format("%d",
                                    (int)current_planet.terrain.noise_octaves);
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
  noise_zoom_value_string.Format("%.2f",
                                 (double)current_planet.terrain.noise_zoom);
  return noise_zoom_value_string.view();
}

void increase_noise_zoom() { current_planet.terrain.noise_zoom += 0.05f; }

void decrease_noise_zoom() { current_planet.terrain.noise_zoom -= 0.05f; }

std::string_view get_noise_persistance_string() {
  static pw::StringBuffer<16> noise_persistance_value_string;
  noise_persistance_value_string.clear();
  noise_persistance_value_string.Format(
      "%.2f", (double)current_planet.terrain.noise_persistance);
  return noise_persistance_value_string.view();
}

void increase_noise_persistance() {
  current_planet.terrain.noise_persistance += 0.05f;
}

void decrease_noise_persistance() {
  current_planet.terrain.noise_persistance -= 0.05f;
}

std::string_view get_seed_string() {
  static pw::StringBuffer<16> seed_value_string;
  seed_value_string.clear();
  seed_value_string.Format("%d", (int)Random::GetCurrentSeed());
  return seed_value_string.view();
}

void increase_seed() {
  Random::IncrementSeed(1);
  current_planet.SetSeed(Random::GetCurrentSeed());
}

void decrease_seed() {
  Random::IncrementSeed(-1);
  current_planet.SetSeed(Random::GetCurrentSeed());
}

std::string_view get_auto_rotation_string() {
  if (auto_rotation)
    return enabled_string;
  return disabled_string;
}
void toggle_auto_rotation() { auto_rotation = not auto_rotation; }

std::string_view get_display_mode_string() {
  if (display_mode_orthographic)
    return orthographic_string;
  return equirectangular_string;
}

void toggle_display_mode() {
  display_mode_orthographic = not display_mode_orthographic;

  planet_framebuffer.pen = 255;
  planet_framebuffer.clear();
}

std::string_view get_color_padding_start_string() {
  static pw::StringBuffer<8> color_padding_start_value_string;
  color_padding_start_value_string.clear();
  color_padding_start_value_string.Format(
      "%d", current_planet.terrain.color_padding_start);
  return color_padding_start_value_string.view();
}

void increase_color_padding_start() {
  current_planet.terrain.color_padding_start += 1;
  // Arbitrary upper limit
  if (current_planet.terrain.color_padding_start > 16)
    current_planet.terrain.color_padding_start = 16;
}

void decrease_color_padding_start() {
  int octaves = current_planet.terrain.color_padding_start - 1;
  if (octaves < 1)
    octaves = 1;
  current_planet.terrain.color_padding_start = octaves;
}

std::string_view get_color_padding_end_string() {
  static pw::StringBuffer<8> color_padding_end_value_string;
  color_padding_end_value_string.clear();
  color_padding_end_value_string.Format(
      "%d", current_planet.terrain.color_padding_end);
  return color_padding_end_value_string.view();
}

void increase_color_padding_end() {
  current_planet.terrain.color_padding_end += 1;
  // Arbitrary upper limit
  if (current_planet.terrain.color_padding_end > 16)
    current_planet.terrain.color_padding_end = 16;
}

void decrease_color_padding_end() {
  int octaves = current_planet.terrain.color_padding_end - 1;
  if (octaves < 1)
    octaves = 1;
  current_planet.terrain.color_padding_end = octaves;
}

static constexpr heliopause::MenuItem planet_menu_items[] = {
    {
        .name = std::string_view{"Type"},
        .get_value = &get_planet_type_string,
        .increase_function = &next_planet,
        .decrease_function = &previous_planet,
    },
    {
        .name = std::string_view{"Seed"},
        .get_value = &get_seed_string,
        .increase_function = &increase_seed,
        .decrease_function = &decrease_seed,
    },
    {
        .name = std::string_view{"Noise Octaves"},
        .get_value = &get_noise_octaves_string,
        .increase_function = &increase_noise_octaves,
        .decrease_function = &decrease_noise_octaves,
    },
    {
        .name = std::string_view{"Noise Zoom"},
        .get_value = &get_noise_zoom_string,
        .increase_function = &increase_noise_zoom,
        .decrease_function = &decrease_noise_zoom,
    },
    {
        .name = std::string_view{"Noise Persistance"},
        .get_value = &get_noise_persistance_string,
        .increase_function = &increase_noise_persistance,
        .decrease_function = &decrease_noise_persistance,
    },
    {
        .name = std::string_view{"ColorPadding Start"},
        .get_value = &get_color_padding_start_string,
        .increase_function = &increase_color_padding_start,
        .decrease_function = &decrease_color_padding_start,
    },
    {
        .name = std::string_view{"ColorPadding End"},
        .get_value = &get_color_padding_end_string,
        .increase_function = &increase_color_padding_end,
        .decrease_function = &decrease_color_padding_end,
    },
    {
        .name = std::string_view{"DisplayMode"},
        .get_value = &get_display_mode_string,
        .increase_function = &toggle_display_mode,
        .decrease_function = &toggle_display_mode,
    },
    {
        .name = std::string_view{"AutoRotate"},
        .get_value = &get_auto_rotation_string,
        .increase_function = &toggle_auto_rotation,
        .decrease_function = &toggle_auto_rotation,
    },
};

constexpr std::span<const heliopause::MenuItem>
    planet_menu_items_span(planet_menu_items);

uint32_t last_render_time = 0;
uint32_t last_render_duration = 0;

} // namespace

pw::StringBuffer<24> last_render_update_message;

heliopause::Menu planet_menu = heliopause::Menu(planet_menu_items_span);

void render_planet() {
  uint32_t start_time = blit::now();
  // planet_framebuffer.clip = Rect(0, 0, 10, 10);

  // Black outline around planet
  // planet_framebuffer.pen = 0;
  // Draw::circle(&planet_framebuffer, 60, 60, 60, true);

  if (display_mode_orthographic) {
    current_planet.SetDrawOffset(0, 0);
    current_planet.render_orthographic(&planet_framebuffer, PLANET_HEIGHT);
  } else {
    current_planet.SetDrawOffset(0, 0);
    current_planet.render_equirectangular(&planet_framebuffer, PLANET_WIDTH,
                                          PLANET_HEIGHT);
  }

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

  last_render_time = blit::now();
  last_render_duration = last_render_time - start_time;
  // blit::debugf("Render time: %d\n", last_render_duration);
  // last_render_duration_string = std::to_string(last_render_duration);
  last_render_update_message.clear();
  last_render_update_message.Format("Render Time: %d ms",
                                    (int)last_render_duration);
}

bool auto_rotate() {
  if (not auto_rotation || not display_mode_orthographic)
    return false;
  // Auto rotate - only good on host machine
  uint32_t time_now = blit::now();
  if (time_now > last_render_time + 50) {
    current_planet.AdjustViewpointLongitude(blit::pi * 0.005f);
    return true;
  }
  return false;
}

} // namespace heliopause::PlanetEditor
