#ifndef AVR_DEPO_display_h
#define AVR_DEPO_display_h

void display_dims(uint16_t *cols, uint16_t *rows);
void display_print(uint16_t col, uint16_t row, const char *s);
void display_print_n(uint16_t col, uint16_t row, uint32_t n, int radix);
int display_print_chunk(uint16_t col, uint16_t row, const char *s);

#endif /* AVR_DEPO_display_h */
