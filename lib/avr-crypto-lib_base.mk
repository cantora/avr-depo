
include gmsl

OPTIMIZE            = -Os
DEFS                = -D$$(call uc, $$(MCU_TARGET)) -DF_CPU=$$(F_CPU)

override CFLAGS_A   = -MMD -MF$(DEP_DIR)$(patsubst %.o,%.d,$(notdir $(1))) \
                      $(DEBUG) $(WARNING) -std=$(CSTD) $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override CFLAGS     = -MMD -MF$(DEP_DIR)$(patsubst %.o,%.d,$(notdir $@)) \
                      $(DEBUG) $(WARNING) -std=$(CSTD) $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)

override ASFLAGS    = -mmcu=$(MCU_TARGET)

.PHONY: lib
lib: $(BIN_DIR)/avr-crypto-lib.a

$(BIN_DIR)/avr-crypto-lib.a: $(foreach mod, $(AVR_CRYPTO_LIB_MODS), $(call uc, $(mod))_OBJ)
	rm -f $@
	ar rcs $@ $(foreach dir, $(AVR_CRYPTO_LIB_MODS), $(shell find $(BIN_DIR)/$(dir) -name '*.o') )

