#include "schema.h"
#include "util.h"
#include "avr-depo.h"
#include "rand_source.h"

#include <stdlib.h>

const char *schema_names[] = {
  "hex",
  "pw0"
};

/************************************************************************
 ************************************************************************
 * schema which converts the generated key bytes into hex characters
 */
static inline int schema_hex_init(struct schema *sch) {
  if(sch->len < 1)
    return -1;

  sch->keylen = (sch->len >> 1); /* divide by two */
  if(sch->len % 2 != 0)
    sch->keylen++;

  return 0;
}

static inline int schema_hex_run(const struct schema *sch,
                                 const uint8_t *key, uint8_t *out) {
  uint16_t i, len;

  len = sch->len >> 1; /* divide by 2 */
  if(len > sch->keylen)
    return -1;

  for(i = 0; i < len; i++)
    util_byte_to_hex(key[i], out+(i*2));

  /* if an odd number of bytes are needed */
  if(sch->len % 2 != 0)
    util_msnibble_to_hex(key[i], out+(i*2));

  return 0;
}

/************************************************************************
 ************************************************************************
 * schema which uses the key bytes to make random decisions required for
 * generating a random password. random password contains at least one
 * upper case, lower case, number, and symbol character.
 */
   
static inline schema_pw0_init(struct schema *sch) {
  if(sch->len < 4 || sch->len > 16) /* 16*4 + 15*4*2 = 184 */
    return -1;

  sch->keylen = (sch->len-1)*4     /* choose 3 integers in (0..sch-len-1) */
                + sch->len*4       /* choose representatives from their classes */
                + (sch->len-1)*4;  /* shuffle the result array */
  return 0;
}

/*   . . . . . . . .
 * [0|1|2|3|4|5|6|7|8] */
static inline int schema_pw0_run(const struct schema *sch,
                             const uint8_t *key, uint8_t *out) {
  struct rand_source src;
  uint16_t subdivs[3]
  uint16_t i;

  rand_source_init(&src, key, schema_keylen(sch));

  if(rand_source_choose_k(&src, sch->len-1, 3, subdivs) != 0)
    return -1;

  for(i = 0; i < subdivs[0]+1; i++) {
    if(rand_source_char_upper(&src, out+i) != 0)
      return -1;
  }
  for(; i < subdivs[1]+1; i++) {
    if(rand_source_char_lower(&src, out+i) != 0)
      return -1;
  }
  for(; i < subdivs[2]+1; i++) {
    if(rand_source_char_numeric(&src, out+i) != 0)
      return -1;
  }
  for(; i < sch->len; i++) {
    if(rand_source_char_symbolic(&src, out+i) != 0)
      return -1;
  }

  if(rand_source_shuffle(&src, out, sch->len) != 0)
    return -1;

  return 0;  
}

int schema_init(struct schema *sch, schema_id sid, uint16_t len) {
  int status;

  sch->len = len;
  sch->id = sid;

#define SCHEMA_INIT_CASE(schema_id) \
  case schema_id: \
    status = schema_##schema_id##_init(sch); \
    break;

  switch(sch->id) {
  SCHEMA_INIT_CASE(SCHEMA_ID_HEX)
  SCHEMA_INIT_CASE(SCHEMA_ID_PW0)
  default:
    status = -1;
  }

  return status;
}

/* out must have enough space for sch->len bytes */
int schema_run(const struct schema *sch, const uint8_t *key, uint8_t *out) {
  int status;

  switch(sch->id) {
  case SCHEMA_ID_HEX:
    status = schema_hex_run(sch, key, out);
    break;
  case SCHEMA_ID_PW0:
    status = schema_pw0_run(sch, key, out);
    break;
  default:
    status = -1;
  }

  return status;
}
