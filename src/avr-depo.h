#ifndef AVR_DEPO_avr_depo_h
#define AVR_DEPO_avr_depo_h

#ifdef __cplusplus
extern "C" {
#endif

#include "avr-depo_platform.h"

#ifndef ADP_selector_position
#  error "platform: define a uint32_t ADP_selector_position()"
#endif

#ifndef ADP_selector_set
#  error "platform: define a void ADP_selector_set(uint32_t pos)"
#endif

#ifndef ADP_button_check
#  error "platform: define a uint8_t ADP_button_check()"
#endif

#ifndef ADP_debug_print
#  error "platform: define a void ADP_debug_print(const char *)"
#endif

#ifndef ADP_display_dims
#  error "platform: define a void ADP_display_dims(uint16_t *cols, uint16_t *rows) "
#endif

#ifndef ADP_display_cursor_set
#  error "platform: define a void ADP_display_cursor_set(uint16_t col, uint16_t row)"
#endif

#ifndef ADP_display_cursor
#  error "platform: define a void ADP_display_cursor(uint8_t on)"
#endif

#ifndef ADP_display_blink
#  error "platform: define a void ADP_display_blink(uint8_t on)"
#endif

#ifndef ADP_display_write
#  error "platform: define a void ADP_display_write(char)"
#endif

#ifndef ADP_display_clear
#  error "platform: define a void ADP_display_clear()"
#endif

#ifndef ADP_delay
#  error "platform: define a void ADP_delay(uint32_t millis)"
#endif

#ifndef ADP_ts_millis
#  error "platform: define a uint32_t ADP_ts_millis()"
#endif

#ifdef __cplusplus
}
#endif

#endif /* AVR_DEPO_depo_main_h */
