#include "avr-depo.h"

void debug_print_n(uint32_t n, int radix) {
  char buf[16];
  char *result;

  result = ultoa(n, buf, radix);
  if(result) {
    ADP_debug_print(result);
    ADP_debug_print("\r\n");
  }
}
