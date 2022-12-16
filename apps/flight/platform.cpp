#include "platform.hpp"
#include "font_asset.hpp"

#ifdef PICO_ON_DEVICE
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#endif

namespace heliopause {

PlatformType kCurrentPlatform;

// const blit::Font kCustomFont(m3x6_font);
// const blit::Font kCustomFont(m6x11_font);
// const blit::Font kCustomFont(mrmotext);
const blit::Font kCustomFont(kitchensink);

} // namespace heliopause
