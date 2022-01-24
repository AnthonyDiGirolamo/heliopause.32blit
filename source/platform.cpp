#include "platform.hpp"
#include "font_asset.hpp"

namespace heliopause {

PlatformType kCurrentPlatform;

uint8_t planet_pixel_data[PLANET_FRAMEBUFFER_WIDTH * PLANET_FRAMEBUFFER_HEIGHT];

blit::Surface planet_framebuffer((uint8_t *)planet_pixel_data,
                                 blit::PixelFormat::P,
                                 blit::Size(PLANET_FRAMEBUFFER_WIDTH,
                                            PLANET_FRAMEBUFFER_HEIGHT));

const blit::Font kCustomFont(m3x6_font);

} // namespace heliopause
