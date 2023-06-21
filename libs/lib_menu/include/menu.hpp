#pragma once

#include "32blit.hpp"
#include "pw_span/span.h"
#include <string>

namespace heliopause {

struct MenuItem {
  std::string_view name;
  std::string_view (*get_value)();
  void (*increase_function)();
  void (*decrease_function)();
};

class Menu {
public:
  blit::Button close_button;
  blit::Button toggle_button;
  std::string_view title;
  int selected_item_index;
  int max_name_length;
  bool active;
  const blit::Font *font;
  int item_height;
  int item_top_padding;
  int item_bottom_padding;
  int left_margin;
  int right_margin;

  blit::Size size;
  uint8_t border_size;

  blit::Pen color_background;
  blit::Pen color_border;
  blit::Pen color_title_foreground;
  blit::Pen color_title_shadow;
  blit::Pen color_text_foreground;
  blit::Pen color_text_shadow;
  blit::Pen color_text_selected_foreground;
  blit::Pen color_selected_background;

  pw::span<const MenuItem> items;
  uint32_t repeat_rate;
  uint32_t last_update_time;

  Menu();
  Menu(std::string_view menu_title, pw::span<const MenuItem> menu_items,
       const blit::Font *menu_font, int row_height, int item_top_padding,
       int item_bottom_padding, int left_margin, int right_margin);
  void SetDefaults();
  void SetButtons(blit::Button closeb, blit::Button toggleb);
  void Draw(blit::Surface *framebuffer, int posx, int posy);
  bool Update(uint32_t time);
  void ToggleActive();
  void NextItem();
  void PrevItem();
};

} // namespace heliopause
