#ifndef AVR_DEPO_ui_h
#define AVR_DEPO_ui_h

#include <stddef.h>
#include <stdint.h>

typedef enum ui_input_opts {
  UI_INPUT_OPTS_HIDE = 1 /* bit 0 */
};
size_t ui_input(uint16_t col, uint16_t row, uint8_t *data, size_t len, uint8_t opts);

#endif /* AVR_DEPO_ui_h */
