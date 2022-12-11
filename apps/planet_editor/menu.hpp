#pragma once

#include "graphics/surface.hpp"
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
  int item_height;
  int item_text_y_offset;
  std::span<const MenuItem> items;

  Menu(std::span<const MenuItem> menu_items);
  void Draw(blit::Surface *framebuffer, int posx, int posy);
  bool Update();
  void ToggleActive();
  void NextItem();
  void PrevItem();
};

} // namespace heliopause
