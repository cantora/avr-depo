#ifndef AVR_DEPO_schema_h
#define AVR_DEPO_schema_h

struct schema {
  uint8_t *key;
  uint16_t keylen;
  uint16_t len;
  uint8_t id;
};

typedef enum {
  SCHEMA_ID_HEX = 0
} schema_id;

const char *schema_names[] {
  "hex"
};

int schema_init(struct schema *sch, schema_id sid, uint16_t len);
int schema_run(const struct schema *sch, uint8_t *out);

#endif /* AVR_DEPO_schema_h */
