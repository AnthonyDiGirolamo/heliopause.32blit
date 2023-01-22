#include "main.hpp"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>

#include "32blit.hpp"
#include "pw_random/random.h"
#include "pw_random/xor_shift.h"
#include "pw_string/string_builder.h"

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
  constexpr Card(Suit s, uint8_t n)
      : suit(s), number(n), screen_position(Point(0, 0)) {}
  constexpr Card(Suit s, uint8_t n, Point pos)
      : suit(s), number(n), screen_position(pos) {}

  Suit suit = Tarot;
  uint8_t number = 0;
  Point screen_position;
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
  uint8_t animating = 0;
  uint8_t card_limit = 0;

  Stack() = default;
  Stack(Point position);
  void ScreenPosition(int32_t x, int32_t y);
  void Draw();
  bool UpdateCardPositions(float delta_seconds);
  bool AddCard(Card c);
  void DrawHolder();
  Card BottomCard();
  void Clear();
};

Stack::Stack(Point position) { screen_position = position; }

void Stack::DrawHolder() {
  // Outline
  sprite_colors[FG] = ENDESGA64[Ocean2];
  sprite_colors[BG] = clear;
  draw_map(map_holder, screen_position);
  // Background
  sprite_colors[FG] = ENDESGA64[Ocean1];
  sprite_colors[BG] = ENDESGA64[Ocean2];
  draw_map(map_holder_background, screen_position);
}

void traverse_line(Point *location, int x2, int y2, int amount) {
  int x1 = location->x;
  int y1 = location->y;
  int pixels_traversed = 0;

  // Bresenham's Line Algorithm
  int16_t steep_gradient = abs(y2 - y1) > abs(x2 - x1);
  // Swap values
  int16_t temp;
  if (steep_gradient) {
    temp = x1;
    x1 = y1;
    y1 = temp;
    temp = x2;
    x2 = y2;
    y2 = temp;
  }
  if (x1 > x2) {
    amount = abs(x2 - x1) - (amount / 2);
    temp = x1;
    x1 = x2;
    x2 = temp;
    temp = y1;
    y1 = y2;
    y2 = temp;
  }

  int16_t dx = x2 - x1;
  int16_t dy = abs(y2 - y1);
  int16_t error_value = dx / 2;
  int16_t ystep = y1 < y2 ? 1 : -1;

  for (; x1 <= x2; x1++) {
    if (steep_gradient) {
      location->x = y1;
      location->y = x1;
      // screen.pixel(location);
      // fb->SetPixel(y1, x1);
    } else {
      location->x = x1;
      location->y = y1;
      // fb->SetPixel(x1, y1);
      // screen.pixel(location);
    }
    error_value -= dy;
    if (error_value < 0) {
      y1 += ystep;
      error_value += dx;
    }

    pixels_traversed++;
    if (pixels_traversed >= amount) {
      break;
    }
  }
}

bool Stack::UpdateCardPositions(float delta_seconds) {
  if (animating == 0) {
    return false;
  }

  Point destination_position = screen_position;
  for (auto &c : cards) {
    if (c.screen_position != destination_position) {
      // Vec2 start_pos(c.screen_position.x, c.screen_position.y);
      // Vec2 end_pos(destination_position.x, destination_position.y);
      // Vec2 move_vec = end_pos;
      // move_vec -= start_pos;
      // float move_len = move_vec.length();

      // if (move_len < 0)
      //   move_len *= -1;
      // if (move_len <= 2) {
      //   c.screen_position = destination_position;
      // }

      Point diff = destination_position - c.screen_position;
      if (diff.x < 0)
        diff.x *= -1;
      if (diff.y < 0)
        diff.y *= -1;
      if (diff.x >= 0 && diff.x <= 4 && diff.y >= 0 && diff.y <= 4) {
        c.screen_position = destination_position;
      } else {
        Point new_pos = Point(c.screen_position.x, c.screen_position.y);
        traverse_line(&new_pos, destination_position.x, destination_position.y,
                      8);
        c.screen_position.x = new_pos.x;
        c.screen_position.y = new_pos.y;
      }
      // one card at a time
      return true;
    }

    destination_position.y += card_draw_offset.h;
  }
  animating = 0;
  return false;
}

void Stack::Draw() {
  Point pos = screen_position;
  // pos.x = 100;
  for (auto &c : cards) {
    c.Draw(c.screen_position);
    pos.y += card_draw_offset.h;
    // pos.x -= 8;
  }
}

void Stack::ScreenPosition(int32_t x, int32_t y) {
  screen_position.x = x;
  screen_position.y = y;
}

bool Stack::AddCard(Card c) {
  if (card_limit > 0) {
    if (cards.size() >= card_limit) {
      return false;
    }
  }

  cards.push_back(std::move(c));
  animating = 1;
  return true;
}

Card Stack::BottomCard() { return cards.back(); }

void Stack::Clear() { cards.clear(); }

// clang-format off
constexpr Card shenzhen_deck[] = {
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
};
// clang-format on
constexpr std::span<const Card> shenzhen_deck_span(shenzhen_deck);

enum BoardState {
  WAITING,
  ANIMATING,
};

class Board {
public:
  std::vector<Stack> stacks;
  std::vector<Stack> temp_slots;
  std::vector<Stack> discard_slots;
  std::vector<Card> deal_deck;
  Stack deal_stack;
  uint32_t seed;
  BoardState state;

