#include "main.hpp"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>

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

Size card_sprite_size(8, 8);
Size card_map_size(5, 7);
Size card_pixel_size(card_sprite_size.w *card_map_size.w,
                     card_sprite_size.h *card_map_size.h);

enum Suit {
  Spade = 0,
  Heart = 1,
  Diamond = 2,
  Clover = 3,
  Tarot = 4,
  DragonDagger = 5,
  DragonMirror = 6,
  DragonWealth = 7,
  Flower = 8,
};

constexpr uint8_t SuitIcons[]{
    16, // Spade
    17, // Heart
    18, // Diamond
    19, // Clover
    20, // Tarot
    64, // DragonDagger
    65, // DragonMirror
    66, // DragonWealth
    67, // Flower
};

Pen SuitColors[]{
    ENDESGA64[Black0],      // Spade
    ENDESGA64[Blood],       // Heart
    ENDESGA64[Blood],       // Diamond
    ENDESGA64[Black0],      // Clover
    ENDESGA64[Orange1],     // Tarot
    ENDESGA64[Blood],       // Dagger
    ENDESGA64[Black0],      // Mirror
    ENDESGA64[Green1],      // Wealth
    ENDESGA64[CandyGrape1], // Flower
};

class Card {
public:
  Card() = default;
  constexpr Card(Suit s, uint8_t n) : suit(s), number(n) {}

  Suit suit = Tarot;
  uint8_t number = 0;
  void Draw(Point pos, int32_t shadow_offset);
  void DrawSuit(Point pos);
};

void Card::DrawSuit(Point glyph_pos) {
  const int32_t number_glyph_index = 80;
  // Point glyph_pos = pos + ;

  // Suit
  sprite_colors[FG] = SuitColors[suit];
  screen.sprite(SuitIcons[suit], glyph_pos);
  glyph_pos.x += card_sprite_size.w;
  int32_t ones_place = number % 10;

  if (suit == Tarot) {
    // No suit icon
    // 0, 1, 2, 3, ..., 19, 20, 21, 22

    glyph_pos.x -= 4;
    if (number >= 20) {
      screen.sprite(number_glyph_index + 2, glyph_pos);
      glyph_pos.x += card_sprite_size.w;
    } else if (number >= 10) {
      // Center the 10 a bit more.
      glyph_pos.x += 1;
      screen.sprite(number_glyph_index + 1, glyph_pos);
      glyph_pos.x += 6;
    }
    screen.sprite(number_glyph_index + ones_place, glyph_pos);
    glyph_pos.x += card_sprite_size.w;

  } else if (suit >= DragonDagger) {
    // Dragons
  } else {
    // Suit icon
    // 0, 1, 2, 3, ..., 10, J, Q, K, A

    if (number == 10) {
      screen.sprite(number_glyph_index + 10, glyph_pos);
      glyph_pos.x += 5;
      screen.sprite(number_glyph_index + ones_place, glyph_pos);
      glyph_pos.x += card_sprite_size.w;
    } else if (number <= 14) {
      screen.sprite(number_glyph_index + number, glyph_pos);
      glyph_pos.x += card_sprite_size.w;
    }
  }
}

void Card::Draw(Point pos, int32_t shadow_offset = 2) {
  Size size(card_sprite_size.w * card_map_size.w,
            card_sprite_size.h * card_map_size.h);

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

  DrawSuit(pos + Point(card_sprite_size.w - 1, card_sprite_size.h));
  if (suit >= DragonDagger && suit <= Flower) {
    DrawSuit(pos + Point(card_sprite_size.w * 3 + 1, card_sprite_size.h * 5));
  } else if (suit <= Clover) {
    if (number == 10)
      DrawSuit(pos + Point(card_sprite_size.w * 2 - 4, card_sprite_size.h * 5));
    else
      DrawSuit(pos + Point(card_sprite_size.w * 2 + 1, card_sprite_size.h * 5));
  } else {
  }
}

class Stack {
public:
  Point screen_position = Point(0, 0);
  std::deque<Card> cards;
  Size card_draw_offset = Size(0, 14);
  int32_t draw_count = 1;

  Stack() = default;
  Stack(Point position);
  void ScreenPosition(int32_t x, int32_t y);
  void Draw();
  bool AddCard(Card c);
  void DrawHolder();
  Card BottomCard();
  void Clear();
};

Stack::Stack(Point position) { screen_position = position; }

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
    pos.y += card_draw_offset.h;
    // pos.x -= 8;
  }
}

void Stack::ScreenPosition(int32_t x, int32_t y) {
  screen_position.x = x;
  screen_position.y = y;
}

bool Stack::AddCard(Card c) {
  cards.push_back(std::move(c));
  return true;
}

