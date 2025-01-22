#include <stdint.h>

uint32_t bits_revert(uint32_t num, uint8_t bits)
{
    uint32_t rev = 0;

    for (uint32_t i = 0; i < (bits >> 1); i++) {
        if (num & (1 << i)) {
            rev |= (1 << (bits - 1 - i));
        }
        if ((num >> (bits - 1 - i)) & 1) {
            rev |= 1 << i;
        }
    }

    return rev;
}
