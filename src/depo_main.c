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

static void warn(const char *msg) {
  ADP_debug_print(msg);
  ADP_debug_print("\r\n");
  ADP_display_clear();
  display_print_chunk(0, 0, msg);
  ADP_delay(1500);
}

static void err(const char *msg) {
  warn(msg);
  g_aborted = 1;
}

static int generate_key(uint8_t *key) {
  uint8_t pass[PASS_MAX_LEN];
  size_t len;
  struct ui_processing proc;

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

  ui_processing_init(&proc, KEYLEN << 3);
  ADP_display_clear();
  if(crypto_pbkdf2(pass, (uint16_t) len,
                   (const uint8_t *) AVR_DEPO_config_pbkdf2_salt,
                   AVR_DEPO_config_pbkdf2_saltlen,
                   AVR_DEPO_config_pbkdf2_rounds,
                   KEYLEN, key, ui_processing_update,
                   500, &proc) != 0)
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

static void action_gen_update(void *user) {
  ui_processing_update(0, user);
}

static int action_gen(const uint8_t *key) {
#define DEPO_ACTION_GEN_ITER_MIN 1
#define DEPO_ACTION_GEN_SIZE_MIN 6
#define DEPO_ACTION_GEN_SIZE_DEFAULT 12
  const char *yesno[] = {"no", "yes"};
  uint8_t buf[128];
  uint8_t *result;
  uint16_t buf_len, buf_len2, do_options, iteration, rsize;
  int status;
  struct schema sch;
  struct ui_processing proc;
  schema_id sid;

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
  display_print(0, 0, "opts?:");
  
  do_options = ui_option(6, 0, yesno, 2);
  if(do_options != 0) {
    ADP_display_clear();
    display_print(0, 0, "scma:");
    sid = ui_option(5, 0, schema_names, SCHEMA_COUNT);

    ADP_display_clear();
    display_print(0, 0, "sz:");
    rsize = (uint16_t) ui_input_n(3, 0,
                                  DEPO_ACTION_GEN_SIZE_MIN,
                                  96,
                                  DEPO_ACTION_GEN_SIZE_DEFAULT);
    ADP_display_clear();
    display_print(0, 0, "n:");
    iteration = (uint16_t) ui_input_n(2, 0, 
                                      DEPO_ACTION_GEN_ITER_MIN,
                                      29999,
                                      DEPO_ACTION_GEN_ITER_MIN);
  }
  else {
    sid = SCHEMA_ID_PW0;
    rsize = DEPO_ACTION_GEN_SIZE_DEFAULT;
    iteration = DEPO_ACTION_GEN_ITER_MIN;
  }

  /* some sanity checks for extra security */
  if(buf_len > sizeof(buf) || rsize < 1)
    return -1;

  /* we write the iteration in big endian format */
  buf[buf_len++] = iteration/256;
  buf[buf_len++] = iteration % 256;

  schema_init(&sch, sid, rsize,
                 buf, buf_len, action_gen_update,
                 500, &proc);

  result = malloc(rsize*sizeof(uint8_t));
  if(result == NULL)
    return -1;

  ui_processing_init(&proc, 0);
  status = 0;
  if(schema_run(&sch, result) != 0) {
    status = -1;
    goto done;
  }

  ADP_debug_nprint(rsize, (const char *) result);
  ADP_debug_print("\n\r");
  ADP_display_clear();
  ui_print_scroll(0, 0, rsize, (const char *) result);

done:
  free(result);
  return status;
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
      if(action_gen(key) != 0)
        warn("unknown failure in action gen");
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

static void lcd_char_test() {
  char chs[256];
  int i;

  for(i = 0; i < sizeof(chs); i++)
    chs[i] = i;

  ui_print_scroll(0, 0, sizeof(chs), chs);
}

void depo_main() {
  uint8_t key[KEYLEN];

  if(g_aborted != 0)
    return;

  if(check_display_dims() != 0) {
    err("dims too small. abort");
    return;
  }

  //lcd_char_test();
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
