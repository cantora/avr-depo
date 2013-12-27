#ifndef AVR_DEPO_ui_h
#define AVR_DEPO_ui_h

#include <stddef.h>
#include <stdint.h>

typedef enum {
  UI_INPUT_OPTS_HIDE = 1 /* bit 0 */
} ui_input_opts;

struct ui_processing {
  uint16_t col, row;
  uint32_t total;
  uint8_t state;
};

size_t ui_input(uint16_t col, uint16_t row,
                uint8_t *data, size_t len,
                uint8_t opts);
uint8_t ui_menu(const char **items, size_t n_items);
int32_t ui_input_n(uint16_t col, uint16_t row,
                   int32_t min, int32_t max,
                   int32_t start);

uint16_t ui_option(uint16_t col, uint16_t row, const char **options, uint16_t n_options);
void ui_processing_init(struct ui_processing *proc, uint32_t total);
void ui_processing_update(uint32_t done, void *user);
void ui_wait_for_button_release();
int ui_print_scroll(uint16_t col, uint16_t row, uint16_t len, const char *s);

#endif /* AVR_DEPO_ui_h */
