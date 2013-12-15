#include "depo_main.h"

#include "util.h"
#include "display.h"
#include "ui.h"

#include <stdint.h>
#include <stdlib.h>

#define DEPO_MIN_ROWS 1
#define DEPO_MIN_COLS 8

static uint8_t g_aborted = 0;

static void err(const char *msg) {
  ADP_debug_print(msg);
  g_aborted = 1;
}

void depo_main() {
  uint8_t key[64];
  uint16_t rows, cols;

  if(g_aborted != 0)
    return;

  display_dims(&cols, &rows);
  if(rows < 1 || cols < 8) {
    err("dims too small. abort\n");
    return;
  }

  ADP_display_clear();
  display_print(0, 0, "key:");
  if(ui_input(4, 0, key, sizeof(key), UI_INPUT_OPTS_HIDE) != 0) {
    err("error in password entry\n");
    return;
  }

  err("unknown failure\n");
}
