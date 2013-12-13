#include "depo_main.h"

#include <stdint.h>
#include <stdlib.h>

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

static const uint32_t PASS_CHAR_OFFSET = 0x20;
static const uint32_t PASS_CHAR_RANGE = 0x80-2-0x20;

void debug_print_n(uint32_t n, int radix) {
  char buf[16];
  char *result;

  result = ultoa(n, buf, radix);
  if(result) {
    ADP_debug_print(result);
    ADP_debug_print("\r\n");
  }
}

static int password(uint16_t col, uint16_t row, uint8_t *key) {
  char pass[DEPO_MAX_PASSLEN];
  int i, j;
  uint8_t pos0, pos1, btn;
  uint16_t col_space;

  col_space = g_cols - col;
  if(col_space < 2)
    return -1;

  pos0 = 0x61; /* a */
  ADP_selector_set(PASS_CHAR_RANGE*1000UL + (pos0 - PASS_CHAR_OFFSET));
  btn = 0;

  ADP_display_cursor_set(col, row);
  ADP_display_write(pos0);

  for(i = 0; i < DEPO_MAX_PASSLEN; i++) {
    while(1) {
      btn = ADP_button_check();
      pos1 = PASS_CHAR_OFFSET + (ADP_selector_position() % PASS_CHAR_RANGE);
      if(pos1 != pos0) {
        pos0 = pos1;
        ADP_display_cursor_set(col, row);

        if(i > 1)
          for(j = 0; j < i-1; j++) {
            if(j >= col_space - 2)
              break;
            ADP_display_write('*');
          }
        if(i > 0)
          ADP_display_write(pass[i-1]);

        ADP_display_write(pos0);
        debug_print_n(pos0, 16);
      }
      if(btn != 0) {
        pass[i] = pos0;
        while(ADP_button_check() != 0);
        break;
      }
    }
  }

  return 0;
}

void depo_main() {
  uint8_t key[64];

  if(g_aborted != 0)
    return;

  ADP_display_dims(&g_cols, &g_rows);
  if(g_rows < 1 || g_cols < 8) {
    err("dims too small. abort\n");
    return;
  }

  ADP_display_clear();
  display_print(0, 0, "key:");
  if(password(4, 0, key) != 0) {
    err("error in password entry\n");
    return;
  }

  err("unknown failure\n");
}
