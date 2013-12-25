#ifndef AVR_DEPO_rand_source_h
#define AVR_DEPO_rand_source_h

struct rand_source {
  const uint8_t *bytes;
  uint16_t len;
  uint16_t idx;  
};

void rand_source_init(struct rand_source *src,
                      const uint8_t *bytes, uint16_t len);

int rand_source_empty(const struct rand_source *src);
uint32_t rand_source_uint32(struct rand_source *src);
int rand_source_choose_k(struct rand_source *src, uint16_t max,
                         uint16_t k, uint16_t *result);
int rand_source_shuffle(struct rand_source *src, uint8_t *bytes,
                        uint16_t len);
int rand_source_char_range(struct rand_source *src, uint8_t *out,
                           uint8_t offset, uint8_t range);

int rand_source_char_upper(struct rand_source *src, uint8_t *upper);
int rand_source_char_lower(struct rand_source *src, uint8_t *lower);
int rand_source_char_numeric(struct rand_source *src, uint8_t *numeric);
int rand_source_char_symbolic(struct rand_source *src, uint8_t *symbolic);

#endif /* AVR_DEPO_rand_source_h */
