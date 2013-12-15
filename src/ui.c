#include "avr-depo.h"

#include "display.h"
#include "util.h"

static const uint32_t PASS_CHAR_OFFSET = 0x20;
static const uint32_t PASS_CHAR_RANGE = 0x80-2-0x20;

#define DEPO_MAX_PASSLEN 128

int password(uint16_t col, uint16_t row, uint8_t *key) {
  char pass[DEPO_MAX_PASSLEN];
  int i, j;
  uint8_t pos0, pos1, btn, new_char;
  uint16_t col_space, prev_amt, cols, rows;

  display_dims(&cols, &rows);
  col_space = cols - col;
  if(col_space < 2)
    return -1;

  pos0 = 0;
  ADP_selector_set(PASS_CHAR_RANGE*1000UL + (0x61 - PASS_CHAR_OFFSET));
  btn = 0;

  for(i = 0; i < DEPO_MAX_PASSLEN; i++) {
    new_char = 1;
    while(1) {
      btn = ADP_button_check();
      pos1 = PASS_CHAR_OFFSET + (ADP_selector_position() % PASS_CHAR_RANGE);

      if(pos1 != pos0 || new_char) {
        pos0 = pos1;
        ADP_display_cursor_set(col, row);
        if(i > 0) {
          prev_amt = col_space - 1;
          if(i < prev_amt)
            prev_amt = i;
          for(j = 0; j < (prev_amt-1); j++)
            ADP_display_write('.');
          ADP_display_write(pass[i-1]);
        }
        ADP_display_write(pos0);
      }

      if(btn != 0) {
        pass[i] = pos0;
        for(j = 0; j < 3; ) {
          if(ADP_button_check() == 0)
            j++;
          ADP_delay(50);
        }
        break;
      }
      new_char = 0;
    }
  }

  return 0;
}
