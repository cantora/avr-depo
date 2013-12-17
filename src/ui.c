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

typedef enum {
  UI_BTN_FLAGS_HELD = 1
} ui_btn_flags;

static struct {
  uint32_t t;
  ui_btn_state state;
  uint8_t flags;
} g_btn;


static void btn_state_init() {
  g_btn.state = UI_BTN_STATE_OFF;
  g_btn.t = ADP_ts_millis();
  g_btn.flags = 0;
}

#define UI_BTN_EDGE_THRESH 350 /* milliseconds */

static inline int btn_state_down() {
  return (g_btn.state == UI_BTN_STATE_HOLDING_A
          || g_btn.state == UI_BTN_STATE_HOLDING_B
          || g_btn.state == UI_BTN_STATE_TRIGGER);
}

static inline int btn_state_released() {
  return (g_btn.state == UI_BTN_STATE_RELEASED);
}

static inline uint8_t btn_flag_held() {
  return (g_btn.flags & UI_BTN_FLAGS_HELD);
}

static inline void btn_state_update() {
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
    g_btn.flags |= UI_BTN_FLAGS_HELD;
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
    else if(ADP_ts_millis() - g_btn.t >= 225) {
      g_btn.state = UI_BTN_STATE_RELEASED;
    }
    break;
  default:
    ;/* stick in this state until manual reset */
  }
}

static inline uint16_t print_input(uint8_t *data, uint8_t len,
                               uint16_t col_space, uint8_t opts) {
  int j;
  uint16_t prev_amt;

  if(len < 1)
    return 0;

  prev_amt = col_space;
  if(len < prev_amt)
    prev_amt = len;

  for(j = 0; j < (prev_amt-1); j++)
    if(opts & UI_INPUT_OPTS_HIDE)
      ADP_display_write('.');
    else
      ADP_display_write(data[len-prev_amt+j]);

  ADP_display_write(data[len-1]);
  
  return prev_amt;
}

#define UI_INPUT_DELETE_START_POS 1048576UL

size_t ui_input(uint16_t col, uint16_t row, /* starting col,row of interaction */
                uint8_t *data, size_t len,  /* buffer to write user input into */
                uint8_t opts) {             /* options, see enum */
  size_t i;
  uint8_t pos, tmp_pos, refresh, terminal_ch, ok, delete_state, brk;
  uint16_t col_space, cols, rows, tmp, j;
  uint32_t prev_delete_amt, delete_amt;

  display_dims(&cols, &rows);
  col_space = cols - col;
  if(col_space < 2)
    return 0;

  delete_amt = 0;
  prev_delete_amt = 0;
  delete_state = 0;
  ok = ADP_display_ok_char();
  terminal_ch = ADP_display_input_terminal_char();
  pos = 0;
  selector_set(0x61 - INPUT_CHAR_OFFSET, INPUT_CHAR_RANGE);
  btn_state_init();

  for(i = 0; i < len; i++) {
    refresh = 1;
    brk = 0;
    while(brk == 0) {
      btn_state_update();

      if(btn_state_down()) {
        if( (delete_state & 1) == 0) {
          delete_state = 1;
          ADP_selector_set(UI_INPUT_DELETE_START_POS);
          prev_delete_amt = 0; /* arbitrary non-zero */
        }
        delete_amt = UI_INPUT_DELETE_START_POS - ADP_selector_position();
        if(delete_amt < UI_INPUT_DELETE_START_POS) {
          if(delete_amt > i) {
            delete_amt = i;
            ADP_selector_set(UI_INPUT_DELETE_START_POS - delete_amt);
          }
          if(delete_amt != prev_delete_amt) {
            prev_delete_amt = delete_amt;
            ADP_display_cursor_set(col, row);
            tmp = print_input(data, i-delete_amt, col_space-1, opts);
            for(j = tmp; j < col_space; j++)
              ADP_display_write(' ');
            if(delete_amt != 0)
              delete_state |= 2;
          }
        }
        else {
          delete_state |= 2;
          ADP_selector_set(UI_INPUT_DELETE_START_POS);
        }
      }
      else {
        if(delete_state & 1) {
          delete_state ^= 1;
          selector_set(pos - INPUT_CHAR_OFFSET, INPUT_CHAR_RANGE);
        }
        if((delete_state & 2) && btn_state_released()) {
          i -= delete_amt;
          refresh = 1;
          btn_state_init();
        }
        else {
          tmp_pos = INPUT_CHAR_OFFSET + (ADP_selector_position() % INPUT_CHAR_RANGE);

          if(tmp_pos != pos || refresh) {
            pos = tmp_pos;
            ADP_display_cursor_set(col, row);
            print_input(data, i, col_space-1, opts);
            ADP_display_write((pos == terminal_ch)? ok : pos);
            refresh = 0;
          }

          if(btn_state_released()) {
            if(pos == terminal_ch)
              goto done;
            data[i] = pos;
            brk = 1;
            btn_state_init();
          }
        }
      }
    }
  }

done:
  return i;
}
