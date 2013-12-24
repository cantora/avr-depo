#ifndef AVR_DEPO_util_h
#define AVR_DEPO_util_h

#include <stdint.h>

void util_msnibble_to_hex(uint8_t b, uint8_t *out);
void util_byte_to_hex(uint8_t b, uint8_t *out);
void debug_print_n(uint32_t n, int radix);

#endif /* AVR_DEPO_util_h */
