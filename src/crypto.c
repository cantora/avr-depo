#include "crypto.h"
#include "avr-depo-config.h"
#include "schema.h"
#include "util.h"
#include "avr-depo.h"

int crypto_gen(uint8_t *seed, uint16_t seedlen,
               const struct schema *sch, uint8_t *out,
               void (*cb)(uint32_t bits, void *),
               uint32_t cb_ms_ivl, void *user) {
  uint8_t *key;
  int status;

  if(schema_keylen(sch) < 1)
    return -1;

  if(schema_keylen(sch) > 128)
    return -1;
  
  key = malloc(schema_keylen(sch) * sizeof(uint8_t));
  if(key == NULL)
    return -1;

  status = 0;
  if(crypto_pbkdf2((const char *) seed, seedlen,
                   (const uint8_t *) AVR_DEPO_config_pbkdf2_salt,
                   AVR_DEPO_config_pbkdf2_saltlen,
                   AVR_DEPO_config_gen_pbkdf2_rounds,
                   sch->keylen, key,
                   cb, cb_ms_ivl, user) != 0) {
    status = -1;
    goto done;
  }

  if(schema_run(sch, key, out) != 0) {
    status = -1;
    goto done;
  }

done:
  free(key);
  return status;
}
