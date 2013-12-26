#include "schema.h"
#include "util.h"
#include "avr-depo.h"
#include "rand_source.h"
#include "avr-depo-config.h"

#include <stdlib.h>

const char *schema_names[] = {
  "hex",
  "pw0"
};

static void schema_update(uint32_t bytes, void *user) {
  struct schema *sch = (struct schema *) user;
  (void)(bytes);

  sch->cb(sch->user);
}

/************************************************************************
 ************************************************************************
 * schema which converts the generated key bytes into hex characters
 */
static inline int schema_hex_run(const struct schema *sch, 
                                 struct rand_source *src, uint8_t *out) {
  uint16_t i, len;

  len = sch->len >> 1; /* divide by 2 */

  for(i = 0; i < len; i++)
    util_byte_to_hex(rand_source_uint8(src), out+(i*2));

  /* if an odd number of bytes are needed */
  if(sch->len % 2 != 0)
    util_msnibble_to_hex(rand_source_uint8(src), out+(i*2));

  return 0;
}

/************************************************************************
 ************************************************************************
 * schema which uses the key bytes to make random decisions required for
 * generating a random password. random password contains at least one
 * upper case, lower case, number, and symbol character.
 */
   
/*   . . . . . . . .
 * [0|1|2|3|4|5|6|7|8] */
static inline int schema_pw0_run(const struct schema *sch,
                                 struct rand_source *src, uint8_t *out) {
  uint16_t subdivs[3];
  uint16_t i;

  if(rand_source_choose_k(src, sch->len-1, 3, subdivs) != 0)
    return -1;

  for(i = 0; i < subdivs[0]+1; i++)
    rand_source_char_upper(src, out+i);
  for(; i < subdivs[1]+1; i++)
    rand_source_char_lower(src, out+i);
  for(; i < subdivs[2]+1; i++)
    rand_source_char_numeric(src, out+i);
  for(; i < sch->len; i++)
    rand_source_char_symbolic(src, out+i);

  rand_source_shuffle(src, out, sch->len);

  return 0;
}

void schema_init(struct schema *sch, schema_id sid, uint16_t len,
                 uint8_t *key, uint16_t keylen,
                 void (*cb)(void *), uint32_t cb_ms_ivl,
                 void *user) {
  sch->key = key;
  sch->keylen = keylen;
  sch->len = len;
  sch->id = sid;
  sch->cb = cb;
  sch->cb_ms_ivl = cb_ms_ivl;
  sch->user = user;
}

/* out must have enough space for sch->len bytes */
int schema_run(const struct schema *sch, uint8_t *out) {
  int status;
  struct rand_source src;

  if(rand_source_init(&src, sch->key, schema_keylen(sch),
                      schema_update, sch->cb_ms_ivl,
                      (void *) sch) != 0)
    return -1;

#define SCHEMA_RUN_CASE(schema_id, name) \
  case schema_id: \
    status = schema_##name##_run(sch, &src, out); \
    break

  switch(sch->id) {
  SCHEMA_RUN_CASE(SCHEMA_ID_HEX, hex);
  SCHEMA_RUN_CASE(SCHEMA_ID_PW0, pw0);
  default:
    status = -1;
  }

  rand_source_free(&src);
  return status;
}
