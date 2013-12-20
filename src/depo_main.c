#include "depo_main.h"

#include "util.h"
#include "display.h"
#include "ui.h"
#include "crypto.h"
#include "avr-depo-config.h"

#include <stdint.h>
#include <stdlib.h>

static const uint8_t MIN_ROWS = 1;
static const uint8_t MIN_COLS = 8;
static const uint8_t PASS_MAX_LEN = 128;
static const uint8_t KEYLEN = 20;


static uint8_t g_aborted = 0;

static void err(const char *msg) {
  ADP_debug_print(msg);
  ADP_debug_print("\r\n");
  g_aborted = 1;
}

static void generate_update(uint16_t bits) {
  static uint8_t state = 0;
  float percent;
  int i;

  ADP_display_clear();
  ADP_display_cursor_set(0, 0);

  for(i = 0; i <= state; i++)  
    ADP_display_write('>');
  for(i = 0; i < 2-state; i++)
    ADP_display_write(' ');

  percent = bits/((float) (KEYLEN << 3));
  display_print_n(4, 0, percent*100, 10);
  ADP_display_write('%');

  state = (state+1)%3;
}

static int generate_key(uint8_t *key) {
  uint8_t pass[PASS_MAX_LEN];
  size_t len;

  while(1) {
    ADP_display_clear();
    display_print(0, 0, "key:");
    len = ui_input(4, 0, pass, sizeof(pass), UI_INPUT_OPTS_HIDE);
    if(len < 10) {
      ADP_display_clear();
      display_print_chunk(0, 0, "pass < 10 chars");
      ADP_delay(1500);
      continue;
    }

    break;
  }

  ADP_display_clear();
  if(crypto_pbkdf2((const char *) pass, (uint16_t) len,
                   (const uint8_t *) AVR_DEPO_config_pbkdf2_salt,
                   AVR_DEPO_config_pbkdf2_saltlen,
                   AVR_DEPO_config_pbkdf2_rounds,
                   KEYLEN, key,
                   generate_update, 500) != 0)
    return -1;

  return 0;
}

static int check_display_dims() {
  uint16_t rows, cols;

  display_dims(&cols, &rows);
  if(rows < 1 || cols < 8)
    return 1;

  return 0;
}

static void action_gen(const uint8_t *key) {
  uint8_t alias[64];
  uint8_t name[64];
  uint16_t iteration;
  size_t alias_len, name_len;

  ADP_display_clear();
  display_print(0, 0, "alias:");
  alias_len = ui_input(6, 0, alias, sizeof(alias), 0);

  ADP_display_clear();
  display_print(0, 0, "name:");
  name_len = ui_input(5, 0, name, sizeof(name), 0);

  ADP_display_clear();
  display_print(0, 0, "n:");
  iteration = ui_input_n(2, 0, 1, 99, 1);

  ADP_delay(2000);
}

static uint8_t depo_loop(const uint8_t *key) {
  uint8_t choice;
  const char *actions[] = {
#define DEPO_ACTION_GEN 0
    "gen",
#define DEPO_ACTION_SRVR 1
    "srvr",
#define DEPO_ACTION_QUIT 2
    "quit"
  };

  while(1) {
    choice = ui_menu(actions, 3);

    switch(choice) {
    case DEPO_ACTION_GEN:
      action_gen(key);
      break;
    case DEPO_ACTION_SRVR:
      /* not implemented yet */
      break;
    case DEPO_ACTION_QUIT:
      return 0;
    default:
      ADP_debug_print("invalid choice\r\n");
      return -1;
    }
  }
}

void depo_main() {
  uint8_t key[KEYLEN];

  if(g_aborted != 0)
    return;

  if(check_display_dims() != 0) {
    err("dims too small. abort");
    return;
  }

  while(1) {
    if(generate_key(key) != 0) {
      err("error in password entry");
      return;
    }

    if(depo_loop(key) != 0) {
      err("error in depo loop");
      return;
    }
  }

  err("unknown failure");
}
