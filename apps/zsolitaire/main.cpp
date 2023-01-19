#include "main.hpp"

#include "32blit.hpp"
#include "pw_random/random.h"
#include "pw_random/xor_shift.h"

#include "assets.hpp"
#include "card_font.hpp"
#include "colors.hpp"
#include "random.hpp"

using namespace blit;

enum SpritePaletteColor {
  BG = 0,
  FG = 1,
};

Pen clear = Pen(0, 0, 0, 0);

Pen sprite_colors[]{
    Pen(0, 0, 0, 0), // clear
    Pen(0xff, 0xff, 0xff, 255),
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
  screen.sprites->palette = sprite_colors;

  map_card = TileMap::load_tmx((uint8_t *)asset_map_cards, screen.sprites,
                               Layer_CardOutline);
  map_holder = TileMap::load_tmx((uint8_t *)asset_map_cards, screen.sprites,
                                 Layer_HolderOutline);
  map_holder_background = TileMap::load_tmx(
      (uint8_t *)asset_map_cards, screen.sprites, Layer_HolderBackground);

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
      screen.sprite(tile_index, pos + Point(8 * tile_pos.x, 8 * tile_pos.y));
    }
  }
}

void draw_card(int x, int y, bool holder = false) {
  Point pos(x, y);
  Point pos_shadow(x + 2, y + 2);
  Size map_size(5, 7);
  Size size(8 * map_size.w, 8 * map_size.h);
  Rect card_rect(pos, size);

  if (holder) {
    // Holder
    sprite_colors[FG] = ENDESGA64[Ocean2];
    sprite_colors[BG] = clear;
    // map_holder->draw(&screen, card_rect, nullptr);
    draw_map(map_holder, pos);
    // Holder background
    sprite_colors[FG] = ENDESGA64[Ocean1];
    sprite_colors[BG] = ENDESGA64[Ocean2];
    // map_holder_background->draw(&screen, card_rect, nullptr);
    draw_map(map_holder_background, pos);
  }

  // Card Shadow
  sprite_colors[FG] = ENDESGA64[Coffee0];
  sprite_colors[FG].a = 128;
  sprite_colors[BG] = clear;
  draw_map(map_card, pos + Point(2, 2));

  // Card background color
  screen.pen = ENDESGA64[Coffee7];
  int bg_offset = 5;
  Point bg_corner1 = pos + Point(bg_offset, bg_offset);
  Point bg_corner2 =
      bg_corner1 + Point(size.w - bg_offset * 2, size.h - bg_offset * 2);
  screen.rectangle(Rect(bg_corner1, bg_corner2));

  // Card Border
  sprite_colors[FG] = ENDESGA64[Coffee6];
  sprite_colors[BG] = clear;
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
  sprite_colors[FG] = ENDESGA64[Ocean0];
  sprite_colors[BG] = ENDESGA64[Ocean1];

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

  // Test card drawing
  draw_card(24, 0, true);
  draw_card(16, 14, false);
  draw_card(8, 14 + 14, false);
  draw_card(0, 14 + 14 + 14, false);
}

void update(uint32_t time) {
  // Empty
}