  Board();
  void Draw();
  void Update(float delta_seconds);
  void Shuffle();
};

void Board::Shuffle() {
  // Load base deck
  deal_deck.clear();
  for (auto &card : shenzhen_deck_span) {
    deal_deck.emplace_back(
        Card(card.suit, card.number, deal_stack.screen_position));
  }

  // Swap random cards
  for (size_t i = 0; i < deal_deck.size(); i++) {
    // int r1 = Random::GetRandomInteger(deal_deck.size());
    int r2 = Random::GetRandomInteger(deal_deck.size());
    Card temp(std::move(deal_deck[i]));
    deal_deck[i] = std::move(deal_deck[r2]);
    deal_deck[r2] = std::move(temp);
  }

  // // Move cards to the deal stack
  // for (auto &c : deal_deck) {
  //   // c.screen_position = deal_stack.screen_position;
  //   deal_stack.AddCard(c);
  //   deal_deck.pop_back();
  // }

  // Clear each stack
  for (auto &stack : stacks) {
    stack.Clear();
  }

  // Deal cards to each stack
  for (size_t i = 0; i < 5; i++) {
    for (auto &stack : stacks) {
      stack.AddCard(deal_deck.back());
      deal_deck.pop_back();
    }
  }

  // for (auto &stack : stacks) {
  //   printf("\nSP: %d, %d\n", stack.screen_position.x,
  //   stack.screen_position.y); Point destination_position =
  //   stack.screen_position; for (auto &card : stack.cards) {
  //     printf("CP: %d, %d\n", card.screen_position.x, card.screen_position.y);
  //     printf("dest: %d, %d\n", destination_position.x,
  //     destination_position.y); Point diff = destination_position -
  //     card.screen_position; printf("diff: %d, %d\n", diff.x, diff.y);
  //     destination_position.y += stack.card_draw_offset.h;
  //   }
  // }
}

Board::Board() {
  state = ANIMATING;

  // Position Stacks
  const int stack_count = 8;
  for (int i = 0; i < stack_count; i++) {
    Stack stack1(Point(i * card_pixel_size.w,
                       card_sprite_size.h * (card_map_size.h + 1)));
    stacks.push_back(std::move(stack1));
  }

  for (int i = stack_count - 3; i < stack_count; i++) {
    Stack discard1(Point(i * card_pixel_size.w, 0));
    discard1.card_draw_offset.h = 0;
    discard_slots.push_back(std::move(discard1));
  }

  deal_stack.ScreenPosition((4 * card_pixel_size.w) - card_sprite_size.w, 0);
  deal_stack.ScreenPosition(140, 0);
  deal_stack.card_draw_offset.h = 0;

  for (int i = 0; i < 3; i++) {
    Stack temp1(Point(i * card_pixel_size.w, 0));
    temp_slots.push_back(std::move(temp1));
  }
}

void Board::Update(float delta_seconds) {
  bool card_updated = false;

  if (!card_updated) {
    for (auto &s : stacks) {
      card_updated = s.UpdateCardPositions(delta_seconds);
      if (card_updated)
        break;
    }
  }

  if (!card_updated) {
    for (auto &s : discard_slots) {
      card_updated = s.UpdateCardPositions(delta_seconds);
      if (card_updated)
        break;
    }
  }

  if (!card_updated) {
    for (auto &s : temp_slots) {
      card_updated = s.UpdateCardPositions(delta_seconds);
      if (card_updated)
        break;
    }
  }

  uint8_t animating_count = 0;

  for (auto &s : stacks) {
    animating_count += s.animating;
  }
  for (auto &s : discard_slots) {
    animating_count += s.animating;
  }
  for (auto &s : temp_slots) {
    animating_count += s.animating;
  }

  if (animating_count > 0) {
    state = ANIMATING;
  } else {
    state = WAITING;
  }
}

void Board::Draw() {
  deal_stack.DrawHolder();
  for (auto &s : stacks) {
    s.DrawHolder();
  }
  for (auto &s : discard_slots) {
    s.DrawHolder();
  }
  for (auto &s : temp_slots) {
    s.DrawHolder();
  }

  for (auto &s : stacks) {
    s.Draw();
  }
  for (auto &s : discard_slots) {
    s.Draw();
  }
  for (auto &s : temp_slots) {
    s.Draw();
  }
}

} // namespace Solitaire

using namespace Solitaire;

Board game_board;
uint32_t last_update_time = 0;
uint32_t last_render_time = 0;

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

pw::StringBuffer<64> board_state;

void render(uint32_t time_ms) {
  // float delta_seconds = (time_ms - last_render_time) / 1000.0f;

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

  screen.pen = Pen(255, 255, 255);
  blit::screen.text(board_state.view(), minimal_font,
                    blit::Point(2, blit::screen.bounds.h - 8),
                    false // variable width?
  );
  last_render_time = time_ms;
}

uint32_t animation_done = 0;

void update(uint32_t time) {
  float delta_seconds = (time - last_update_time) / 1000.0f;
  game_board.Update(delta_seconds);

  board_state.clear();
  board_state.Format("Animating: %d", game_board.state);
  last_update_time = time;

  if (game_board.state == WAITING) {
    if (animation_done == 0) {
      animation_done = time;
    } else if (time > animation_done + 3000) {
      animation_done = 0;
      game_board.Shuffle();
    }
  }
}
