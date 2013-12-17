#include "btn.h"

#include "avr-depo.h"

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

inline void btn_state_init() {
  g_btn.state = UI_BTN_STATE_OFF;
  g_btn.t = ADP_ts_millis();
}

#define UI_BTN_EDGE_THRESH 350 /* milliseconds */

inline int btn_state_down() {
  return (g_btn.state == UI_BTN_STATE_HOLDING_A
          || g_btn.state == UI_BTN_STATE_HOLDING_B
          || g_btn.state == UI_BTN_STATE_TRIGGER);
}

inline int btn_state_released() {
  return (g_btn.state == UI_BTN_STATE_RELEASED);
}

void btn_state_update() {
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
