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

  int row = posy;
  framebuffer->pen = PICO8_WHITE;
  framebuffer->text(title, blit::minimal_font, blit::Point(posx, row));
  row += char_h;

  int max_name_length = 0;
  for (const MenuItem item : items) {
    if (item.name.length() > max_name_length)
      max_name_length = item.name.length();
  }

  int value_posx = max_name_length * char_w;
  for (const MenuItem item : items) {
    framebuffer->text(item.name, blit::minimal_font, blit::Point(posx, row));
    framebuffer->text(item.get_value(), blit::minimal_font,
                      blit::Point(value_posx, row));
    row += char_h;
  }
}

void Menu::ToggleActive() { active = not active; }

void Menu::Update() {
  if (not active)
    return;

  if (blit::buttons.pressed & blit::Button::X) {
    // Deactivate menu
    ToggleActive();
  }
}

} // namespace heliopause
