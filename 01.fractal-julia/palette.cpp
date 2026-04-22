#include "palette.h"

uint32_t bswap32(uint32_t a)
{
    return ((a & 0x000000FF) << 24) |
           ((a & 0x0000FF00) << 8) |
           ((a & 0x00FF0000) >> 8) |
           ((a & 0xFF000000) >> 24);
}

std::vector<uint32_t> color_ramp = {
    bswap32(0xFF0000FF), // Rojo puro
    bswap32(0xEE0011FF),
    bswap32(0xDD0022FF),
    bswap32(0xCC0033FF),
    bswap32(0xBB0044FF),
    bswap32(0xAA0055FF),
    bswap32(0x990066FF),
    bswap32(0x880077FF),
    bswap32(0x770088FF),
    bswap32(0x660099FF),
    bswap32(0x5500AAFF),
    bswap32(0x4400BBFF),
    bswap32(0x3300CCFF),
    bswap32(0x2200DDFF),
    bswap32(0x1100EEFF),
    bswap32(0x0000FFFF) // Azul puro
};