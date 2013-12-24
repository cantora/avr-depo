#include "avr-depo.h"

static void print_n(uint32_t n, int radix) {
  char buf[16];

  /* buf[16] will hold decimal and hex 32 bit int
   * but wont hold a binary conversion */
  if(radix != 10 && radix != 16)
    return; 

  ultoa(n, buf, radix);
  ADP_debug_print(buf);
}

void debug_print_n(uint32_t n, int radix) {
  print_n(n, radix);
  ADP_debug_print("\r\n");
}

/*
void debug_print_hex(uint16_t sz, uint8_t *data) {
  uint16_t i;

  for(i = 0; i < sz; i++) {
    print_n(data[i], 16);
    ADP_debug_print(",");
  }
  ADP_debug_print("\r\n");
}
*/