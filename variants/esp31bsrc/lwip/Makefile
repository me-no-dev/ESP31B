TOOLS_PATH ?= ../../../sdk/xtensa-esp108-elf/bin/xtensa-esp108-elf-
LWIP_LIB ?= liblwip_gcc.a
SDK_PATH ?= $(abspath ../../../sdk/)

BUILD_PATH = build
LWIP_SRCS = $(patsubst %.c,$(BUILD_PATH)/%.o,$(wildcard */*.c)) $(patsubst %.c,$(BUILD_PATH)/%.o,$(wildcard */*/*.c))

LWIP_INCLUDE = -Ibuild -I$(SDK_PATH)/include
BUILD_FLAGS = -c -Os -g -Wpointer-arith -Wno-implicit-function-declaration -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals -falign-functions=4 -MMD -std=gnu99 -ffunction-sections -fdata-sections
BUILD_DEFINES = -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ -DLWIP_OPEN_SRC

CC=$(TOOLS_PATH)gcc
AR=$(TOOLS_PATH)ar

$(BUILD_PATH)/%.h:
	@mkdir -p $(dir $@)
	@touch $@

$(BUILD_PATH)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(BUILD_FLAGS) $(BUILD_DEFINES) $(LWIP_INCLUDE) $< -o $@

$(BUILD_PATH)/$(LWIP_LIB): $(BUILD_PATH)/user_config.h $(LWIP_SRCS)
	$(AR) cru $(BUILD_PATH)/$(LWIP_LIB) $(LWIP_SRCS)

all: $(BUILD_PATH)/$(LWIP_LIB)

install: all
	cp -f $(BUILD_PATH)/$(LWIP_LIB) $(SDK_PATH)/lib/$(LWIP_LIB)

clean:
	@rm -rf $(BUILD_PATH)
	