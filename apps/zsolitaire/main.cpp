#include "main.hpp"

#include "32blit.hpp"

#include "assets.hpp"
#include "card_font.hpp"
#include "pw_random/random.h"
#include "pw_random/xor_shift.h"
#include "random.hpp"

using namespace blit;

Pen cream_on_transparent[] = {
  Pen(0, 0, 0, 0),       // clear
  Pen(0xf9, 0xe6, 0xcf),
};

Pen white_on_transparent[] = {
    Pen(0, 0, 0, 0),       // clear
    Pen(0xff, 0xf1, 0xe8), // #fff1e8 WHITE
};

Pen gray_on_transparent[] = {
    Pen(0, 0, 0, 0),       // clear
    Pen(0x3d, 0x3d, 0x3d), // #3d3d3d
};

Pen green_on_lightgreen[] = {
    Pen(0x33, 0x98, 0x4b), // #33984b
    Pen(0x1e, 0x6f, 0x50), // #1e6f50
};

Pen darkgreen_on_transparent[] = {
  Pen(0, 0, 0, 0), // clear,
  Pen(0x13, 0x4c, 0x4c), // #134c4c
};

Pen darkgreen_on_green[] = {
  Pen(0x1e, 0x6f, 0x50), // #1e6f50
  Pen(0x13, 0x4c, 0x4c), // #134c4c
};

const blit::Font kCardFont(mrmotext);

uint32_t seed;
static pw::random::XorShiftStarRng64 rng(123456);
TileMap *map_card;
TileMap *map_holder;
TileMap *map_holder_background;

enum CardMapLayer {
  Layer_CardOutline,
  Layer_HolderOutline,
  Layer_HolderBackground,
};

void init() {
  blit::set_screen_mode(ScreenMode::hires);

  screen.sprites = Surface::load(asset_sprite_sheet);

  map_card = TileMap::load_tmx((uint8_t *)asset_map_cards, screen.sprites, Layer_CardOutline);
  map_holder = TileMap::load_tmx((uint8_t *)asset_map_cards, screen.sprites, Layer_HolderOutline);
  map_holder_background = TileMap::load_tmx((uint8_t *)asset_map_cards, screen.sprites, Layer_HolderBackground);

  // Test sprite index fetch
  printf("tile offset: %d %d %d\n", map_card->tile_at(Point(0, 0)),
         map_card->tile_at(Point(1, 0)), map_card->tile_at(Point(0, 1)));
}

void draw_map(TileMap *map, Point pos) {
  for (int dy = 0; dy < map->bounds.h; dy++) {
    for (int dx = 0; dx < map->bounds.w; dx++) {
      Point tile_pos(dx, dy);
      uint8_t tile_index = map->tile_at(tile_pos);
      if (tile_index == 0)
        continue;
      screen.sprite(tile_index, pos + Point(8*tile_pos.x, 8*tile_pos.y));
    }
  }
}

void draw_card(int x, int y, bool holder = false) {
  Point pos(x, y);
  Point pos_shadow(x+2, y+2);
  Size map_size(5, 7);
  Size size(8*map_size.w, 8*map_size.h);
  Rect card_rect(pos, size);

  if (holder) {
    // Holder
    screen.sprites->palette = darkgreen_on_transparent;
    // map_holder->draw(&screen, card_rect, nullptr);
    draw_map(map_holder, pos);
    // Holder background
    screen.sprites->palette = darkgreen_on_green;
    // map_holder_background->draw(&screen, card_rect, nullptr);
    draw_map(map_holder_background, pos);
  }

  // Card Shadow
  screen.sprites->palette = gray_on_transparent;
  draw_map(map_card, pos + Point(1, 1));

  // Card background color
  screen.pen = white_on_transparent[1];
  int bg_offset = 5;
  Point bg_corner1 = pos + Point(bg_offset, bg_offset);
  Point bg_corner2 = bg_corner1 + Point(size.w - bg_offset*2, size.h - bg_offset*2);
  screen.rectangle(Rect(bg_corner1, bg_corner2));

  // Card
  screen.sprites->palette = cream_on_transparent;
  // map_card->draw(&screen, card_rect, nullptr);
  draw_map(map_card, pos);

  screen.pen = Pen(0, 0, 0, 255);
  screen.text("234", kCardFont, pos + Vec2(8, 8));
  screen.text("567", kCardFont, pos + Vec2(8, 16));
  screen.text("891", kCardFont, pos + Vec2(8, 24));
  screen.text("000", kCardFont, pos + Vec2(8, 32));
  screen.text("000", kCardFont, pos + Vec2(8, 40));
}

void render(uint32_t time_ms) {
  screen.alpha = 255;
  screen.mask = nullptr;
  screen.clear();

  // Reset seed
  screen.sprites->palette = green_on_lightgreen;
  rng = pw::random::XorShiftStarRng64(123456);
  // Draw background
  for (int x = 0; x < screen.bounds.w; x += 8) {
    for (int y = 0; y < screen.bounds.h; y += 8) {
      uint8_t sprite_index = 6;
      if (Random::GetRandomNumber(&rng) % 2 == 0)
        sprite_index = 7;
      screen.sprite(sprite_index, Point(x, y));
    }
  }

  draw_card(0, 0, true);
  draw_card(0, 14, false);
  draw_card(0, 14+14, false);

}

void update(uint32_t time) {
  // Empty
}
