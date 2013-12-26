#ifndef AVR_DEPO_crypto_h
#define AVR_DEPO_crypto_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "schema.h"

#define AVR_DEPO_PBKDF2_DIGEST_BYTES 20
#define AVR_DEPO_PBKDF2_BLOCK_BYTES 64 
#define AVR_DEPO_PBKDF2_MAX_SALT_BYTES (AVR_DEPO_PBKDF2_BLOCK_BYTES - 28 - 4)

struct crypto_pbkdf2 {
  uint32_t i;
  uint32_t iter;
  const uint8_t *pass;  
  const uint8_t *salt;
  uint8_t *salt_itmp;
  uint16_t passlen;
  uint16_t saltlen;
  void (*cb)(uint32_t bits, void *user);
  uint32_t cb_ms_ivl;
  uint32_t t;
  void *user;
};

int crypto_pbkdf2_init(struct crypto_pbkdf2 *kdf,
                       uint32_t iters_per_block,
                       const uint8_t *pass, uint16_t passlen,
                       const uint8_t *salt, uint16_t saltlen,
                       void (*cb)(uint32_t bits, void *user),
                       uint32_t cb_ms_ivl, void *user);
void crypto_pbkdf2_free(struct crypto_pbkdf2 *kdf);
uint16_t crypto_pbkdf2_blocklen(struct crypto_pbkdf2 *kdf);
void crypto_pbkdf2_block(struct crypto_pbkdf2 *kdf, uint8_t *out);
int crypto_pbkdf2(const uint8_t *pass, uint16_t passlen,
                  const uint8_t *salt, uint16_t saltlen,
                  uint32_t iter, uint16_t keylen, uint8_t *out,
                  void (*cb)(uint32_t bits, void *user),
                  uint32_t cb_ms_ivl, void *user);

#endif /* AVR_DEPO_crypto_h */
