#include "ui.h"
#include "avr-depo.h"

#include "display.h"
#include "util.h"

static const uint32_t INPUT_CHAR_OFFSET = 0x20;
static const uint32_t INPUT_CHAR_RANGE = 0x80-2-0x20;

static void selector_set(uint32_t val, uint32_t range) {
  ADP_selector_set(range*1000UL + val);
}

typedef enum {
  UI_BTN_STATE_OFF = 0,
  UI_BTN_STATE_TRIGGER,
  UI_BTN_STATE_HOLDING_A,
  UI_BTN_STATE_HOLDING_B,
  UI_BTN_STATE_RELEASED
} ui_btn_state;

static struct {
  uint32_t t;
  uint16_t triggers;
  ui_btn_state state;
} g_btn;


static void btn_state_init() {
  g_btn.state = UI_BTN_STATE_OFF;
  g_btn.t = ADP_ts_millis();
  g_btn.triggers = 0;
}

#define UI_BTN_EDGE_THRESH 30 /* milliseconds */

static inline int btn_state_holding() {
  return (g_btn.state == UI_BTN_STATE_HOLDING_A
          || g_btn.state == UI_BTN_STATE_HOLDING_B);
}

static inline int btn_state_released() {
  return (g_btn.state == UI_BTN_STATE_RELEASED);
}

static void btn_state_update() {
  uint8_t val;

  if(btn_state_released())
    return;

  val = ADP_button_check();

  switch(g_btn.state) {
  case UI_BTN_STATE_OFF:
    if(val != 0) {
      g_btn.t = ADP_ts_millis();
      g_btn.state = UI_BTN_STATE_TRIGGER;
    }
    break;
  case UI_BTN_STATE_TRIGGER:
    if(val == 0)
      g_btn.state = UI_BTN_STATE_HOLDING_B;
    else
      if(ADP_ts_millis() - g_btn.t >= UI_BTN_EDGE_THRESH)
        g_btn.state = UI_BTN_STATE_HOLDING_A;
    break;
  case UI_BTN_STATE_HOLDING_A:
    if(val == 0) {
      g_btn.state = UI_BTN_STATE_HOLDING_B;
      g_btn.t = ADP_ts_millis();
    }
    break;
  case UI_BTN_STATE_HOLDING_B:
    if(val != 0) {
      if(g_btn.t < UI_BTN_EDGE_THRESH)
        g_btn.state = UI_BTN_STATE_HOLDING_A;
      else {
        g_btn.state = UI_BTN_STATE_TRIGGER;
        g_btn.t = ADP_ts_millis();
      }
    }
    else if(ADP_ts_millis() - g_btn.t >= 100) {
      g_btn.state = UI_BTN_STATE_RELEASED;
    }
    break;
  default:
    ;/* stick in this state until manual reset */
  }
}

static inline void print_input(uint8_t *data, uint8_t len,
                               uint16_t col_space, uint8_t opts) {
  int j;
  uint16_t prev_amt;

  if(len > 0) {
    prev_amt = col_space - 1;
    if(len < prev_amt)
      prev_amt = len;
    for(j = 0; j < (prev_amt-1); j++)
      if(opts & UI_INPUT_OPTS_HIDE)
        ADP_display_write('.');
      else
        ADP_display_write(data[len-prev_amt+j]);
    ADP_display_write(data[len-1]);
  }
}

size_t ui_input(uint16_t col, uint16_t row, /* starting col,row of interaction */
                uint8_t *data, size_t len,  /* buffer to write user input into */
                uint8_t opts) {             /* options, see enum */
  size_t i;
  uint8_t pos0, pos1, refresh, terminal_ch, ok;
  uint16_t col_space, cols, rows;

  display_dims(&cols, &rows);
  col_space = cols - col;
  if(col_space < 2)
    return 0;

  ok = ADP_display_ok_char();
  terminal_ch = ADP_display_input_terminal_char();
  pos0 = 0;
  selector_set(0x61 - INPUT_CHAR_OFFSET, INPUT_CHAR_RANGE);
  btn_state_init();

  for(i = 0; i < len; i++) {
    refresh = 1;
    while(1) {
      btn_state_update();
      pos1 = INPUT_CHAR_OFFSET + (ADP_selector_position() % INPUT_CHAR_RANGE);

      if(pos1 != pos0 || refresh) {
        pos0 = pos1;
        ADP_display_cursor_set(col, row);
        print_input(data, i, col_space, opts);
        ADP_display_write((pos0 == terminal_ch)? ok : pos0);
      }

      if(btn_state_released()) {
        btn_state_init();
        if(pos0 == terminal_ch)
          goto done;
        data[i] = pos0;
        break;
      }
      refresh = 0;
    }
  }

done:
  return i;
}
