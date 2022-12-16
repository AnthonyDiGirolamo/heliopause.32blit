#pragma once

#include "32blit.hpp"
#include "graphics/font.hpp"
#include <span>
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
  std::span<const MenuItem> items;
  uint32_t repeat_rate;
  uint32_t last_update_time;

  Menu(std::string_view menu_title, std::span<const MenuItem> menu_items,
       const blit::Font *menu_font, int row_height, int item_top_padding,
       int item_bottom_padding, int left_margin, int right_margin);
  void Draw(blit::Surface *framebuffer, int posx, int posy);
  bool Update(uint32_t time);
  void ToggleActive();
  void NextItem();
  void PrevItem();
};

} // namespace heliopause
