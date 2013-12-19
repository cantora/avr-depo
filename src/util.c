#include "avr-depo.h"

void debug_print_n(uint32_t n, int radix) {
  char buf[16];
  char *result;

  /* buf[16] will hold decimal and hex 32 bit int
   * but wont hold a binary conversion */
  if(radix != 10 && radix != 16)
    return; 

  result = ultoa(n, buf, radix);
  if(result) {
    ADP_debug_print(result);
    ADP_debug_print("\r\n");
  }
}
