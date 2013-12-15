#include "encoder.h"

#include <Arduino.h>

/* based on the example at http://www.circuitsathome.com/mcu/programming/reading-rotary-encoder-on-arduino 
 * and the modification on the arduino website.
 */

#define ENCODER_DOWNSHIFT ENCODER_PIN_A_OFFSET
#define ENCODER_MASK (0b11 << ENCODER_DOWNSHIFT)

/* up:   00 -> 10 -> 11 -> 01 -> 00 -> ... 
 * down: 00 -> 01 -> 11 -> 10 -> 00 -> ... */
static const int8_t STATES[] = {
  0,   /* 00 -> 00 */
  -1,  /* 00 -> 01 */
  1,   /* 00 -> 10 */
  0,   /* 00 -> 11 */
  1,   /* 01 -> 00 */
  0,   /* 01 -> 01 */
  0,   /* 01 -> 10 */
  -1,  /* 01 -> 11 */
  -1,  /* 10 -> 00 */
  0,   /* 10 -> 01 */
  0,   /* 10 -> 10 */
  1,   /* 10 -> 11 */
  0,   /* 11 -> 00 */
  1,   /* 11 -> 01 */
  -1,  /* 11 -> 10 */
  0    /* 11 -> 11 */
};

static uint8_t g_state = 0;
static uint32_t g_position = 0;

void encoder_update() {
  uint8_t new_state;

  new_state = (ENCODER_PORT & ENCODER_MASK) >> ENCODER_DOWNSHIFT;
  /* shift out previous state, making current state the previous state */
  g_state <<= 2;
  /* add in new state a the least significant 2 bits */
  g_state |= new_state;
  /* get rid of high bits */
  g_state &= 0b1111;
  g_position += STATES[g_state];
}

uint32_t encoder_position() {
  return g_position >> 2;
}

void encoder_set(uint32_t position) {
  /* add two to put it in a stable position */
  g_position = (position << 2) + 2;
}