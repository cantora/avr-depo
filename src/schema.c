#include "schema.h"
#include "util.h"
#include "avr-depo.h"

#include <stdlib.h>

const char *schema_names[] = {
  "hex"
};

static inline int schema_hex_init(struct schema *sch) {
  if(sch->len < 2)
    return -1;
  if(sch->len % 2 != 0)
    return -1;

  sch->keylen = sch->len >> 1; /* divide by two */
  return 0;
}

static inline int schema_hex_run(const struct schema *sch,
                                 const uint8_t *key, uint8_t *out) {
  uint16_t i, len;
  uint8_t *p;

  len = sch->len >> 1; /* divide by 2 */
  if(len > sch->keylen)
    return -1;

  for(i = 0; i < len; i++) {
    p = out+(i*2);
    if(key[i] <= 0xf) {
      *p = '0';
      p++;
    }
    utoa(key[i], (char *) p, 16);
  }

  return 0;
}

int schema_init(struct schema *sch, schema_id sid, uint16_t len) {
  int status;

  sch->len = len;
  sch->id = sid;

  switch(sch->id) {
  case SCHEMA_ID_HEX:
    status = schema_hex_init(sch);
    break;
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
  default:
    status = -1;
  }

  return status;
}
