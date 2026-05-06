#include "palette.h"

uint32_t bswap32(uint32_t a)
{
    return ((a & 0x000000FF) << 24) |
           ((a & 0x0000FF00) << 8) |
           ((a & 0x00FF0000) >> 8) |
           ((a & 0xFF000000) >> 24);
}

std::vector<uint32_t> color_ramp = {
    // Azul puro
    bswap32(0x21ED4A),
    bswap32(0x1EDD45),
    bswap32(0x1CCD40),
    bswap32(0x1ABD3B),
    bswap32(0x18AD36),
    bswap32(0x169E31),
    bswap32(0x138E2C),
    bswap32(0x117E27),
    bswap32(0x0F6E22),
    bswap32(0x0D5E1D),
    bswap32(0x0B4F18),
    bswap32(0x083F13),
    bswap32(0x062F0E),
    bswap32(0x041F09),
    bswap32(0x020F04)};