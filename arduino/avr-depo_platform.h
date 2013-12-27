#ifndef AVR_DEPO_ARDUINO_avr_depo_platform_h
#define AVR_DEPO_ARDUINO_avr_depo_platform_h

#include "encoder.h"
#include "button.h"

#ifdef __cplusplus
extern "C" {
#endif

void serial_print(const char *);
void serial_nprint(size_t, const char *);
void lcd_set_cursor(uint16_t, uint16_t);
void lcd_cursor(uint8_t);
void lcd_blink(uint8_t);
void lcd_write(char);
void lcd_clear();
void arduino_delay(uint32_t m);
uint32_t arduino_millis();

#define ADP_selector_position() encoder_position()
#define ADP_selector_set(pos) encoder_set(pos)
#define ADP_button_check() button_check()
#define ADP_debug_print(str) serial_print(str)
#define ADP_debug_nprint(sz, str) serial_nprint(sz, str)
#define ADP_display_dims(cols_p, rows_p) \
  do { \
    *(cols_p) = 16; \
    *(rows_p) = 2; \
  } while(0)
#define ADP_display_cursor_set(col, row) lcd_set_cursor(col, row)
#define ADP_display_cursor(on) lcd_cursor(on)
#define ADP_display_blink(on) lcd_blink(on)
#define ADP_display_write(c) lcd_write(c)
#define ADP_display_clear() lcd_clear()
/* LCD:
 *   db = hollow square
 *   a5 = dot
 *   f8 = X hat
 *   ff = solid block
 *
 *   note: 5c ('\') shows up as the yen symbol, so we
 *         use it as the terminal character (thus it
 *         is an illegal character for passwords).
 */
#define ADP_display_ok_char() 0xff /* solid block */
#define ADP_display_input_terminal_char() 0x5c /* '\' */
#define ADP_display_left_char() 0x7f
#define ADP_display_right_char() 0x7e
#define ADP_delay(m) arduino_delay(m)
#define ADP_ts_millis() arduino_millis()

#ifdef __cplusplus
}
#endif

#endif /* AVR_DEPO_ARDUINO_avr_depo_platform_h */
