#ifndef AVR_DEPO_crypto_h
#define AVR_DEPO_crypto_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define AVR_DEPO_PBKDF2_DIGEST_BYTES 20
#define AVR_DEPO_PBKDF2_BLOCK_BYTES 64 
#define AVR_DEPO_PBKDF2_MAX_SALT_BYTES (AVR_DEPO_PBKDF2_BLOCK_BYTES - 28 - 4)

int crypto_gen(uint8_t *seed, uint16_t seedlen,
               const struct schema *sch, const uint8_t *out,
               void (*cb)(uint8_t percent), uint32_t cb_ms_ivl);

int crypto_pbkdf2(const char *pass, uint16_t passlen,
                  const uint8_t *salt, uint16_t saltlen,
                  uint32_t iter, uint16_t keylen, uint8_t *out,
                  void (*cb)(uint16_t bits, void *user),
                  uint32_t cb_ms_ivl, void *user);

#endif /* AVR_DEPO_crypto_h */
