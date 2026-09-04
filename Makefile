CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
RM      := rm -f
CURL    := curl -fsSL

FIRM_DIR := firm/arm9
START_S  := $(FIRM_DIR)/start.s
SOURCE   := $(FIRM_DIR)/main.c
HOME_A   := $(FIRM_DIR)/home_a.c
HOME_B   := $(FIRM_DIR)/home_b.c
LINKER   := $(FIRM_DIR)/link.ld
ELF      := arm9.elf
BIN      := arm9.bin
TARGET   := Omni10.firm
ENTRY    := 0x08000040
GOOD_MAIN := https://raw.githubusercontent.com/Omni-3DS/Omni10-3DS/a199a2616931bd534ecb166eccb717af23f3a569/firm/arm9/main.c

CFLAGS  := -Wall -O2 -marm -fomit-frame-pointer -nostdlib -march=armv5te \
           -fno-builtin-memset -fno-builtin-memcpy -fno-builtin-strlen \
           -fno-builtin-strncmp -ffreestanding
ASFLAGS := -marm -march=armv5te
LDFLAGS := -T $(LINKER) -nostdlib -Wl,--nmagic

.PHONY: all clean firm check ensure-main

all: firm

ensure-main:
	@if [ -f $(HOME_A) ] && [ -f $(HOME_B) ] && grep -q 'screen_scripts_hub' $(HOME_B) 2>/dev/null; then \
	  cat $(HOME_A) $(HOME_B) > $(SOURCE); \
	  echo "[OK] main.c from home_a+home_b (HOME scripts)"; \
	elif ! grep -q 'int main' $(SOURCE) 2>/dev/null; then \
	  echo "[!] restoring known-good main.c"; \
	  $(CURL) -o $(SOURCE) $(GOOD_MAIN); \
	fi
	@grep -q 'int main' $(SOURCE)

firm: ensure-main $(TARGET)

$(TARGET): $(START_S) $(SOURCE) $(LINKER)
	@echo "=== Omni10 FIRM ==="
	@$(RM) $(ELF) $(BIN) $(TARGET)
	$(CC) $(CFLAGS) $(ASFLAGS) $(LDFLAGS) $(START_S) $(SOURCE) -o $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(BIN)
	firmtool build $(TARGET) -i -n $(ENTRY) -e 0 -D $(BIN) -A $(ENTRY) -C NDMA
	@wc -c $(BIN) $(TARGET)
	@echo "[OK] $(TARGET)"

check:
	@command -v $(CC) >/dev/null && echo "[OK] gcc" || echo "[!!] gcc"
	@command -v firmtool >/dev/null && echo "[OK] firmtool" || echo "[!!] firmtool"

clean:
	@$(RM) $(ELF) $(BIN) $(TARGET)
