#include "game.hpp"
#include "engine/engine.hpp"

using namespace blit;

///////////////////////////////////////////////////////////////////////////
//
// init()
//
// setup your game here
//
void init() { set_screen_mode(ScreenMode::hires); }

///////////////////////////////////////////////////////////////////////////
//
// render(time)
//
// This function is called to perform rendering of the game. time is the
// amount if milliseconds elapsed since the start of your game
//
void render(uint32_t time) {

  // clear the screen -- screen is a reference to the frame buffer and can be
  // used to draw all things with the 32blit
  screen.clear();
  // blit::debugf("Hello from 32blit time = %lu - %d,%d\n", time,
  // screen.bounds.w, screen.bounds.h);

  // draw some text at the top of the screen
  screen.alpha = 255;
  screen.mask = nullptr;
  screen.pen = Pen(255, 255, 255);
  screen.rectangle(Rect(0, 0, 320, 14));
  screen.pen = Pen(0, 0, 0);
  screen.text("Hello 32blit!", minimal_font, Point(5, 4));

  int rect_width = 8;

  for (int i = 0; i < 16; i++) {
    screen.pen = PICO8[i];
    screen.rectangle(
        Rect(2 + (i * (rect_width + 1)), 16, rect_width, rect_width));
  }

  int x_offset = 0;
  for (int i = 0; i < 32; i++) {
    screen.pen = ENDESGA32[i];

    int x = (i * (rect_width + 1));
    int y = 25;
    if (i > 15) {
      y = 34;
      x -= x_offset + rect_width + 1;
    } else {
      x_offset = x;
    }
    screen.rectangle(Rect(2 + x, y, rect_width, rect_width));
  }

  screen.pen = Pen(0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////
//
// update(time)
//
// This is called to update your game state. time is the
// amount if milliseconds elapsed since the start of your game
//
void update(uint32_t time) {}
