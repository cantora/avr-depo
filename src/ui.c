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
  ui_btn_state state;
} g_btn;


static void btn_state_init() {
  g_btn.state = UI_BTN_STATE_OFF;
  g_btn.t = ADP_ts_millis();
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

struct ui_input_params {
  uint16_t col_space, col, row;
  uint8_t opts;
};

struct ui_input_state {
  uint32_t delete_amt;
  uint8_t *data;
  size_t i;
  uint8_t delete_state, pos, refresh;
};

#define UI_INPUT_DELETE_START_POS 1048576UL

static void ui_input_down(struct ui_input_state *state,
                          const struct ui_input_params *params) {
  uint32_t amt;
  uint16_t j;

  if((state->delete_state & 1) == 0) {
    state->delete_state = 1;
    ADP_selector_set(UI_INPUT_DELETE_START_POS);
    state->delete_amt = 0;
  }

  amt = UI_INPUT_DELETE_START_POS - ADP_selector_position();
  if(amt < UI_INPUT_DELETE_START_POS) {
    if(amt > state->i) {
      amt = state->i;
      ADP_selector_set(UI_INPUT_DELETE_START_POS - amt);
    }
    if(amt != state->delete_amt) {
      state->delete_amt = amt;
      ADP_display_cursor_set(params->col, params->row);
      j = print_input(state->data, state->i-state->delete_amt,
                        params->col_space-1, params->opts);
      for(; j < params->col_space; j++)
        ADP_display_write(' ');
      if(state->delete_amt != 0)
        state->delete_state |= 2;
    }
  }
  else {
    state->delete_state |= 2;
    ADP_selector_set(UI_INPUT_DELETE_START_POS);
  }
}

static uint8_t ui_input_not_down(struct ui_input_state *state,
                                 const struct ui_input_params *params) {
  uint8_t pos, terminal_ch, ok, ret;

  ret = -1;
  ok = ADP_display_ok_char();
  terminal_ch = ADP_display_input_terminal_char();

  if(state->delete_state & 1) {
    state->delete_state ^= 1;
    selector_set(state->pos - INPUT_CHAR_OFFSET, INPUT_CHAR_RANGE);
  }

  if((state->delete_state & 2) && btn_state_released()) {
    state->i -= state->delete_amt;
    state->refresh = 1;
    btn_state_init();
  }
  else {
    pos = INPUT_CHAR_OFFSET + (ADP_selector_position() % INPUT_CHAR_RANGE);

    if(pos != state->pos || state->refresh) {
      state->pos = pos;
      ADP_display_cursor_set(params->col, params->row);
      print_input(state->data, state->i, params->col_space-1, params->opts);
      ADP_display_write((state->pos == terminal_ch)? ok : state->pos);
      state->refresh = 0;
    }

    if(btn_state_released()) {
      if(state->pos == terminal_ch)
        return 0;
      state->data[state->i] = state->pos;
      ret = 1;
      btn_state_init();
    }
  }

  return ret;
}

size_t ui_input(uint16_t col, uint16_t row, /* starting col,row of interaction */
                uint8_t *data, size_t len,  /* buffer to write user input into */
                uint8_t opts) {             /* options, see enum */
  uint16_t cols, rows;
  uint8_t brk;
  struct ui_input_params params;
  struct ui_input_state state;

  display_dims(&cols, &rows);
  params.col_space = cols - col;
  if(params.col_space < 2)
    return 0;
  params.col = col;
  params.row = row;
  params.opts = opts;

  state.delete_amt = 0;
  state.delete_state = 0;
  state.data = data;
  state.pos = 0;

  selector_set(0x61 - INPUT_CHAR_OFFSET, INPUT_CHAR_RANGE);
  btn_state_init();

  for(state.i = 0; state.i < len; state.i++) {
    state.refresh = 1;
    brk = 0;
    while(brk == 0) {
      btn_state_update();

      if(btn_state_down()) {
        ui_input_down(&state, &params);
      }
      else {
        switch(ui_input_not_down(&state, &params)) {
        case 0:
          goto done;
        case 1:
          brk = 1;
          break;
        default:
          ; /* nothing */
        }
      }
    }
  }

done:
  return state.i;
}
