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

int display_print_chunk(uint16_t col, uint16_t row, const char *s) {
  int i, j, chunks;
  uint16_t cols, rows, space;

  display_dims(&cols, &rows);
  space = cols - col;
  if(space < 1)
    return -1;

  ADP_display_cursor_set(col, row);
  j = 0;
  chunks = 0;
  for(i = 0; s[i] != 0x00; i++) {
    if(j >= space) {
      j = 0;
      chunks++;
      ADP_delay(1000);
      ADP_display_cursor_set(col, row);
    }
    ADP_display_write(s[i]);
    j++;
  }

  if(chunks > 0)
    for(; j < space; j++)
      ADP_display_write(' ');

  return 0;
}
