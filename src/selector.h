#ifndef AVR_DEPO_selector_h
#define AVR_DEPO_selector_h

static inline void selector_set_u(uint32_t val) {
  ADP_selector_set(val);
}

static inline void selector_set_s(int32_t val) {
  ADP_selector_set(val);
}

static inline uint32_t selector_position_u() {
  return ADP_selector_position();
}

static inline int32_t selector_position_s() {
  return ADP_selector_position();
}

static inline void selector_set_range(uint32_t val, uint32_t range) {
  selector_set_u(range*1000UL + val);
}

#endif /* AVR_DEPO_selector_h */