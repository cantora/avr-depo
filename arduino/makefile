BOARD_TAG          = uno
AVR_DEPO_PLATFORM  = arduino-$(BOARD_TAG)
AVR_DEPO_PLATFORM_MCU = $(MCU)
AVR_DEPO_PLATFORM_CPU = $(F_CPU)
AVR_DEPO_PLATFORM_BUILD_DIR = $(realpath .)/build-$(BOARD_TAG)

#install from github.com/sudar/Arduino-Makefile
ARDUINO_DIR        = /usr/share/arduino
ARDUINO_MK_DIR     = ../../Arduino-Makefile
ARDUINO_PORT       = /dev/ttyACM0
USER_LIB_PATH      = ./lib
ARDUINO_LIBS       = LiquidCrystal 
TARGET             = avr-depo
MONITOR_BAUDRATE   = 115200
OPTIMIZATION_LEVEL = 3
AVR_DEPO_SRC       = $(realpath ../src)
EXTRA_FLAGS        = -iquote$(AVR_DEPO_SRC) -I$(AVR_DEPO_LIB_DIR)
AVR_DEPO_SRCS      = $(notdir $(wildcard $(AVR_DEPO_SRC)/*.c))
AVR_DEPO_OBJS      = $(patsubst %.c,%.o,$(AVR_DEPO_SRCS))

OTHER_OBJS         = $(patsubst %.o,$(OBJDIR)/%.o,$(AVR_DEPO_OBJS)) $(AVR_DEPO_LIBS)

include $(ARDUINO_MK_DIR)/arduino-mk/Arduino.mk
include ../lib/makefile.inc

$(TARGET_ELF): $(AVR_DEPO_LIBS)

$(OBJDIR)/%.o: $(AVR_DEPO_SRC)/%.c $(COMMON_DEPS) | $(OBJDIR)
	@$(MKDIR) $(dir $@)
	$(CC) -MMD -c $(CPPFLAGS) $(CFLAGS) $< -o $@

.PHONY: clean-deps
clean-deps:
	find $(AVR_DEPO_PLATFORM_BUILD_DIR) -type f -iname '*.d' -delete

clean: clean-deps clean-libs

