#include "platform.hpp"
#include "font_asset.hpp"
#include "planet.hpp"

#ifdef PICO_ON_DEVICE
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#endif

namespace heliopause {

PlatformType kCurrentPlatform;

uint8_t planet_pixel_data[PLANET_FRAMEBUFFER_WIDTH * PLANET_FRAMEBUFFER_HEIGHT];
uint8_t
    atmosphere_pixel_data[PLANET_FRAMEBUFFER_WIDTH * PLANET_FRAMEBUFFER_HEIGHT];

blit::Surface planet_framebuffer((uint8_t *)planet_pixel_data,
                                 blit::PixelFormat::P,
                                 blit::Size(PLANET_FRAMEBUFFER_WIDTH,
                                            PLANET_FRAMEBUFFER_HEIGHT));
blit::Surface atmosphere_framebuffer((uint8_t *)atmosphere_pixel_data,
                                     blit::PixelFormat::P,
                                     blit::Size(PLANET_FRAMEBUFFER_WIDTH,
                                                PLANET_FRAMEBUFFER_HEIGHT));

// const blit::Font kCustomFont(m3x6_font);
// const blit::Font kCustomFont(m6x11_font);
// const blit::Font kCustomFont(mrmotext);
const blit::Font kCustomFont(kitchensink);

#ifdef PICO_ON_DEVICE
queue_t call_queue;
queue_t results_queue;

void core1_entry() {
  while (1) {
    // Function pointer is passed to us via the queue_entry_t which also
    // contains the function parameter.
    // We provide an int32_t return value by simply pushing it back on the
    // return queue which also indicates the result is ready.

    queue_entry_t entry;

    queue_remove_blocking(&call_queue, &entry);

    void (*func)() = (void (*)())(entry.func);
    (*func)();
    int32_t result = 1;

    queue_add_blocking(&results_queue, &result);
  }
}
#endif

} // namespace heliopause
