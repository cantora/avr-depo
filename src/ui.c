#include "ui.h"
#include "avr-depo.h"

#include "display.h"
#include "util.h"

static const uint32_t INPUT_CHAR_OFFSET = 0x20;
static const uint32_t INPUT_CHAR_RANGE = 0x80-2-0x20;

static void selector_set(uint32_t val, uint32_t range) {
  ADP_selector_set(range*1000UL + val);
}

static int button_wait(uint8_t state, uint32_t millisecs, uint32_t max) {
  uint32_t start, t, last;

  start = ADP_ts_millis();
  last = start;
  while( (t = ADP_ts_millis()) < (start + max) ) {
    if(ADP_button_check() != state)
      last = t;
    else if(t - last >= millisecs)
      return 0;
  }

  return 1;
}

size_t ui_input(uint16_t col, uint16_t row, uint8_t *data, size_t len, uint8_t opts) {
  size_t i, j;
  uint8_t pos0, pos1, btn, new_char;
  uint16_t col_space, prev_amt, cols, rows;

  display_dims(&cols, &rows);
  col_space = cols - col;
  if(col_space < 2)
    return 0;

  pos0 = 0;
  selector_set(0x61 - INPUT_CHAR_OFFSET, INPUT_CHAR_RANGE);
  btn = 0;

  for(i = 0; i < len; i++) {
    new_char = 1;
    while(1) {
      btn = ADP_button_check();
      pos1 = INPUT_CHAR_OFFSET + (ADP_selector_position() % INPUT_CHAR_RANGE);

      if(pos1 != pos0 || new_char) {
        pos0 = pos1;
        ADP_display_cursor_set(col, row);
        if(i > 0) {
          prev_amt = col_space - 1;
          if(i < prev_amt)
            prev_amt = i;
          for(j = 0; j < (prev_amt-1); j++)
            if(opts & UI_INPUT_OPTS_HIDE)
              ADP_display_write('.');
            else
              ADP_display_write(data[i-prev_amt+j]);
          ADP_display_write(data[i-1]);
        }
        ADP_display_write(pos0);
      }

      if(btn != 0) {
        data[i] = pos0;
        button_wait(0, 200, 1000);
        break;
      }
      new_char = 0;
    }
  }

  return i;
}
