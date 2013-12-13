#install from github.com/sudar/Arduino-Makefile
ARDUINO_DIR        = /usr/share/arduino
ARDUINO_MK_DIR     = ../../Arduino-Makefile
BOARD_TAG          = uno
ARDUINO_PORT       = /dev/ttyACM0
USER_LIB_PATH      = ./lib
ARDUINO_LIBS       = LiquidCrystal 
TARGET             = avr-depo
MONITOR_BAUDRATE   = 115200
OPTIMIZATION_LEVEL = 3
AVR_DEPO_SRC       = $(realpath ../src)
EXTRA_FLAGS        = -iquote$(AVR_DEPO_SRC)
AVR_DEPO_SRCS      = $(notdir $(wildcard $(AVR_DEPO_SRC)/*.c))
AVR_DEPO_OBJS      = $(patsubst %.c,%.o,$(AVR_DEPO_SRCS))

OTHER_OBJS         = $(patsubst %.o,$(OBJDIR)/%.o,$(AVR_DEPO_OBJS))

include $(ARDUINO_MK_DIR)/arduino-mk/Arduino.mk

$(OBJDIR)/%.o: $(AVR_DEPO_SRC)/%.c $(COMMON_DEPS) | $(OBJDIR)
	@$(MKDIR) $(dir $@)
	$(CC) -MMD -c $(CPPFLAGS) $(CFLAGS) $< -o $@
