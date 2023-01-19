#include "main.hpp"

#include <cstddef>
#include <cstdint>
#include <deque>

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
TileMap *map_card_outline;
TileMap *map_card_filled;
TileMap *map_holder;
TileMap *map_holder_background;

enum CardMapLayer {
  Layer_CardOutline,
  Layer_CardFilled,
  Layer_HolderOutline,
  Layer_HolderBackground,
};

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

namespace Solitaire {

enum Suit {
  Spade,
  Heart,
  Diamond,
  Clover,
  Tarot,
  DragonDagger,
  DragonMirror,
  DragonWealth,
};

Pen SuitColors[]{
    ENDESGA64[Black0],  // Spade
    ENDESGA64[Blood],   // Heart
    ENDESGA64[Blood],   // Diamond
    ENDESGA64[Black0],  // Clover
    ENDESGA64[Orange1], // Tarot
    ENDESGA64[Blood],   // Dagger
    ENDESGA64[Black0],  // Mirror
    ENDESGA64[Green1],  // Wealth
};

class Card {
public:
  Card() = default;
  constexpr Card(Suit s, uint8_t n) : suit(s), number(n) {}

  Suit suit = Tarot;
  uint8_t number = 0;
  void Draw(Point pos, int32_t shadow_offset);
};

void Card::Draw(Point pos, int32_t shadow_offset = 2) {
  const int32_t sprite_size = 8;
  Size map_size(5, 7);
  Size size(sprite_size * map_size.w, sprite_size * map_size.h);

  // Card Shadow
  sprite_colors[FG] = ENDESGA64[Coffee0];
  sprite_colors[FG].a = 128;
  sprite_colors[BG] = clear;
  draw_map(map_card_filled, pos + Point(shadow_offset, shadow_offset));

  // Card background color
  sprite_colors[FG] = ENDESGA64[Coffee7];
  sprite_colors[BG] = clear;
  draw_map(map_card_filled, pos);

  // Card Border
  sprite_colors[FG] = ENDESGA64[Coffee6];
  sprite_colors[BG] = clear;
  draw_map(map_card_outline, pos);

  screen.pen = Pen(0, 0, 0, 255);

  const int32_t number_glyph_index = 80;
  const int32_t suit_glyph_index = 16;
  Point glyph_pos = pos + Point(sprite_size, sprite_size);

  // Suit
  sprite_colors[FG] = SuitColors[suit];
  screen.sprite(suit_glyph_index + suit, glyph_pos);
  glyph_pos.x += sprite_size;
  int32_t ones_place = number % 10;

  if (suit == Tarot) {
    // No suit icon
    // 0, 1, 2, 3, ..., 19, 20, 21, 22

    glyph_pos.x -= 4;
    if (number >= 20) {
      screen.sprite(number_glyph_index + 2, glyph_pos);
      glyph_pos.x += sprite_size;
    } else if (number >= 10) {
      // Center the 10 a bit more.
      glyph_pos.x += 1;
      screen.sprite(number_glyph_index + 1, glyph_pos);
      glyph_pos.x += 6;
    }
    screen.sprite(number_glyph_index + ones_place, glyph_pos);
    glyph_pos.x += sprite_size;

  } else if (suit >= DragonDagger) {
    // Dragons
  } else {
    // Suit icon
    // 0, 1, 2, 3, ..., 10, J, Q, K, A

    if (number == 10) {
      screen.sprite(number_glyph_index + 10, glyph_pos);
      glyph_pos.x += 5;
      screen.sprite(number_glyph_index + ones_place, glyph_pos);
      glyph_pos.x += sprite_size;
    } else if (number <= 14) {
      screen.sprite(number_glyph_index + number, glyph_pos);
      glyph_pos.x += sprite_size;
    }
  }

  glyph_pos = pos + Point(sprite_size, sprite_size * 2);

  screen.text("123", kCardFont, glyph_pos + Point(0, 0));
  screen.text("456", kCardFont, glyph_pos + Point(0, sprite_size));
  screen.text("789", kCardFont, glyph_pos + Point(0, sprite_size * 2));
  screen.text("#!$", kCardFont, glyph_pos + Point(0, sprite_size * 3));
}

class Stack {
public:
  Point screen_position = Point(0, 0);
  std::deque<Card> cards;

  Stack() = default;
  void ScreenPosition(int32_t x, int32_t y);
  void Draw();
  void AddCard(Card c);
  void DrawHolder();
  Card BottomCard();
};

void Stack::DrawHolder() {
  sprite_colors[FG] = ENDESGA64[Ocean2];
  sprite_colors[BG] = clear;
  draw_map(map_holder, screen_position);
  // Holder background
  sprite_colors[FG] = ENDESGA64[Ocean1];
  sprite_colors[BG] = ENDESGA64[Ocean2];
  draw_map(map_holder_background, screen_position);
}

void Stack::Draw() {
  DrawHolder();

  Point pos = screen_position;
  // pos.x = 100;
  for (auto &c : cards) {
    c.Draw(pos);
    pos.y += 14;
    // pos.x -= 8;
  }
}

void Stack::ScreenPosition(int32_t x, int32_t y) {
  screen_position.x = x;
  screen_position.y = y;
}

void Stack::AddCard(Card c) { cards.push_back(std::move(c)); }

Card Stack::BottomCard() { return cards.back(); }

} // namespace Solitaire

void draw_card(int x, int y, bool holder = false) {}

using namespace Solitaire;

Stack stack1;

void init() {
  blit::set_screen_mode(ScreenMode::hires);

  screen.sprites = Surface::load(asset_sprite_sheet);
  screen.sprites->palette = sprite_colors;

  map_card_outline = TileMap::load_tmx((uint8_t *)asset_map_cards,
                                       screen.sprites, Layer_CardOutline);
  map_card_filled = TileMap::load_tmx((uint8_t *)asset_map_cards,
                                      screen.sprites, Layer_CardFilled);
  map_holder = TileMap::load_tmx((uint8_t *)asset_map_cards, screen.sprites,
                                 Layer_HolderOutline);
  map_holder_background = TileMap::load_tmx(
      (uint8_t *)asset_map_cards, screen.sprites, Layer_HolderBackground);

  stack1.ScreenPosition(0, 0);
  stack1.AddCard(Card(Spade, 14));
  stack1.AddCard(Card(Spade, 13));
  stack1.AddCard(Card(Spade, 12));
  stack1.AddCard(Card(Spade, 11));
  stack1.AddCard(Card(Heart, 10));
  stack1.AddCard(Card(Diamond, 9));
  stack1.AddCard(Card(Clover, 1));
  stack1.AddCard(Card(Clover, 0));
  stack1.AddCard(Card(Tarot, 22));
  stack1.AddCard(Card(Tarot, 10));
  stack1.AddCard(Card(DragonDagger, 0));
  stack1.AddCard(Card(DragonMirror, 1));
  stack1.AddCard(Card(DragonWealth, 2));
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

  stack1.Draw();
}

void update(uint32_t time) {
  // Empty
}
