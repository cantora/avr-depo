#include "crypto.h"
#include "avr-depo-config.h"
#include "schema.h"

void crypto_gen_update(uint16_t bits, void *user) {
  void (*cb)(uint8_t percent);
  uint16_t total_bits;
  float percent;

  total_bits = AVR_DEP_PBKDF2_DIGEST_BYTES << 8;
  percent = bits/((float) total_bits);
  cb = user;

  cb(percent*100);
}

int crypto_gen(uint8_t *seed, uint16_t seedlen,
               const struct schema *sch, const uint8_t *out,
               void (*cb)(uint8_t percent), uint32_t cb_ms_ivl) {
  if(sch->keylen < 1)
    return -1;

  if(crypto_pbkdf2((const char *) buf, buf_len,
                   (const uint8_t *) AVR_DEPO_config_pbkdf2_salt,
                   AVR_DEPO_config_pbkdf2_saltlen,
                   AVR_DEPO_config_gen_pbkdf2_rounds,
                   sch->keylen, sch->key,
                   generate_update, cb_ms_ivl, cb) != 0)
    return -1;

  return schema_run(sch, out);
}

