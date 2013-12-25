#ifndef AVR_DEPO_schema_h
#define AVR_DEPO_schema_h

#include <stdint.h>

struct schema {
  uint16_t keylen;
  uint16_t len;
  uint8_t id;
};

typedef enum {
  SCHEMA_ID_HEX = 0,
  SCHEMA_ID_PW0
} schema_id;

extern const char *schema_names[];

int schema_init(struct schema *sch, schema_id sid, uint16_t len);
static inline uint16_t schema_keylen(const struct schema *sch) { return sch->keylen; }
int schema_run(const struct schema *sch, const uint8_t *key, uint8_t *out);

#endif /* AVR_DEPO_schema_h */
