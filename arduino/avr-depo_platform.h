#ifndef AVR_DEPO_avr_depo_platform_h
#define AVR_DEPO_avr_depo_platform_h

#include "encoder.h"
#include "button.h"

#ifdef __cplusplus
extern "C" {
#endif

void serial_print(const char *);
void lcd_set_cursor(uint16_t, uint16_t);
void lcd_cursor(uint8_t);
void lcd_write(char);
void lcd_clear();

#define ADP_selector_position() encoder_position()
#define ADP_selector_set(pos) encoder_set(pos)
#define ADP_button_check() button_check()
#define ADP_debug_print(str) serial_print(str)
#define ADP_display_dims(cols_p, rows_p) \
  do { \
    *(cols_p) = 16; \
    *(rows_p) = 2; \
  } while(0)
#define ADP_display_cursor_set(col, row) lcd_set_cursor(col, row)
#define ADP_display_cursor(on) lcd_cursor(on)
#define ADP_display_write(c) lcd_write(c)
#define ADP_display_clear() lcd_clear()

#ifdef __cplusplus
}
#endif

#endif /* AVR_DEPO_avr_depo_platform_h */
