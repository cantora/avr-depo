#ifndef AVR_DEPO_schema_h
#define AVR_DEPO_schema_h

#include <stdint.h>

struct schema {
  uint8_t *key;
  uint16_t keylen;
  uint16_t len;
  uint8_t id;
  void (*cb)(void *);
  uint32_t cb_ms_ivl;
  void *user;
};

typedef enum {
  SCHEMA_ID_HEX = 0,
  SCHEMA_ID_PW0
} schema_id;

extern const char *schema_names[];

void schema_init(struct schema *sch, schema_id sid, uint16_t len,
                 uint8_t *key, uint16_t keylen,
                 void (*cb)(void *), uint32_t cb_ms_ivl,
                 void *user);

static inline uint16_t schema_keylen(const struct schema *sch) { return sch->keylen; }
int schema_run(const struct schema *sch, uint8_t *out);

#endif /* AVR_DEPO_schema_h */
