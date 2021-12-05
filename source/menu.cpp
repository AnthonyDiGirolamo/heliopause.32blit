#include "menu.hpp"
#include "32blit.hpp"
#include "colors.hpp"

namespace heliopause {

Menu::Menu(std::span<const MenuItem> menu_items) {
  items = menu_items;
  title = std::string_view{"Parameters"};
  active = false;
}

void Menu::Draw(blit::Surface *framebuffer, int posx, int posy) {
  if (not active)
    return;

  int char_w = blit::minimal_font.char_w;
  int char_h = blit::minimal_font.char_h;

  int total_rows = 1;
  total_rows += items.size();
  int max_name_length = 0;
  for (const MenuItem item : items) {
    if ((int)item.name.length() > max_name_length)
      max_name_length = item.name.length();
  }

  framebuffer->pen = blit::Pen(0, 0, 0, 128);
  framebuffer->rectangle(
      Rect(posx, posy, char_w * (max_name_length + 5), char_h * total_rows));

  int row = 0;
  framebuffer->text(title, blit::minimal_font,
                    blit::Point(posx, posy + (row * char_h)));
  row += 1;

  framebuffer->pen = PICO8_WHITE;
  int value_posx = max_name_length * char_w;

  for (const MenuItem item : items) {
    int y = posy + (row * char_h);
    if (row - 1 == selected_item_index) {
      framebuffer->pen = PICO8_BLUE;
    }
    framebuffer->text(item.name, blit::minimal_font, blit::Point(posx, y));
    framebuffer->text(item.get_value(), blit::minimal_font,
                      blit::Point(value_posx, y));
    if (row - 1 == selected_item_index) {
      framebuffer->pen = PICO8_WHITE;
    }
    row += 1;
  }
}

void Menu::ToggleActive() { active = not active; }

void Menu::NextItem() {
  selected_item_index += 1;
  if (selected_item_index >= (int)items.size())
    selected_item_index = 0;
}

void Menu::PrevItem() {
  selected_item_index -= 1;
  if (selected_item_index < 0)
    selected_item_index = (int)items.size() - 1;
}

bool Menu::Update() {
  if (not active)
    return false;

  if (blit::buttons.pressed & blit::Button::X) {
    // Deactivate menu
    ToggleActive();
  } else if (blit::buttons.pressed & blit::Button::DPAD_DOWN) {
    NextItem();
  } else if (blit::buttons.pressed & blit::Button::DPAD_UP) {
    PrevItem();
  } else if (blit::buttons.pressed & blit::Button::DPAD_RIGHT) {
    items[selected_item_index].increase_function();
    return true;
  } else if (blit::buttons.pressed & blit::Button::DPAD_LEFT) {
    items[selected_item_index].decrease_function();
    return true;
  }

  return false;
}

} // namespace heliopause
