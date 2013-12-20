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

static void generate_update(uint16_t bits, void *user) {
  static uint8_t state = 0;
  float percent;
  int i;
  (void)(user);

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
                   KEYLEN, key, generate_update,
                   500, user) != 0)
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

static int do_action_gen(const uint8_t *seed, uint16_t seedlen,
                         schema_id sid, uint16_t len) {
  struct schema sch;

  if(schema_init(&sch, sid, len) != 0)
    return -1;

  //if(crypto_gen(seed, seedlen, &sch, 
}

static int action_gen(const uint8_t *key) {
  uint8_t buf[128];
  uint16_t buf_len, buf_len2;
  uint16_t iteration;

  ADP_display_clear();
  display_print(0, 0, "alias:");
  buf_len = ui_input(6, 0, buf, 63, 0);
  if(buf_len > 63) /* overflow paranoia ;) */
    return -1;

  while(1) {
    ADP_display_clear();
    display_print(0, 0, "name:");
    buf_len2 = ui_input(5, 0, buf+buf_len, 63, 0);
    if(buf_len2 >= 3)
      break;
   
    ADP_display_clear();
    display_print_chunk(0, 0, "name < 3 chars");
    ADP_delay(1500);
  }
  if(buf_len2 > 63)
    return -1;
  buf_len += buf_len2;

  ADP_display_clear();
  display_print(0, 0, "n:");
  iteration = (uint16_t) ui_input_n(2, 0, 1, 29999, 1);
#ifndef __BYTE_ORDER__
#  error expected __BYTE_ORDER__ to be defined!
#endif
  /* we store the iteration in big endian format */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  buf[buf_len++] = (iteration >> 8) && 0xff;
  buf[buf_len++] = iteration && 0xff;
#else
  buf[buf_len++] = iteration && 0xff;
  buf[buf_len++] = (iteration >> 8) && 0xff;
#endif

  if(buf_len > sizeof(buf))
    return -1;

  return do_action_gen(buf, buflen, SCHEMA_ID_HEX, 8);
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
