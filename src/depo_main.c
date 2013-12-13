#include "depo_main.h"

#include <stdint.h>

#define DEPO_MAX_PASSLEN 128
#define DEPO_MIN_ROWS 1
#define DEPO_MIN_COLS 8

static uint8_t g_aborted = 0;
static uint16_t g_rows, g_cols;

static void err(const char *msg) {
  ADP_debug_print(msg);
  g_aborted = 1;
}

static void display_print(uint16_t col, uint16_t row, const char *s) {
  int i;

  ADP_display_cursor_set(col, row);
  for(i = col; i < g_cols; i++){ 
    if(*(s+i) == 0x00)
      break;
    ADP_display_write(*(s+i));
  }
}

static void password(uint16_t col, uint16_t row, uint8_t *key) {
  char pass[DEPO_MAX_PASSLEN];
  int i;
  uint8_t pos0, pos1, btn;

# define PASS_CHAR_OFFSET 0x20
# define PASS_CHAR_RANGE (0x100-1-PASS_CHAR_OFFSET)

  pos0 = 0x61-PASS_CHAR_OFFSET;
  ADP_selector_set(pos0);
  btn = 0;

  ADP_display_cursor_set(col, row);
  ADP_display_cursor(1);
  ADP_display_write(PASS_CHAR_OFFSET + pos0);

  /*
  for(i = 0; i < DEPO_MAX_PASSLEN; i++) {
    while(btn == 0) {
      pos1 = ADP_selector_position() % PASS_CHAR_RANGE;
      btn = ADP_button_check();
      if(pos1 != pos0) {
        ADP_display_cursor_set(col, row);
        ADP_display_write(PASS_CHAR_OFFSET + pos1);
        pos0 = pos1;
      }
      if(btn)
        break;
    }
  }*/

#undef PASS_CHAR_OFFSET
#undef PASS_CHAR_RANGE
}

void depo_main() {
  uint8_t key[64];

  ADP_display_dims(&g_cols, &g_rows);
  if(g_rows < 1 || g_cols < 8) {
    err("dims too small. abort\n");
    return;
  }

  ADP_display_clear();
  display_print(0, 0, "key:");
  password(4, 0, key);
}
