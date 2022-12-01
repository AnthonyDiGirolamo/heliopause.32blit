#include "planet_editor.hpp"
#include "colors.hpp"
#include "planet_types.hpp"
#include "platform.hpp"
#include "random.hpp"

#if PICO_ON_DEVICE
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#endif

namespace heliopause::PlanetEditor {

bool display_mode_orthographic = true;

Planet current_planet = Planet(0x64063701, AllPlanets[0]);

namespace {

uint32_t last_render_time = 0;
uint32_t last_render_duration = 0;

bool not_rendered = true;

std::string_view enabled_string = {"true"};
std::string_view disabled_string = {"false"};
std::string_view orthographic_string = {"3D"};
std::string_view equirectangular_string = {"Flat"};

pw::StringBuffer<20> menu_item_value;

bool auto_rotation = false;

float camera_zoom = 1.0;
int camera_pan_x = 0;
int camera_pan_y = 0;

int selected_planet_index = 0;

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
  menu_item_value.clear();
  menu_item_value.Format("%d", (int)current_planet.terrain.noise_octaves);
  return menu_item_value.view();
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
  menu_item_value.clear();
  menu_item_value.Format("%.2f", (double)current_planet.terrain.noise_zoom);
  return menu_item_value.view();
}

void increase_noise_zoom() { current_planet.terrain.noise_zoom += 0.05f; }

void decrease_noise_zoom() { current_planet.terrain.noise_zoom -= 0.05f; }

std::string_view get_noise_persistance_string() {
  menu_item_value.clear();
  menu_item_value.Format("%.2f",
                         (double)current_planet.terrain.noise_persistance);
  return menu_item_value.view();
}

void increase_noise_persistance() {
  current_planet.terrain.noise_persistance += 0.05f;
}

void decrease_noise_persistance() {
  current_planet.terrain.noise_persistance -= 0.05f;
}

std::string_view get_seed_string() {
  menu_item_value.clear();
  menu_item_value.Format("%d", (int)Random::GetCurrentSeed());
  return menu_item_value.view();
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

  planet_framebuffer.pen = 49;
  planet_framebuffer.clear();
}

std::string_view get_latitude_bias_string() {
  menu_item_value.clear();
  menu_item_value.Format("%.2f", (double)current_planet.terrain.latitude_bias);
  return menu_item_value.view();
}

void increase_latitude_bias() { current_planet.terrain.latitude_bias += 0.01f; }

void decrease_latitude_bias() { current_planet.terrain.latitude_bias -= 0.01f; }

std::string_view get_camera_zoom_string() {
  menu_item_value.clear();
  menu_item_value.Format("%.2f", (double)camera_zoom);
  return menu_item_value.view();
}
void increase_camera_zoom() { camera_zoom += 0.05f; }
void decrease_camera_zoom() {
  camera_zoom -= 0.05f;
  if (camera_zoom < 0.1f)
    camera_zoom = 0.1f;
}

std::string_view get_camera_pan_x_string() {
  menu_item_value.clear();
  menu_item_value.Format("%.2f", (double)camera_pan_x);
  return menu_item_value.view();
}
void increase_camera_pan_x() { camera_pan_x += 10; }
void decrease_camera_pan_x() { camera_pan_x -= 10; }

std::string_view get_camera_pan_y_string() {
  menu_item_value.clear();
  menu_item_value.Format("%.2f", (double)camera_pan_y);
  return menu_item_value.view();
}
void increase_camera_pan_y() { camera_pan_y += 10; }
void decrease_camera_pan_y() { camera_pan_y -= 10; }

std::string_view get_color_padding_start_string() {
  menu_item_value.clear();
  menu_item_value.Format("%d", current_planet.terrain.color_padding_start);
  return menu_item_value.view();
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
  menu_item_value.clear();
  menu_item_value.Format("%d", current_planet.terrain.color_padding_end);
  return menu_item_value.view();
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
        .name = std::string_view{"Latitude Bias"},
        .get_value = &get_latitude_bias_string,
        .increase_function = &increase_latitude_bias,
        .decrease_function = &decrease_latitude_bias,
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
        .name = std::string_view{"Camera Zoom"},
        .get_value = &get_camera_zoom_string,
        .increase_function = &increase_camera_zoom,
        .decrease_function = &decrease_camera_zoom,
    },
    {
        .name = std::string_view{"Camera Pan_X"},
        .get_value = &get_camera_pan_x_string,
        .increase_function = &increase_camera_pan_x,
        .decrease_function = &decrease_camera_pan_x,
    },
    {
        .name = std::string_view{"Camera Pan_Y"},
        .get_value = &get_camera_pan_y_string,
        .increase_function = &increase_camera_pan_y,
        .decrease_function = &decrease_camera_pan_y,
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

} // namespace

pw::StringBuffer<24> last_render_update_message;
pw::StringBuffer<32> planet_metadata;

heliopause::Menu planet_menu = heliopause::Menu(planet_menu_items_span);

void render_planet() {
  uint32_t start_time = blit::now();
  // planet_framebuffer.clip = blit::Rect(0, 0, 10, 10);

  // Black outline around planet
  // planet_framebuffer.pen = 0;
  // Draw::circle(&planet_framebuffer, 60, 60, 60, true);

  if (display_mode_orthographic) {
    current_planet.SetDrawOffset(0, 0);
    current_planet.render_orthographic(
        &planet_framebuffer, PLANET_FRAMEBUFFER_WIDTH,
        PLANET_FRAMEBUFFER_HEIGHT, camera_zoom, camera_pan_x, camera_pan_y);
  } else {
    current_planet.SetDrawOffset(0, 0);
    current_planet.render_equirectangular(&planet_framebuffer,
                                          PLANET_FRAMEBUFFER_WIDTH,
                                          PLANET_FRAMEBUFFER_HEIGHT);
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
  printf("Render time: %d\n", last_render_duration);
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
    current_planet.AdjustViewpointLongitude(blit::pi * 0.01f);
    return true;
  }
  return false;
}

void init() {
  planet_framebuffer.palette = PICO8;
  // TODO: Does this matter?
  planet_framebuffer.alpha = 0;

  // TODO: Figure out exactly to set transparent color index correctly. This
  // seems to work with the index != pen when clearing.
  planet_framebuffer.transparent_index = 48;
  planet_framebuffer.pen = 49;
  planet_framebuffer.clear();

  planet_metadata.Format("Rendering...");

#ifdef PICO_ON_DEVICE
  queue_init(&heliopause::call_queue, sizeof(heliopause::queue_entry_t), 2);
  queue_init(&heliopause::results_queue, sizeof(int32_t), 2);
  multicore_launch_core1(heliopause::core1_entry);
#endif
}

void render(uint32_t time) {
  // Clear screen
  blit::screen.pen = blit::Pen(64, 64, 64, 255);
  blit::screen.alpha = 255;
  blit::screen.clear();
  blit::screen.mask = nullptr;

  int char_h_offset = -5;
  int xoffset = 0;
  // // Debug: Show planet_framebuffer draw bounds
  // blit::screen.pen = PICO8_INDIGO;
  // int xoffset = 32;
  // Draw::rectangle(&blit::screen, xoffset + 0, 0, PLANET_FRAMEBUFFER_WIDTH,
  //                 PLANET_FRAMEBUFFER_HEIGHT);

  // if (blit::screen.bounds.w > blit::screen.bounds.h) {
  //   xoffset += (int)((blit::screen.bounds.w - blit::screen.bounds.h) * 0.5f);
  // }

  if (!heliopause::PlanetEditor::display_mode_orthographic)
    xoffset = 0;

  blit::screen.blit(
      &planet_framebuffer,
      blit::Rect(0, 0, PLANET_FRAMEBUFFER_WIDTH, PLANET_FRAMEBUFFER_HEIGHT),
      blit::Point(xoffset + 0, 0));

  // // Draw at 2x size
  // blit::screen.stretch_blit(&heliopause::PlanetEditor::planet_framebuffer,
  //                           blit::Rect(0, 0, PLANET_FRAMEBUFFER_WIDTH,
  //                           PLANET_FRAMEBUFFER_HEIGHT), blit::Rect(xoffset +
  //                           0, 0, PLANET_FRAMEBUFFER_WIDTH*2,
  //                           PLANET_FRAMEBUFFER_HEIGHT*2));

  if (heliopause::PlanetEditor::planet_menu.active) {
    // Draw the menu if active
    heliopause::PlanetEditor::planet_menu.Draw(&blit::screen, 0, 0);

  } else {
    // Planet Type name in upper left.
    // Text Shadow
    int color_index =
        heliopause::PlanetEditor::current_planet.terrain.map_icon_color + 16;
    blit::screen.pen = PICO8[color_index];
    blit::screen.text(
        heliopause::PlanetEditor::current_planet.terrain.type_string,
        heliopause::kCustomFont,
        blit::Point(3, blit::screen.bounds.h - 24 + 1 + char_h_offset));
    // Text
    color_index =
        heliopause::PlanetEditor::current_planet.terrain.map_icon_color;
    blit::screen.pen = PICO8[color_index];
    blit::screen.text(
        heliopause::PlanetEditor::current_planet.terrain.type_string,
        heliopause::kCustomFont,
        blit::Point(2, blit::screen.bounds.h - 24 + char_h_offset));

    // Planet info
    blit::screen.pen = PICO8_BLACK;
    blit::screen.text(
        planet_metadata.view(), heliopause::kCustomFont,
        blit::Point(2 + 1, blit::screen.bounds.h - 16 + 1 + char_h_offset));
    blit::screen.pen = PICO8_WHITE;
    blit::screen.text(
        planet_metadata.view(), heliopause::kCustomFont,
        blit::Point(2, blit::screen.bounds.h - 16 + char_h_offset));
  }

  // Render time
  blit::screen.pen = PICO8_BLACK;
  blit::screen.text(
      heliopause::PlanetEditor::last_render_update_message.view(),
      heliopause::kCustomFont,
      blit::Point(2 + 1, blit::screen.bounds.h - 8 + 1 + char_h_offset));
  blit::screen.pen = PICO8_WHITE;
  blit::screen.text(heliopause::PlanetEditor::last_render_update_message.view(),
                    heliopause::kCustomFont,
                    blit::Point(2, blit::screen.bounds.h - 8 + char_h_offset));
}

bool planet_render_done = false;

void render_planet_on_core_1() {
  while (!heliopause::PlanetEditor::current_planet.render_orthographic_done()) {
    // TODO: Add an abort flag
    heliopause::PlanetEditor::current_planet.render_orthographic_line();
  }
  planet_render_done = true;
}

#ifdef PICO_ON_DEVICE
queue_entry_t entry;
#endif

void render_planet_complete() {
  last_render_time = blit::now();
  last_render_duration =
      last_render_time -
      heliopause::PlanetEditor::current_planet.render_orthographic_start_time();
  // blit::debugf("Render time: %d\n", last_render_duration);
  printf("Render time: %d\n", last_render_duration);
  // last_render_duration_string = std::to_string(last_render_duration);
  last_render_update_message.clear();
  last_render_update_message.Format("Render Time: %d ms",
                                    (int)last_render_duration);

  planet_metadata.clear();
  planet_metadata.Format(
      "Noise Range: [%.2f, %.2f]",
      (double)heliopause::PlanetEditor::current_planet.min_noise,
      (double)heliopause::PlanetEditor::current_planet.max_noise);
}

void update(uint32_t time) {
  bool rerender = false;
  if (not_rendered
#ifdef PICO_ON_DEVICE
      // Pico needs time to startup USB
      && time > 3000
#endif
  ) {
    rerender = true;
    not_rendered = false;
  }

  if (heliopause::PlanetEditor::planet_menu.active) {
    rerender = heliopause::PlanetEditor::planet_menu.Update();
  } else if (buttons.pressed & Button::X) {
    // Activate menu
    heliopause::PlanetEditor::planet_menu.ToggleActive();
  } else if (buttons.pressed & Button::DPAD_UP) {
    heliopause::PlanetEditor::current_planet.AdjustViewpointLatitude(blit::pi *
                                                                     0.1f);
    rerender = true;
  } else if (buttons.pressed & Button::DPAD_DOWN) {
    heliopause::PlanetEditor::current_planet.AdjustViewpointLatitude(blit::pi *
                                                                     -0.1f);
    rerender = true;
  } else if (buttons.pressed & Button::DPAD_LEFT) {
    heliopause::PlanetEditor::current_planet.AdjustViewpointLongitude(blit::pi *
                                                                      -0.1f);
    rerender = true;
  } else if (buttons.pressed & Button::DPAD_RIGHT) {
    heliopause::PlanetEditor::current_planet.AdjustViewpointLongitude(blit::pi *
                                                                      0.1f);
    rerender = true;
    // Picosystem manual reboot
    // #if PICO_ON_DEVICE
    //   } else if (buttons.pressed & Button::Y) {
    //     reset_usb_boot(0, 0);
    // #endif
  }

#ifdef PICO_ON_DEVICE
#else
  if (not rerender)
    rerender = heliopause::PlanetEditor::auto_rotate();
#endif

  if (rerender) {
    heliopause::PlanetEditor::current_planet.Regen();
    planet_render_done = false;
    // heliopause::PlanetEditor::render_planet();
    if (display_mode_orthographic) {
      current_planet.SetDrawOffset(0, 0);
      current_planet.setup_render_orthographic(
          &planet_framebuffer, PLANET_FRAMEBUFFER_WIDTH,
          PLANET_FRAMEBUFFER_HEIGHT, camera_zoom, camera_pan_x, camera_pan_y,
          blit::now());
#ifdef PICO_ON_DEVICE
      entry.func = &render_planet_on_core_1;
      entry.data = 1;
      queue_add_blocking(&heliopause::call_queue, &entry);
#else
      current_planet.render_orthographic_all();
      render_planet_complete();
#endif
    }
  }

#ifdef PICO_ON_DEVICE
  if (planet_render_done) {
    planet_render_done = false;
    int32_t res;
    queue_remove_blocking(&heliopause::results_queue, &res);
    render_planet_complete();
  }
#else
  if (!heliopause::PlanetEditor::current_planet.render_orthographic_done()) {
    heliopause::PlanetEditor::current_planet.render_orthographic_line();

    if (heliopause::PlanetEditor::current_planet.render_orthographic_done()) {
      render_planet_complete();
    }
  }
#endif
}

} // namespace heliopause::PlanetEditor