Card Stack::BottomCard() { return cards.back(); }

void Stack::Clear() { cards.clear(); }

// clang-format off
constexpr Card shenzhen_deck[] = {
  Card(DragonDagger, 0),
  Card(DragonDagger, 0),
  Card(DragonDagger, 0),
  Card(DragonDagger, 0),
  Card(DragonMirror, 0),
  Card(DragonMirror, 0),
  Card(DragonMirror, 0),
  Card(DragonMirror, 0),
  Card(DragonWealth, 0),
  Card(DragonWealth, 0),
  Card(DragonWealth, 0),
  Card(DragonWealth, 0),
  Card(Flower, 0),
  Card(Spade, 1),
  Card(Spade, 2),
  Card(Spade, 3),
  Card(Spade, 4),
  Card(Spade, 5),
  Card(Spade, 6),
  Card(Spade, 7),
  Card(Spade, 8),
  Card(Spade, 9),
  Card(Heart, 1),
  Card(Heart, 2),
  Card(Heart, 3),
  Card(Heart, 4),
  Card(Heart, 5),
  Card(Heart, 6),
  Card(Heart, 7),
  Card(Heart, 8),
  Card(Heart, 9),
  Card(Diamond, 1),
  Card(Diamond, 2),
  Card(Diamond, 3),
  Card(Diamond, 4),
  Card(Diamond, 5),
  Card(Diamond, 6),
  Card(Diamond, 7),
  Card(Diamond, 8),
  Card(Diamond, 9),
};
// clang-format on
constexpr std::span<const Card> shenzhen_deck_span(shenzhen_deck);

class Board {
public:
  std::vector<Stack> stacks;
  std::vector<Stack> temp_slots;
  std::vector<Stack> discard_slots;
  std::vector<Card> deal_deck;
  Stack deal_stack;
  uint32_t seed;

  Board();
  void Draw();
  void Shuffle();
};

void Board::Shuffle() {
  // Load base deck
  deal_deck.clear();
  for (auto &card : shenzhen_deck_span) {
    deal_deck.push_back(card);
  }

  // Swap random cards
  for (int i = 0; i < 400; i++) {
    int c1 = Random::GetRandomInteger(deal_deck.size());
    int c2 = Random::GetRandomInteger(deal_deck.size());
    Card temp(std::move(deal_deck[c1]));
    deal_deck[c1] = std::move(deal_deck[c2]);
    deal_deck[c2] = std::move(temp);
  }

  // // Move cards to the deal stack
  // for (auto &c : deal_deck) {
  //   deal_stack.AddCard(c);
  //   deal_deck.pop_back();
  // }

  // Deal cards to each stack
  for (auto &stack : stacks) {
    stack.Clear();
    stack.AddCard(deal_deck.back());
    deal_deck.pop_back();
    stack.AddCard(deal_deck.back());
    deal_deck.pop_back();
    stack.AddCard(deal_deck.back());
    deal_deck.pop_back();
    stack.AddCard(deal_deck.back());
    deal_deck.pop_back();
    stack.AddCard(deal_deck.back());
    deal_deck.pop_back();
  }
}

Board::Board() {
  const int stack_count = 8;
  for (int i = 0; i < stack_count; i++) {
    Stack stack1(Point(i * card_pixel_size.w,
                       card_sprite_size.h * (card_map_size.h + 1)));
    stacks.push_back(std::move(stack1));
  }

  for (int i = stack_count - 3; i < stack_count; i++) {
    Stack discard1(Point(i * card_pixel_size.w, 0));
    discard_slots.push_back(std::move(discard1));
  }

  deal_stack.ScreenPosition((4 * card_pixel_size.w) - card_sprite_size.w, 0);
  deal_stack.card_draw_offset.h = 0;

  for (int i = 0; i < 3; i++) {
    Stack temp1(Point(i * card_pixel_size.w + card_sprite_size.w, 0));
    temp_slots.push_back(std::move(temp1));
  }
}

void Board::Draw() {
  for (auto &s : stacks) {
    s.Draw();
  }
  for (auto &s : discard_slots) {
    s.Draw();
  }
  for (auto &s : temp_slots) {
    s.Draw();
  }
  deal_stack.DrawHolder();
}

} // namespace Solitaire

using namespace Solitaire;

Board game_board;

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

  game_board.Shuffle();
}

void render(uint32_t time_ms) {
  screen.alpha = 255;
  screen.mask = nullptr;
  screen.clear();

  sprite_colors[FG] = ENDESGA64[Ocean0];
  sprite_colors[BG] = ENDESGA64[Ocean1];

  // Reset seed
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

  game_board.Draw();
}

void update(uint32_t time) {
  // Empty
}
