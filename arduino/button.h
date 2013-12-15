#ifndef AVR_DEPO_button_h
#define AVR_DEPO_button_h

#include <Arduino.h>

#define BUTTON_PIN_OFFSET 2
#define BUTTON_PORT PINB
#define BUTTON_PORT_OFFSET 8

#define BUTTON_PIN (BUTTON_PORT_OFFSET + BUTTON_PIN_OFFSET)
#define BUTTON_DOWNSHIFT BUTTON_PIN_OFFSET
#define BUTTON_MASK (0b1 << BUTTON_DOWNSHIFT)

static inline uint8_t button_check() {
  return !((BUTTON_PORT & BUTTON_MASK) >> BUTTON_DOWNSHIFT);
}

#endif /* AVR_DEPO_button_h */
