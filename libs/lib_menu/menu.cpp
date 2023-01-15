#include "menu.hpp"
#include "32blit.hpp"
#include "colors.hpp"
#include <string>

namespace heliopause {

Menu::Menu() {}

Menu::Menu(std::string_view menu_title, std::span<const MenuItem> menu_items,
           const blit::Font *menu_font, int text_height,
           int rhs_item_top_padding, int rhs_item_bottom_padding,
           int rhs_left_margin, int rhs_right_margin) {
  close_button = blit::Button::X;
  toggle_button = blit::Button::A;
  size = blit::screen.bounds;

  items = menu_items;
  title = menu_title;
  active = false;
  repeat_rate = 200;
  left_margin = rhs_left_margin;
  right_margin = rhs_right_margin;

  // Calculate max width of menu item titles.
  max_name_length = 0;
  for (const MenuItem item : items) {
    if ((int)item.name.length() > max_name_length)
      max_name_length = item.name.length();
  }

  font = menu_font;

  item_height = text_height;
  if (item_height == 0) {
    item_height = font->char_h + font->spacing_y;
  }
  printf("MenuFont w:%u, h:%u\n", menu_font->char_w, menu_font->char_h);

  item_top_padding = rhs_item_top_padding;
  item_bottom_padding = rhs_item_bottom_padding;

  // // m3x6_font
  // item_height = 10;
  // top_padding = -3;

  // // m6x11_font
  // item_height = 17;
  // top_padding = 3;
}

void Menu::SetButtons(blit::Button closeb, blit::Button toggleb) {
  close_button = closeb;
  toggle_button = toggleb;
}

void Menu::Draw(blit::Surface *framebuffer, int posx, int posy) {
  if (not active)
    return;

  int char_w = font->char_w;
  int char_h = item_height + item_top_padding + item_bottom_padding;

  // int total_rows = 1 + items.size(); // 1 for the title
  // int total_height = char_h * total_rows;

  // TODO: Right justify options text
  int total_cols = max_name_length * 2;
  int total_width = char_w * total_cols + left_margin + right_margin;
  // Shaded Background Rect
  framebuffer->pen = blit::Pen(0, 0, 0, 128);
  framebuffer->rectangle(Rect(Point(posx, posy), size));

  posx += left_margin;

  int row = 0;

  // Menu title
  framebuffer->pen = PICO8_ORANGE;
  framebuffer->text(title, *font, blit::Point(posx, posy + item_top_padding));
  row += 1;

  // Items
  framebuffer->pen = PICO8_WHITE;
  int value_posx = max_name_length * char_w;

  for (const MenuItem item : items) {
    int y = posy + (row * char_h);
    if (row - 1 == selected_item_index) {
      framebuffer->pen = blit::Pen(0, 0, 0, 200);
      framebuffer->rectangle(Rect(posx - left_margin, y, total_width, char_h));

      framebuffer->pen = PICO8_BLUE;
    }
    framebuffer->text(item.name, *font,
                      blit::Point(posx, y + item_top_padding));
    framebuffer->text(item.get_value(), *font,
                      blit::Point(value_posx, y + item_top_padding));
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

bool Menu::Update(uint32_t time) {
  bool menu_action_was_run = false;
  if (not active) {
    return menu_action_was_run;
  }

  if (blit::buttons.pressed & close_button) {
    // Deactivate menu
    ToggleActive();
  }

  if (time - last_update_time > repeat_rate) {
    bool input_occured = false;

    if (blit::buttons & blit::Button::DPAD_DOWN || blit::joystick.y >= 0.1f) {
      input_occured = true;
      NextItem();

    } else if (blit::buttons & blit::Button::DPAD_UP ||
               blit::joystick.y <= -0.1f) {
      input_occured = true;
      PrevItem();

    } else if ((blit::buttons & blit::Button::DPAD_RIGHT) |
               (blit::buttons.pressed & toggle_button)) {
      input_occured = true;
      items[selected_item_index].increase_function();
      menu_action_was_run = true;

    } else if (blit::buttons & blit::Button::DPAD_LEFT) {
      input_occured = true;
      if (items[selected_item_index].decrease_function != nullptr) {
        items[selected_item_index].decrease_function();
      } else {
        items[selected_item_index].increase_function();
      }
      menu_action_was_run = true;
    }

    if (input_occured)
      last_update_time = time;
  }

  return menu_action_was_run;
}

} // namespace heliopause
