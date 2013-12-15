#include "avr-depo.h"

inline void display_dims(uint16_t *cols, uint16_t *rows) {
  ADP_display_dims(cols, rows);
}

void display_print(uint16_t col, uint16_t row, const char *s) {
  int i;
  uint16_t cols, rows;

  display_dims(&cols, &rows);
  ADP_display_cursor_set(col, row);
  for(i = col; i < cols; i++){
    if(*(s+i) == 0x00)
      break;
    ADP_display_write(*(s+i));
  }
}
