#include "schema.h"

static inline int schema_hex_init(struct schema *sch) {
  if(sch->len < 2)
    return -1;
  if(sch->len % 2 != 0)
    return -1;

  sch->keylen = sch->len >> 1; /* divide by two */
  return 0;
}

static inline int schema_hex_run(struct schema *sch, uint8_t *out) {
  uint16_t i;

  if(sch->len < sch->keylen*2)
    return -1;

  for(i = 0; i < sch->keylen; i++) {
    utoa(sch->key[i], out+(i*2), 16);
  }

  return 0;
}

int schema_init(struct schema *sch, schema_id sid, uint16_t len) {
  int status;

  sch->key = NULL;
  sch->len = len;
  sch->id = sid;

  switch(sch->id) {
  case SCHEMA_ID_DIGEST:
    status = schema_hex_init(sch);
    break;
  default:
    status = -1;
  }

  return status;
}

/* out must have enough space for sch->len bytes */
int schema_run(const struct schema *sch, uint8_t *out) {
  int status;

  switch(sch->id) {
  case SCHEMA_ID_HEX:
    status = schema_hex_run(sch, out);
    break;
  default:
    status = -1;
  }

  return status;
}
