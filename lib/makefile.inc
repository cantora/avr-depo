
ifndef AVR_DEPO_PLATFORM
$(error "AVR_DEPO_PLATFORM must be defined")
endif

ifndef AVR_DEPO_PLATFORM_MCU
$(error "AVR_DEPO_PLATFORM_MCU must be defined")
endif

ifndef AVR_DEPO_PLATFORM_CPU
$(error "AVR_DEPO_PLATFORM_CPU must be defined")
endif

ifndef AVR_DEPO_PLATFORM_BUILD_DIR
$(error "AVR_DEPO_PLATFORM_BUILD_DIR must be defined")
endif
ifeq ($(strip $(AVR_DEPO_PLATFORM_BUILD_DIR)),)
$(error "AVR_DEPO_PLATFORM_BUILD_DIR is empty")
endif

AVR_DEPO_LIB_DIR := $(realpath $(dir $(realpath $(lastword $(MAKEFILE_LIST)))))
THIS_MAKEFILE    := $(AVR_DEPO_LIB_DIR)/makefile.inc

AVR_CRYPTO_LIB_DIR  := $(AVR_DEPO_LIB_DIR)/avr-crypto-lib
include $(AVR_CRYPTO_LIB_DIR)/gmsl

AVR_DEPO_LIB_NAMES    = avr-crypto-lib
AVR_DEPO_LIBS    = $(foreach lib, $(AVR_DEPO_LIB_NAMES), $(AVR_DEPO_PLATFORM_BUILD_DIR)/$(lib).a)

AVR_CRYPTO_LIB_MODS = hmac-sha1
AVR_CRYPTO_LIB_BASE_MAKEFILE = $(AVR_DEPO_LIB_DIR)/avr-crypto-lib_base.mk

CRYPTO_LIB_MAKEFILE = $(AVR_CRYPTO_LIB_DIR)/Makefile_$(AVR_DEPO_PLATFORM)_conf.inc

include $(foreach mod, $(AVR_CRYPTO_LIB_MODS), $(AVR_CRYPTO_LIB_DIR)/mkfiles/$(mod).mk)
AVR_CRYPTO_LIB_INCLUDES = $(addprefix -iquote$(AVR_CRYPTO_LIB_DIR)/, $(foreach mod, $(AVR_CRYPTO_LIB_MODS), $($(call uc,$(mod))_INCDIR) ))
AVR_DEPO_INCLUDES = $(AVR_CRYPTO_LIB_INCLUDES)

$(AVR_CRYPTO_LIB_DIR):
	#git clone 'git@github.com:cantora/avr-crypto-lib.git'
	cd $(AVR_DEPO_LIB_DIR) && ln -s ~/proj/avr-crypto-lib

$(CRYPTO_LIB_MAKEFILE): $(THIS_MAKEFILE) $(AVR_CRYPTO_LIB_BASE_MAKEFILE)
	tmpfile=$$(mktemp) && \
	echo 'include Makefile_sys_conf.inc' >> $$tmpfile && \
	echo 'MCU_TARGET     = $(AVR_DEPO_PLATFORM_MCU)' >> $$tmpfile && \
	echo 'F_CPU          = $(AVR_DEPO_PLATFORM_CPU)' >> $$tmpfile && \
	echo 'BOARD_NAME     = $(AVR_DEPO_PLATFORM)' >> $$tmpfile && \
	echo 'BIN_DIR        = $(AVR_DEPO_PLATFORM_BUILD_DIR)/' >> $$tmpfile && \
	echo 'AVR_CRYPTO_LIB_MODS = $(AVR_CRYPTO_LIB_MODS)' >> $$tmpfile && \
	cat $(AVR_CRYPTO_LIB_BASE_MAKEFILE) >> $$tmpfile && \
	echo 'include Makefile_main.inc' >> $$tmpfile && \
	cp $$tmpfile $@

$(AVR_DEPO_PLATFORM_BUILD_DIR)/avr-crypto-lib.a: $(AVR_CRYPTO_LIB_DIR) $(CRYPTO_LIB_MAKEFILE)
	cd $(AVR_CRYPTO_LIB_DIR) && $(MAKE) $(MFLAGS) --file $(CRYPTO_LIB_MAKEFILE) lib

.PHONY: clean-libs
clean-libs:
	for mod in '$(AVR_CRYPTO_LIB_MODS)'; do \
		dir=$(AVR_DEPO_PLATFORM_BUILD_DIR)/$$mod; \
		if [ -d $$dir ]; then \
			find  -type f -iname '*.o' -delete; \
		fi; \
	done
