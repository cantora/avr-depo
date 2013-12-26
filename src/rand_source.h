#ifndef AVR_DEPO_rand_source_h
#define AVR_DEPO_rand_source_h

#include "crypto.h"

struct rand_source {
  struct crypto_pbkdf2 kdf;
  uint16_t data_idx;
  uint8_t data[AVR_DEPO_PBKDF2_DIGEST_BYTES];
};

int rand_source_init(struct rand_source *src,
                     const uint8_t *bytes, uint16_t len,
                     void (*cb)(uint32_t bits, void *user),
                     uint32_t cb_ms_ivl, void *user);

void rand_source_free(struct rand_source *src);
uint8_t rand_source_uint8(struct rand_source *src);
uint32_t rand_source_uint32(struct rand_source *src);
int rand_source_choose_k(struct rand_source *src, uint16_t max,
                         uint16_t k, uint16_t *result);
void rand_source_shuffle(struct rand_source *src, uint8_t *bytes,
                         uint16_t len);
void rand_source_char_range(struct rand_source *src, uint8_t *out,
                           uint8_t offset, uint8_t range);

void rand_source_char_upper(struct rand_source *src, uint8_t *upper);
void rand_source_char_lower(struct rand_source *src, uint8_t *lower);
void rand_source_char_numeric(struct rand_source *src, uint8_t *numeric);
void rand_source_char_symbolic(struct rand_source *src, uint8_t *symbolic);

#endif /* AVR_DEPO_rand_source_h */
